#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <limits.h>
using namespace std;

#include "../include/TreeFile.h"

void printNode(TreeFile::node node) {
    cout << node.n << ","
         << right << setw(3) << node.A[0] << ",";
    for (int j = 1; j <= node.n; j++) {
        cout << "(" 
             << setw(3) << node.K[j] << ", "
             << setw(3) << node.A[j] << ")";
    }
    cout << endl;
}

void printTree(TreeFile &t) {
    int indexRoot = t.getIndexRoot();
    int m = 3; // TODO: método na TreeFile para pegar m

    cout << "T = " << indexRoot << ", m = " << m << endl;
    cout << string(66, '-') << endl;
    cout << left << setw(6) << "No" 
         << "n,A[0],(K[1],A[1]),...,(K[n],A[n])" << endl;
    cout << string(66, '-') << endl;
    
    int size = t.getSize();
    TreeFile::node node;

    for (int i = 1; i <= size; i++) {
        cout << left << setw(6) << i;
        node = t.getNthNode(i);
        printNode(node);
    }

    cout << "------------------------------------------------------------------" << endl;
}

void mSearch(TreeFile &t, int x, int &nodeNumber, int &i, bool &success) {
    TreeFile::node node;

    int p = t.getIndexRoot(); 
    int q;
    int index, childIndex;
    success = false; 

    // Mudar isso para indice talves (p != 0)
    while (p != 0) {
        // Ler nó
        node = t.getNthNode(p);

        // Ajuste do array para incluir MIN_VALUE e MAX_VALUE
        vector<int> K(node.n + 2); 
        K[0] = INT_MIN;
        for (int j = 1; j <= node.n; j++) K[j] = node.K[j];
        K[node.n + 1] = INT_MAX;

        // Busca do indice Ki
        index = 0;
        while (index <= node.n && K[index] < x) {
            index++;
        } 

        if (x == K[index]) {
            nodeNumber = p;
            i = index;
            success = true;
            return;
        } else {

            // TODO: Arrumar a descida para o filho correto
            childIndex = index - 1;
            if (childIndex < 0) childIndex = 0;
            if (childIndex > node.n) childIndex = node.n;
            
            q = p;
            p = node.A[childIndex];
        }

    }

    nodeNumber = q;
    i = childIndex;
    success = false;

}

int main() {
    TreeFile t;
    int i, key, indexNode; 
    bool success = false;
    string isRunning;

    // TODO: Se o arquivo mvias.bin não existir sair do programa, ou tentar abrir de novo...

    do {
        printTree(t);
        cout << "Chave de busca: ";
        cin >> key;

        mSearch(t, key, indexNode, i, success);

        cout << setw(4) << key << " ("
             << indexNode << "," << i << ",";
        cout << (success ? "true" : "false") << ")" << endl;

        cout << "Continuar busca (s/n)? ";
        cin >> isRunning;
    } while (isRunning != "n");

    return 0; // fecha o arquivo e salva-o no disco
}