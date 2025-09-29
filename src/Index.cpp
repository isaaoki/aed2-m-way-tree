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
Index::Index(TreeFile* t) {
    // Pre: um ponteiro para uma classe TreeFile inicializada (objeto).
    // Pos: armazena numa variavel interna um ponteiro para o objeto
    // dado, para uso interno da classe.
    treeFile = t;
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

void Index::insertB(int x) {
    return insertB(treeFile, x);
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
    int i;
    result.found = false; 

    while (p != 0) {
        // Ler nó e colocar na pilha
        node = treeFile->getNthNode(p);
        result.visitedNodes.push(make_tuple(node, p));

        // Ajuste do array para incluir MIN_VALUE e MAX_VALUE
        vector<int> K(node.n + 2); 
        K[0] = INT_MIN;
        for (int j = 1; j <= node.n; j++) K[j] = node.K[j];
        K[node.n + 1] = INT_MAX;

        // Busca do indice Ki
        i = linearSearch(K, x, node.n);

        if (x == K[i]) {
            result.pos = p;
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

    result.pos = q;
    return result;
}

void Index::insertB(TreeFile* treeFile, int x) {
    // (K, A) é o par a ser inserido
    int K = x; 
    int A = 0;
    TreeFile::node nodeP, nodeQ;
    int indexP, index;

    mSearchResult searchResult = mSearch(treeFile, x);    

    // x já está em T
    if (searchResult.found) {
        cout << "Erro: insercao de um valor existente." << endl;
        return;
    }

    while (!searchResult.visitedNodes.empty()) {
        // Usa ultimo nó empilhado
        nodeP = get<0>(searchResult.visitedNodes.top());
        indexP = get<1>(searchResult.visitedNodes.top());
        searchResult.visitedNodes.pop();
        index = searchResult.i.top();
        searchResult.i.pop();

        // Se elemento pode ser inserido no nó
        if (nodeP.n < treeFile->m - 1) {  
            for (int i = nodeP.n; i >= index + 1; i--) {
                nodeP.K[i + 1] = nodeP.K[i];
                nodeP.A[i + 1] = nodeP.A[i];
            }
            nodeP.K[index + 1] = K;
            nodeP.A[index + 1] = A;

            nodeP.n++;

            treeFile->writeNode(nodeP, indexP);
            cout << "Insercao completa!" << endl;
            return;
        } 

        // Definindo nó P e Q com vetores temporarios
        int tempK[treeFile->m + 1];
        int tempA[treeFile->m + 1];
        tempA[0] = nodeP.A[0];

        // Preenche parte da esquerda
        for (int i = 1; i <= index; i++) {
            tempK[i] = nodeP.K[i];
            tempA[i] = nodeP.A[i];
        }

        // Insere chave
        tempK[index + 1] = K;
        tempA[index + 1] = A;

        // Preenche parte da direita
        for (int i = index + 1; i <= nodeP.n; i++) {
            tempK[i + 1] = nodeP.K[i];
            tempA[i + 1] = nodeP.A[i];
        }

        cout << "A[0]: " << tempA[0] << endl;
        for (int i = 1; i <= treeFile->m; i++) {
            cout << "K[" << i << "]: " << tempK[i] << endl;
            cout << "A[" << i << "]: " << tempA[i] << endl;
        }
        
        int half = (int)ceil((treeFile->m)/2.0);
        cout << "Half: " << half << endl;

        // Cria nó Q e P
        nodeQ.n = treeFile->m - half;
        nodeQ.A[0] = tempA[half];
        for (int i = 1; i <= nodeQ.n; i++) {
            nodeQ.K[i] = tempK[half + i];
            nodeQ.A[i] = tempA[half + i];
            cout << "Node Q - K[" << i << "]: " << nodeQ.K[i] << endl;
            cout << "Node Q - A[" << i << "]: " << nodeQ.A[i] << endl;
        } 

        nodeP.n = half - 1;
        nodeP.A[0] = tempA[0];
        for (int i = 1; i <= nodeP.n; i++) {
            nodeP.K[i] = tempK[i];
            nodeP.A[i] = tempA[i];
            cout << "Node P - K[" << i << "]: " << nodeP.K[i] << endl;
            cout << "Node P - A[" << i << "]: " << nodeP.A[i] << endl;
        }

        // Escrever p e q para disco
        K = tempK[half];
        A = treeFile->getSize() + 1;
        cout << "K: " << K << " A: " << A << endl;

        treeFile->writeNode(nodeP, indexP);
        treeFile->writeNode(nodeQ);
    }

    TreeFile::node nodeRoot;
    nodeRoot.n = 1;
    nodeRoot.A[0] = treeFile->getIndexRoot();
    nodeRoot.K[1] = K;
    nodeRoot.A[1] = A;

    // Atualizar raiz e escrever raíz no disco
    treeFile->writeNode(nodeRoot);
    treeFile->setIndexRoot(treeFile->getSize());

    cout << "Insercao completa!" << endl;
}