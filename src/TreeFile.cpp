// TreeFile.cpp
// Class to abstract the TreeFile manipulation
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

#include "../include/TreeFile.h"

TreeFile::TreeFile(){
    tree.open("../files/TreeFile.bin", ios::in | ios::out);

    if(! tree) {

        // Criacao do arquivo de acordo com mvias.txt, caso o arquivo binario nao exista
        cout << "Arquivo TreeFile.bin nao existe ainda, criando..." << endl;
        node p;
        int r = 1; // Indice onde se localiza a raiz da arvore de m-vias

        ofstream arvore ("../files/TreeFile.bin",ios::out);
        if(! arvore){
            cerr << "Arquivo TreeFile.bin nao pode ser aberto." << endl;
            abort();
        }

        ifstream txtFile;
        txtFile.open("../mvias.txt", ios::in);
        if (! txtFile){
            cout << "Arquivo mvias.txt nao pode ser aberto" << endl;
            abort();
        }

        arvore.seekp(r * sizeof(node));
        txtFile >> p.n;
        while(! txtFile.eof()) {
            txtFile >> p.A[0];
            for(int i = 1; i <= p.n; i++) {
                txtFile >> p.K[i] >> p.A[i];
            }
            arvore.write((const char *)(&p),sizeof(node));
            txtFile >> p.n;
        }
        arvore.close();
        txtFile.close();

        ifstream arvoreIn ("../files/TreeFile.bin",ios::in);
        if(! arvoreIn){
            cerr << "Arquivo TreeFile.bin nao pode ser aberto." << endl;
            abort();
        }

        arvoreIn.seekg(r * sizeof(node));
        arvoreIn.read((char *)(&p),sizeof(node));

        while(! arvoreIn.eof()) {
            cout << p.n << " " << p.A[0] << " ";
            for(int i = 1; i <= p.n; i++) {
                cout << p.K[i] << " " << p.A[i] << " ";
            }
            cout << endl;
            arvoreIn.read((char *)(&p),sizeof(node));
        }
        abort();
        // fim da criacao do arquivo de acordo com mvias.txt, caso o arquivo binario nao exista

    }
}

TreeFile::~TreeFile(){
    tree.close();
}

TreeFile::node TreeFile::getTreeRoot() {
    node p;
    tree.seekg(1 * sizeof(node));
    tree.read((char *)(&p), sizeof(node));
    return p;
}

TreeFile::node TreeFile::getNextNode() {
    node p;
    tree.read((char *)(&p), sizeof(node));

    if(! tree.eof()) {
        return p;
    } else {
        tree.seekg(0);
        tree.read((char *)(&p), sizeof(node));
        return p;
    }
}

TreeFile::node TreeFile::getNthNode(int n) {
    tree.seekg(n * sizeof(node));
    node p;
    tree.read((char *)(&p), sizeof(node));

    if(! tree.eof()) {
        return p;
    } else {
        tree.seekg(0);
        tree.read((char *)(&p), sizeof(node));
        return p;
    }
}
