// Index.cpp
// Index usage and manipulation
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <limits.h>
#include <cmath>
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
    result.found = false; 

    while (p != 0) {
        // Ler nó e colocar na pilha
        node = treeFile->getNthNode(p);
        result.visitedNodes.push(node);

        // Ajuste do array para incluir MIN_VALUE e MAX_VALUE
        vector<int> K(node.n + 2); 
        K[0] = INT_MIN;
        for (int j = 1; j <= node.n; j++) K[j] = node.K[j];
        K[node.n + 1] = INT_MAX;

        // Busca do indice Ki
        result.i = linearSearch(K, x, node.n);

        if (x == K[result.i]) {
            result.pos = p;
            result.found = true;
            return result;
        } else {            
            result.i--;
            q = p;
            p = node.A[result.i]; // Desce para subarvore correta
        }

    }

    result.pos = q;
    return result;
}

void Index::insertB(TreeFile* treeFile, int x) {
    int K = x; // (K, A) é o par a ser inserido
    int A = 0;
    TreeFile::node nodeP, nodeQ;

    mSearchResult searchResult = mSearch(treeFile, x);    

    // x já está em T
    if (searchResult.found) return;

    while (!searchResult.visitedNodes.empty()) {
        // Usa ultimo nó empilhado
        nodeP = searchResult.visitedNodes.top();
        searchResult.visitedNodes.pop();
 
        // Se elemento pode ser inserido no nó
        if (nodeP.n <= treeFile->m - 1) { 
            nodeP.n++; 

            for (int i = nodeP.n; i > searchResult.i; i--) {
                nodeP.K[i] = nodeP.K[i - 1];
                nodeP.A[i] = nodeP.A[i - 1];
            }

            nodeP.K[searchResult.i] = K;
            nodeP.A[searchResult.i] = A;

            // Escrever p para disco
            return;
        } 

        // Definindo nó P e Q
        int half = ceil((treeFile->m + 1)/2);
        nodeQ.n = nodeP.n - half;
        for (int i = 1; i <= nodeQ.n; i++) {
            nodeQ.K[i] = nodeP.K[half + i];
            nodeQ.A[i] = nodeP.A[half + i];
        }
        nodeQ.A[0] = nodeP.A[half];
        nodeP.n = half - 1;

        // Escrever p e q para disco

        K = nodeP.K[half];
        // A = nodeQ;
    }

    TreeFile::node nodeRoot;
    nodeRoot.n = 1;
    nodeRoot.A[0] = treeFile->getIndexRoot();
    nodeRoot.K[1] = K;
    nodeRoot.A[1] = A;

    // Atualizar raiz e escrever raíz no disco
}