// Index.cpp
// Index usage and manipulation
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <limits.h>
using namespace std;

#include "../include/Index.h"

Index::Index(TreeFile* t) {
    treeFile = t;
}

Index::~Index() {
    // clear
}

Index::mSearchResut Index::mSearch(int x) {
    return mSearch(treeFile, x);
}

int Index::linearSearch(vector<int> &K, int x, int n) {
    int i = 0;
    while (i <= n && K[i] < x) {
        i++;
    } 
    return i;
}

Index::mSearchResut Index::mSearch(TreeFile* treeFile, int x) {
    TreeFile::node node;
    mSearchResut result;
    int p = treeFile->getIndexRoot(); 
    int q = 0;
    result.found = false; 

    // Mudar isso para indice talves (p != 0)
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
            p = node.A[result.i]; // desce para subarvore correta
        }

    }

    result.pos = q;
    result.found = false;
    return result;
}



