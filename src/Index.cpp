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
tuple<int, int> Index::deleteB(int x) {
    return deleteB(treeFile, dataFile, x);
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
        A = treeFile->getSize() + 1;

        treeFile->writeNode(nodeP, pos);
        treeFile->writeNode(nodeQ);
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
    treeFile->writeNode(nodeRoot);
    treeFile->setIndexRoot(treeFile->getSize());

    accessNumber = make_tuple(searchResult.read, write);
    return accessNumber;
}

// ----------------------------------------------------------------
tuple<int, int> Index::deleteB(TreeFile* treeFile, DataFile* dataFile, int x) {
    TreeFile::node nodeP, nodeQ, nodeR, nodeS;
    tuple<int, int> accessNumber;
    int pos, index, write = 0;
    int posQ, posR, indexR;

    mSearchResult searchResult = mSearch(treeFile, x);

    // x nao esta em T
    if (!searchResult.found) {
        cout << "Error: value doesnt exist." << endl;
        accessNumber = make_tuple(searchResult.read, write);
        return accessNumber;
    }    

    nodeP = get<0>(searchResult.visitedNodes.top());
    pos = get<1>(searchResult.visitedNodes.top());
    index = searchResult.i.top();
    searchResult.visitedNodes.pop();
    searchResult.i.pop();

    // registro removido do datafile
    dataFile->removeRegistry(nodeP.B[index]); // esboço: n esta funcionando
    
    // nó é não-folha? 
    if (nodeP.A[0] != 0) {
        // encontra chave para mover para folha
        nodeQ = treeFile->getNthNode(nodeP.A[index]); // ir para subarvore direita

        while (nodeQ.A[0] != 0) { // q não é folha
            // Adicionar leituras...
            nodeQ = treeFile->getNthNode(nodeP.A[0]);
        }

        // Substituir Ki por Kqi e escrever no disco
        nodeP.K[index] = nodeQ.K[1];
        nodeP.B[index] = nodeQ.B[1];
        treeFile->writeNode(nodeP, pos);
        write++;
        
        nodeP = nodeQ;
        index = 1;
    }
    
    // remover (Ki, Ai, Bi) de p
    nodeP.n--;
    for (int i = pos; i <= nodeP.n; i++) {
        nodeP.K[i] = nodeP.K[i + 1];
        nodeP.A[i] = nodeP.A[i + 1];
        nodeP.B[i] = nodeP.B[i + 1];
    }

    // fragmentar nó (nao possui chaves suficientes e nao é raiz)
    while (nodeP.n < (int)ceil(treeFile->getM()/2.0) - 1 && pos != treeFile->getIndexRoot()) {
        // Pegar nó pai de p = nó R
        nodeR = get<0>(searchResult.visitedNodes.top());
        posR = get<1>(searchResult.visitedNodes.top());
        indexR = searchResult.i.top();
        searchResult.visitedNodes.pop();
        searchResult.i.pop();

        // p tem irmao direito q mais proximo = nó Q
        if (indexR + 1 <= nodeR.n) {
            // Adicionar leituras...
            posQ = nodeR.A[indexR + 1];
            nodeQ = treeFile->getNthNode(posQ);
            int j = indexR + 1;

            if (nodeQ.n >= (int)ceil(treeFile->getM()/2.0)) { 
                // nó Q pode emprestar, redistribuir chaves
                nodeP.n++;
                nodeP.K[nodeP.n] = nodeR.K[j];
                nodeP.A[nodeP.n] = nodeQ.A[0];
                nodeP.B[nodeP.n] = nodeR.B[j];

                nodeR.K[j] = nodeQ.K[1];
                nodeR.B[j] = nodeQ.B[1];

                nodeQ.n--;
                nodeQ.A[0] = nodeQ.A[1];
                for (int i = 1; i <= nodeQ.n; i++) {
                    nodeQ.K[i] = nodeQ.K[i + 1];
                    nodeQ.A[i] = nodeQ.A[i + 1];
                    nodeQ.B[i] = nodeQ.B[i + 1];
                }

                treeFile->writeNode(nodeP, pos);
                treeFile->writeNode(nodeQ, posQ);
                treeFile->writeNode(nodeR, posR);
                write += 3;

                accessNumber = make_tuple(searchResult.read, write);
                return accessNumber;
            }
            // combinar p, Krj e y
            nodeS.n = 2 * (int)ceil(treeFile->getM()/2.0);
            nodeS.A[0] = nodeP.A[0];

            for (int i = 1; i <= nodeP.n; i++) {
                nodeS.K[i] = nodeP.K[i];
                nodeS.A[i] = nodeP.A[i];
                nodeS.B[i] = nodeP.B[i];
            }

            nodeS.K[nodeP.n + 1] = nodeR.K[j];
            nodeS.A[nodeP.n + 1] = nodeQ.A[0];
            nodeS.B[nodeP.n + 1] = nodeR.B[j];

            for (int i = 1; i <= nodeQ.n; i++) {
                nodeS.K[i] = nodeQ.K[i];
                nodeS.A[i] = nodeQ.A[i];
                nodeS.B[i] = nodeQ.B[i];
            }

            // retirar Krj de nó R
            nodeR.n--;
            for (int i = j; i <= nodeR.n; i++) {
                nodeR.K[i] = nodeR.K[i + 1];
                nodeR.A[i] = nodeR.A[i + 1];
                nodeR.B[i] = nodeR.B[i + 1];
            }

            nodeP = nodeR;
            pos = posR; // conferir isso 
            index = indexR;
        }
        // fazer caso do irmão esquerdo
    } 

   // escrita para disco
    if (nodeP.n != 0) {
        treeFile->writeNode(nodeP, pos);
        write++;
    } else {
        treeFile->setIndexRoot(nodeP.A[0]); // checar isso???
    }

    accessNumber = make_tuple(searchResult.read, write);
    return accessNumber;
}