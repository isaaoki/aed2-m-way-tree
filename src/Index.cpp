// Index.cpp
// Index usage and manipulation
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <limits.h>
#include <cmath>
#include <tuple>

using namespace std;

#include "../include/Index.h"

// ----------------------------------------------------------------
Index::Index(TreeFile* t, DataFile* data) {
    // Pre: um ponteiro para uma classe TreeFile inicializada (objeto).
    // Pos: armazena numa variavel interna um ponteiro para o objeto
    // dado, para uso interno da classe.
    treeFile = t;
    dataFile = data;
}

// ----------------------------------------------------------------
Index::~Index() {
    // clear
}

// ----------------------------------------------------------------
Index::mSearchResult Index::mSearch(int x) { // versao publica
    // Pre: classe inicializada.
    // Pos: tenta localizar um valor no index e retorna sua localizacao
    // caso o encontre no formato: (pos, i, found).
    return mSearch(treeFile, x);
}

// ----------------------------------------------------------------
tuple<int, int> Index::insertB(DataFile::registry* newRegistry) { // versao publica
    // Pre: classe inicializada.
    // Pos: tenta inserir um valor x no index
    return insertB(newRegistry, treeFile, dataFile);
}

// ----------------------------------------------------------------
int Index::linearSearch(vector<int> &K, int x, int n) {
    // Pre: vetor de valores de um no, o valor a ser procurado e o 
    // numero de valores num vetor.
    // Pos: retorna a posicao do valor a ser procurado no vetor dado
    // ou o proprio n caso nao o encontre.
    int i = 0;
    while (i <= n && K[i] < x) {
        i++;
    } 
    return i;
}

// ----------------------------------------------------------------
void Index::shiftLeft(TreeFile::node &node, int start) {
    // Pre: um no e um index de start.
    // Pos: retorna o no com os valores deslocados uma unidade para a esquerda.
    for (int i = start; i <= node.n; i++) {
        node.K[i] = node.K[i + 1];
        node.A[i] = node.A[i + 1];
        node.B[i] = node.B[i + 1];
    }
}

// ----------------------------------------------------------------
void Index::replaceWithSuccessor(TreeFile::node &nodeP, int &indexP, int &posP, int &read, int &write, mSearchResult &searchResult) {
    // Pre: recebe um no nao-folha, o index do valor no no, posição no arquivo
    // e numero de escritas e leituras.
    // Pos: retorna o nó folha onde o sucessor estava e atualiza todos os valores.
    // desce ate filho direito e acha no mais a esquerda
    int posQ = nodeP.A[indexP];
    TreeFile::node nodeQ = treeFile->getNthNode(posQ); 
    read++;

    vector<tuple<TreeFile::node,int>> pathNodes;

    // acrescenta nó na pilha de nós visitados
    pathNodes.push_back(make_tuple(nodeQ, posQ));

    while (nodeQ.A[0] != 0) {
        posQ = nodeQ.A[0];
        nodeQ = treeFile->getNthNode(posQ);
        read++;
        pathNodes.push_back(make_tuple(nodeQ, posQ));
    }

    // substitui Ki e Bi por valores do sucessor e escreve no disco
    nodeP.K[indexP] = nodeQ.K[1];
    nodeP.B[indexP] = nodeQ.B[1];
    treeFile->writeNode(nodeP, posP);
    write++;

    searchResult.visitedNodes.push(make_tuple(nodeP, posP));
    searchResult.i.push(indexP);
    for (auto &p : pathNodes) {
        searchResult.visitedNodes.push(p);
        searchResult.i.push(1);
    }
    searchResult.visitedNodes.pop();
    searchResult.i.pop(); 
    
    nodeP = nodeQ;
    posP = posQ;
    indexP = 1;
}

// ----------------------------------------------------------------
void Index::redistributeRight(TreeFile::node &nodeP, TreeFile::node &nodeR, TreeFile::node &nodeQ, int j, int posP, int posR, int posQ, int &write) {
    // Pre: no P tem irmao direito no Q e no Q pode emprestar chave.
    // Pos: redistribui as chaves, movendo chave do pai para P, e a menor chave de Q para pai.

    // Move a chave do pai para P
    nodeP.n++;
    nodeP.K[nodeP.n] = nodeR.K[j];
    nodeP.A[nodeP.n] = nodeQ.A[0];
    nodeP.B[nodeP.n] = nodeR.B[j];

    // Move a menor chave de Q para pai
    nodeR.K[j] = nodeQ.K[1];
    nodeR.B[j] = nodeQ.B[1];

    // Ajusta no Q
    nodeQ.n--;
    nodeQ.A[0] = nodeQ.A[1];
    shiftLeft(nodeQ, 1);

    treeFile->writeNode(nodeP, posP);
    treeFile->writeNode(nodeQ, posQ);
    treeFile->writeNode(nodeR, posR);
    write += 3;
}

// ----------------------------------------------------------------
void Index::redistributeLeft(TreeFile::node &nodeP, TreeFile::node &nodeR, TreeFile::node &nodeQ, int j, int posP, int posR, int posQ, int &write) {
    // Pre: no P tem irmao esquerdo no Q e no Q pode emprestar chave.
    // Pos: redistribui as chaves, movendo chave do pai para P, e a maior chave de Q para pai.

    // Desloca P para direita para abrir espaço no início
    for (int i = nodeP.n; i >= 1; i--) {
        nodeP.K[i + 1] = nodeP.K[i];
        nodeP.A[i + 1] = nodeP.A[i];
        nodeP.B[i + 1] = nodeP.B[i];
    }
    nodeP.A[1] = nodeP.A[0];

    // Adiciona chave do pai a no P
    nodeP.n++;
    nodeP.K[1] = nodeR.K[j];
    nodeP.A[0] = nodeQ.A[nodeQ.n];
    nodeP.B[1] = nodeR.B[j];

    // Move maior chave de Q para pai
    nodeR.K[j] = nodeQ.K[nodeQ.n];
    nodeR.B[j] = nodeQ.B[nodeQ.n];

    // Ajusta no Q
    nodeQ.n--;

    treeFile->writeNode(nodeP, posP);
    treeFile->writeNode(nodeQ, posQ);
    treeFile->writeNode(nodeR, posR);
    write += 3;
}

// ----------------------------------------------------------------
Index::mSearchResult Index::mSearch(TreeFile* treeFile, int x) { // versao privada
    // Pre: classe inicializada.
    // Pos: tenta localizar um valor no index e retorna sua localizacao
    // caso o encontre no formato: (pos, i, found).
    TreeFile::node node;
    mSearchResult result;
    int p = treeFile->getIndexRoot(); 
    int q = 0;
    int b = 0;
    int i;
    result.found = false; 

    while (p != 0) {
        // Ler no e colocar na pilha
        node = treeFile->getNthNode(p);
        result.visitedNodes.push(make_tuple(node, p));
        result.read++;

        // Ajuste do array para incluir MIN_VALUE e MAX_VALUE
        vector<int> K(node.n + 2); 
        K[0] = INT_MIN;
        for (int j = 1; j <= node.n; j++) K[j] = node.K[j];
        K[node.n + 1] = INT_MAX;

        // Busca do indice Ki
        i = linearSearch(K, x, node.n);

        if (x == K[i]) {
            result.found = true;
            result.i.push(i);
            return result;
        } else {            
            i--;
            q = p;
            p = node.A[i]; // Desce para subarvore correta
            result.i.push(i);
        }

    }

    return result;
}

// ----------------------------------------------------------------
tuple<int, int> Index::insertB(DataFile::registry* newRegistry, TreeFile* treeFile, DataFile* dataFile) { // versao privada
    // Pre: classe inicializada.
    // Pos: tenta localizar e inserir um valor no index.

    // (K, A) e o par a ser inserido
    int K = newRegistry->mass;
    int A = 0;
    int B;
    TreeFile::node nodeP, nodeQ;
    int pos, index;
    tuple<int, int> accessNumber;
    int write = 0;

    mSearchResult searchResult = mSearch(treeFile, newRegistry->mass);    

    // x ja esta em T
    if (searchResult.found) {
        cout << "Error: value already exists." << endl;
        accessNumber = make_tuple(searchResult.read, write);
        return accessNumber;
    }

    B = dataFile->writeRegistry(*newRegistry);

    while (!searchResult.visitedNodes.empty()) {
        // Usa ultimo no empilhado
        nodeP = get<0>(searchResult.visitedNodes.top());
        pos = get<1>(searchResult.visitedNodes.top());
        index = searchResult.i.top();

        searchResult.visitedNodes.pop();
        searchResult.i.pop();

        // Se elemento pode ser inserido no no
        if (nodeP.n < treeFile->getM() - 1) {  
            // Arruma as chaves para o local correto
            for (int i = nodeP.n; i >= index + 1; i--) {
                nodeP.K[i + 1] = nodeP.K[i];
                nodeP.A[i + 1] = nodeP.A[i];
                nodeP.B[i + 1] = nodeP.B[i];
            }

            // Insere chave nova
            nodeP.K[index + 1] = K;
            nodeP.A[index + 1] = A;
            nodeP.B[index + 1] = B;

            nodeP.n++;

            treeFile->writeNode(nodeP, pos);
            write++;
            accessNumber = make_tuple(searchResult.read, write);
            return accessNumber;
        } 

        // Definindo no P e Q com vetores temporarios
        int tempK[treeFile->getM() + 1];
        int tempA[treeFile->getM() + 1];
        int tempB[treeFile->getM() + 1];
        tempA[0] = nodeP.A[0];

        // Preenche parte da esquerda
        for (int i = 1; i <= index; i++) {
            tempK[i] = nodeP.K[i];
            tempA[i] = nodeP.A[i];
            tempB[i] = nodeP.B[i];
        }

        // Insere chave
        tempK[index + 1] = K;
        tempA[index + 1] = A;
        tempB[index + 1] = B;

        // Preenche parte da direita
        for (int i = index + 1; i <= nodeP.n; i++) {
            tempK[i + 1] = nodeP.K[i];
            tempA[i + 1] = nodeP.A[i];
            tempB[i + 1] = nodeP.B[i];
        }
        
        int half = (int)ceil(treeFile->getM()/2.0);

        // Cria no Q e P
        nodeQ.n = treeFile->getM() - half;
        nodeQ.A[0] = tempA[half];
        for (int i = 1; i <= nodeQ.n; i++) {
            nodeQ.K[i] = tempK[half + i];
            nodeQ.A[i] = tempA[half + i];
            nodeQ.B[i] = tempB[half + i];
        } 

        nodeP.n = half - 1;
        nodeP.A[0] = tempA[0];
        for (int i = 1; i <= nodeP.n; i++) {
            nodeP.K[i] = tempK[i];
            nodeP.A[i] = tempA[i];
            nodeP.B[i] = tempB[i];
        }

        // Escrever p e q para disco
        K = tempK[half];
        B = tempB[half];
        treeFile->writeNode(nodeP, pos);
        A = treeFile->writeNode(nodeQ);
        
        write += 2;
    }

    // Escreve uma nova raiz      
    TreeFile::node nodeRoot;
    nodeRoot.n = 1;
    nodeRoot.A[0] = treeFile->getIndexRoot();
    nodeRoot.K[1] = K;
    nodeRoot.A[1] = A;
    nodeRoot.B[1] = B;

    // Atualizar raiz e escrever raiz no disco
    int posRoot = treeFile->writeNode(nodeRoot);
    treeFile->setIndexRoot(posRoot);
    write++;
    accessNumber = make_tuple(searchResult.read, write);
    return accessNumber;
}

// ----------------------------------------------------------------
tuple<int, int> Index::deleteB(int x) { // versao privada
    // Pre: classe inicializada.
    // Pos: tenta localizar e deletar um valor no index, retornando o numero de leituras e escritas

    TreeFile::node nodeP, nodeQ, nodeR, nodeS;
    // nodeP: nó atual contendo x (ou onde estava x)
    // nodeQ: nó auxiliar, filho direito/esquerdo de P
    // nodeR: nó auxiliar, pai de P
    tuple<int, int> accessNumber;
    int posP, indexP, posQ, posR, indexR;
    int write = 0, read = 0, j;

    mSearchResult searchResult = mSearch(treeFile, x);
    read = searchResult.read;

    // x nao esta em T
    if (!searchResult.found) {
        cout << "Error: value doesnt exist." << endl;
        accessNumber = make_tuple(read, write);
        return accessNumber;
    }    

    nodeP = get<0>(searchResult.visitedNodes.top());
    posP = get<1>(searchResult.visitedNodes.top());
    indexP = searchResult.i.top();
    searchResult.visitedNodes.pop();
    searchResult.i.pop();

    // registro removido do datafile
    dataFile->removeRegistry(nodeP.B[indexP]); 
    
    // nó é não-folha? 
    if (nodeP.A[0] != 0) {
        replaceWithSuccessor(nodeP, indexP, posP, read, write, searchResult);
    }
        
    // Remover (Ki, Ai, Bi) de p
    nodeP.n--;
    shiftLeft(nodeP, indexP);

    cout << "NO P (COM CHAVE REMOVIDA):" << endl;
    cout << "posP: " << posP << " indexP: " << indexP << endl;
    cout << "nP: " << nodeP.n << endl;
    cout << nodeP.A[0] << " ";
    for (int i = 1; i <= nodeP.n; i++) {
        cout << nodeP.K[i] << " " << nodeP.A[i] << " ";
    }
    cout << endl;

    // fragmentar nó (nao possui chaves suficientes e nao é raiz)
    while (nodeP.n < (int)ceil(treeFile->getM()/2.0) - 1 && posP != treeFile->getIndexRoot() && !searchResult.visitedNodes.empty()) {
        // Pegar nó pai de p = nó R
        nodeR = get<0>(searchResult.visitedNodes.top());
        posR = get<1>(searchResult.visitedNodes.top());
        indexR = searchResult.i.top();
        searchResult.visitedNodes.pop();
        searchResult.i.pop();
        
        cout << "NO R (PAI DO NO P):" << endl;
        cout << "posR: " << posR << " indexR: " << indexR << endl;
        cout << "nR: " << nodeR.n << endl;
        cout << nodeR.A[0] << " ";
        for (int i = 1; i <= nodeR.n; i++) {
            cout << nodeR.K[i] << " " << nodeR.A[i] << " ";
        }
        cout << endl;

        // p tem irmao direito q mais proximo = nó Q
        if (indexR + 1 <= nodeR.n) {
            j = indexR + 1;
            posQ = nodeR.A[j];
            nodeQ = treeFile->getNthNode(posQ);
            read++; 

            cout << "NO Q (IRMAO DIREITO DO NO P):" << endl;
            cout << "posQ: " << posQ << endl;
            cout << "nQ: " << nodeQ.n << endl;
            cout << nodeQ.A[0] << " ";
            for (int i = 1; i <= nodeQ.n; i++) {
                cout << nodeQ.K[i] << " " << nodeQ.A[i] << " ";
            }
            cout << endl;

            // nó Q pode emprestar, redistribuir chaves
            if (nodeQ.n >= (int)ceil(treeFile->getM()/2.0)) { 
                redistributeRight(nodeP, nodeR, nodeQ, j, posP, posR, posQ, write);
                cout << "NO P (EMPRESTIMO DO NO PAI E IRMAO DIREITO):" << endl;
                cout << "posP: " << posP << " indexP: " << indexP << endl;
                cout << "nP: " << nodeP.n << endl;
                cout << nodeP.A[0] << " ";
                for (int i = 1; i <= nodeP.n; i++) {
                    cout << nodeP.K[i] << " " << nodeP.A[i] << " ";
                }
                cout << endl;

                cout << "NO R (EMPRESTIMO DO NO PAI E IRMAO DIREITO):" << endl;
                cout << "posR: " << posR << " indexR: " << indexR << endl;
                cout << "nR: " << nodeR.n << endl;
                cout << nodeR.A[0] << " ";
                for (int i = 1; i <= nodeR.n; i++) {
                    cout << nodeR.K[i] << " " << nodeR.A[i] << " ";
                }
                cout << endl;

                cout << "NO Q (EMPRESTIMO DO NO PAI E IRMAO DIREITO):" << endl;
                cout << "posQ: " << posQ << endl;
                cout << "nQ: " << nodeQ.n << endl;
                cout << nodeQ.A[0] << " ";
                for (int i = 1; i <= nodeQ.n; i++) {
                    cout << nodeQ.K[i] << " " << nodeQ.A[i] << " ";
                }
                cout << endl;

                accessNumber = make_tuple(searchResult.read, write);
                return accessNumber;
            }
            // combinar p, Krj e q
            nodeS = nodeP;
            nodeS.n = nodeP.n + nodeQ.n + 1;

            nodeS.K[nodeP.n + 1] = nodeR.K[j];
            nodeS.A[nodeP.n + 1] = nodeQ.A[0];
            nodeS.B[nodeP.n + 1] = nodeR.B[j];

            for (int i = nodeP.n + 2, k = 1; k <= nodeQ.n; i++, k++) {
                nodeS.K[i] = nodeQ.K[k];
                nodeS.A[i] = nodeQ.A[k];
                nodeS.B[i] = nodeQ.B[k];
            }

            cout << "NO S (COMBINACAO):" << endl;
            cout << "nS: " << nodeS.n << endl;
            cout << nodeS.A[0] << " ";
            for (int i = 1; i <= nodeS.n; i++) {
                cout << nodeS.K[i] << " " << nodeS.A[i] << " ";
            }
            cout << endl;

            treeFile->writeNode(nodeS, posP);
            treeFile->removeNode(posQ);
            write += 2;

            // retirar Krj de nó R
            nodeR.n--;
            shiftLeft(nodeR, j);

            nodeP = nodeR;
            posP = posR;
            indexP = indexR;
        } 
        else { // nó P tem irmão mais próximo esquerdo = nó R (indexR - 1 >= 0)
            if (indexR - 1 < 0) break;
            j = indexR;
            posQ = nodeR.A[indexR - 1];
            nodeQ = treeFile->getNthNode(posQ);
            read++;

            cout << "NO Q  (IRMAO ESQUERDO DO NO P):" << endl;
            cout << "posQ: " << posQ << endl;
            cout << "nQ: " << nodeQ.n << endl;
            cout << nodeQ.A[0] << " ";
            for (int i = 1; i <= nodeQ.n; i++) {
                cout << nodeQ.K[i] << " " << nodeQ.A[i] << " ";
            }
            cout << endl; 

            // nó Q pode emprestar, redistribuir chaves

            if (nodeQ.n >= (int)ceil(treeFile->getM()/2.0)) { 
                redistributeLeft(nodeP, nodeR, nodeQ, j, posP, posR, posQ, write);
                
                cout << "NO P (EMPRESTIMO DO NO PAI E IRMAO ESQUERDO):" << endl;
                cout << "posP: " << posP << " indexP: " << indexP << endl;
                cout << "nP: " << nodeP.n << endl;
                cout << nodeP.A[0] << " ";
                for (int i = 1; i <= nodeP.n; i++) {
                    cout << nodeP.K[i] << " " << nodeP.A[i] << " ";
                }
                cout << endl;

                cout << "NO R (EMPRESTIMO DO NO PAI E IRMAO ESQUERDO):" << endl;
                cout << "posR: " << posR << " indexR: " << indexR << endl;
                cout << "nR: " << nodeR.n << endl;
                cout << nodeR.A[0] << " ";
                for (int i = 1; i <= nodeR.n; i++) {
                    cout << nodeR.K[i] << " " << nodeR.A[i] << " ";
                }
                cout << endl;

                cout << "NO Q (EMPRESTIMO DO NO PAI E IRMAO ESQUERDO):" << endl;
                cout << "posQ: " << posQ << endl;
                cout << "nQ: " << nodeQ.n << endl;
                cout << nodeQ.A[0] << " ";
                for (int i = 1; i <= nodeQ.n; i++) {
                    cout << nodeQ.K[i] << " " << nodeQ.A[i] << " ";
                }
                cout << endl;
                accessNumber = make_tuple(searchResult.read, write);
                return accessNumber;
            }
            // combinar p, Krj e y
            nodeS = nodeQ;
            nodeS.n = nodeP.n + nodeQ.n + 1;

            nodeS.K[nodeQ.n + 1] = nodeR.K[j];
            nodeS.A[nodeQ.n + 1] = nodeP.A[0];
            nodeS.B[nodeQ.n + 1] = nodeR.B[j];

            for (int i = nodeQ.n + 2, k = 1; k <= nodeP.n; i++, k++) {
                nodeS.K[i] = nodeP.K[k];
                nodeS.A[i] = nodeP.A[k];
                nodeS.B[i] = nodeP.B[k];
            }

            cout << "EMPRESTIMO - NO NOVO!" << endl;
            cout << "N: " << nodeS.n << endl;
            cout << nodeS.A[0] << " ";
            for (int i = 1; i <= nodeS.n; i++) {
                cout << nodeS.K[i] << " " << nodeS.A[i] << " ";
            }
            cout << endl;

            treeFile->writeNode(nodeS, posQ);
            treeFile->removeNode(posP);
            write += 2;

            // retirar Krj de nó R
            nodeR.n--;
            shiftLeft(nodeR, j);

            nodeP = nodeR;
            posP = posR;
            indexP = indexR;
        }
    } 

    // escrita para disco
    if (nodeP.n != 0) {
        treeFile->writeNode(nodeP, posP);
        write++;
    } else {
        if (nodeP.A[0] != 0) treeFile->setIndexRoot(nodeP.A[0]);
        else treeFile->setIndexRoot(0);
        treeFile->removeNode(posP);
        write++;
    }

    accessNumber = make_tuple(read, write);
    return accessNumber;
}

// ----------------------------------------------------------------
void Index::bulkInsert() {
    // Pre: existencia de um arquivo txt com os planetas a serem
    // inseridos na seguinte formatacao:
    /*
        <massa> <nome> <raio> <distancia do sol> <nro de satelites> <e satelite?>
        ...
        <massa> <nome> <raio> <distancia do sol> <nro de satelites> <e satelite?>
    */
    // Pos: insere os corpos celestes contidos no arquivo txt nos
    // arquivos binarios ultilizados pelo programa.
    string fileName;
    cout << "Entre com o nome do arquivo de texto a ser lido (default: planets.txt): ";
    cin >> fileName;

    // processo de abertura do arquivo de texto com o nome fornecido 
    // pelo usuario em modo de leitura:
    ifstream txtFile;
    txtFile.open(string("../" + fileName), ios::in);
    if (!txtFile.is_open()){
        txtFile.open(string("../" + fileName + ".txt"), ios::in);
        if (!txtFile.is_open()){
            txtFile.open("../planets.txt", ios::in);
            if (!txtFile.is_open()){
                cout << "Erro: arquivo de texto nao pode ser aberto (provavel erro de armazenamento)." << endl;
                abort();
            } else {
                cout << "Nome fornecido invalido ou arquivo não encontrado, lendo planets.txt..." << endl;
            }
        } else {
            cout << "Lendo " << fileName << ".txt..." << endl;
        }
    } else {
        cout << "Lendo " << fileName << "..." << endl;
    }

    // escrita no arquivo binario
    DataFile::registry planet;
    string isMoon;
    while(! txtFile.eof()) {
        txtFile >> planet.mass >> planet.name >> planet.radius >> planet.distanceFromSun >> planet.satellites;
        txtFile >> isMoon;
        if(isMoon == "t" || isMoon == "T" || isMoon == "true" || isMoon == "True") 
            planet.isMoon = true;
        else
            planet.isMoon = false;
        insertB(&planet);
    }
    txtFile.close();

    cout << "Planetas adicionados com sucesso!" << endl;
}