#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;
#include "TreeFile.cpp"

int main() {
    TreeFile t;
    TreeFile::node node;

    //t.writeNode(t.getNthNode(1));
    // teste da funcao getNthNode
    for(int j = 1; j < 7; j++) {
        node = t.getNthNode(j);
        cout << node.n << " " << node.A[0] << " ";
        for(int i = 1; i <= node.n && i < 5; i++) {
            cout << node.K[i] << " " << node.A[i] << " ";
        }
        cout << endl;
    }

    t.~TreeFile(); // fecha o arquivo e salva-o no disco
}

