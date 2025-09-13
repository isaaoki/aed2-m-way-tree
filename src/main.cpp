#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <limits.h>
using namespace std;

#include "TreeFile.cpp"

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
    int m = t.m;

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

int linearSearch(vector<int> &K, int x, int n) {
    int i = 0;
    while (i <= n && K[i] < x) {
        i++;
    } 
    return i;
}

void mSearch(TreeFile &t, int x, int &nodeNumber, int &i, bool &success) {
    TreeFile::node node;
    int p = t.getIndexRoot(); 
    int q = 0;
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
        i = linearSearch(K, x, node.n);

        if (x == K[i]) {
            nodeNumber = p;
            success = true;
            return;
        } else {            
            i--;
            q = p;
            p = node.A[i]; // desce para subarvore correta
        }

    }

    nodeNumber = q;
    success = false;

}

int main() {
    TreeFile t;
    int i, x, nodeNumber; 
    bool success = false;
    string isRunning;

    // TODO: Se o arquivo mvias.bin não existir sair do programa, ou tentar abrir de novo...

    do {
        printTree(t);
        cout << "Chave de busca: ";
        cin >> x;

        mSearch(t, x, nodeNumber, i, success);

        cout << setw(4) << x << " ("
             << nodeNumber << "," << i << ",";
        cout << (success ? "true" : "false") << ")" << endl;

        cout << "Continuar busca (s/n)? ";
        cin >> isRunning;
    } while (isRunning != "n");

    return 0; // fecha o arquivo e salva-o no disco
}