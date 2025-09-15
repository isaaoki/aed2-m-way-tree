// Index.cpp
// Index usage and manipulation
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <limits.h>
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
Index::mSearchResut Index::mSearch(int x) { // versao publica
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
Index::mSearchResut Index::mSearch(TreeFile* treeFile, int x) { // versao privada
    // Pre: classe inicializada.
    // Pos: tenta localizar um valor no index e retorna sua localizacao
    // caso o encontre no formato: (pos, i, found).
    TreeFile::node node;
    mSearchResut result;
    int p = treeFile->getIndexRoot(); 
    int q = 0;
    result.found = false; 

    while (p != 0) {
        // Ler nó
        node = treeFile->getNthNode(p);

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