// TreeFile.cpp
// Class to abstract the TreeFile manipulation
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

#include "../include/TreeFile.h"

TreeFile::TreeFile(){
    tree.open("../files/TreeFile.bin", ios::in | ios::out | ios::binary);

    if(! tree) {

        // Criacao do arquivo de acordo com mvias.txt, caso o arquivo binario nao exista
        cout << "Arquivo TreeFile.bin nao existe ainda, criando..." << endl;
        node p;
        int r = 1; // Indice onde se localiza a raiz da arvore de m-vias

        ofstream arvore ("../files/TreeFile.bin",ios::out | ios::binary);
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

        p.n = 5;
        p.A[0] = 1; // Definicao da raiz
        arvore.seekp(0);
        arvore.write((const char *)(&p),sizeof(node));
        arvore.close();
        txtFile.close();

        ifstream arvoreIn ("../files/TreeFile.bin", ios::in | ios::binary);
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
    node p;
    tree.seekg(0);
    tree.read((char *)(&p), sizeof(node));
    size = p.n;
    root = p.A[0];
    tree.seekg(1 * sizeof(node));
}

TreeFile::~TreeFile(){
    writeMetaInfo();
    tree.close();
}

TreeFile::node TreeFile::getTreeRoot() {
    node p;
    tree.seekg(root * sizeof(node));
    tree.read((char *)(&p), sizeof(node));
    return p;
}

TreeFile::node TreeFile::getNextNode() {
    node p;
    tree.read((char *)(&p), sizeof(node));

    if(! tree.eof()) { // tratamento de erro no qual tenta-se ler um no depois do final do arquivo
        return p;
    } else {
        tree.seekg(0);
        tree.read((char *)(&p), sizeof(node));
        return p;
    }
}

TreeFile::node TreeFile::getNthNode(int n) {
    tree.seekg(n * sizeof(node)) ;
    node p;
    tree.read((char *)(&p), sizeof(node));

    if(! tree.eof()) { // tratamento de erro no qual tenta-se ler um no depois do final do arquivo
        return p;
    } else {
        cout << "Erro: tentativa de acessar no inexistente."<< endl;
        abort();
    }
}

void TreeFile::writeNode(node newNode) {
    tree.seekp(++size * sizeof(node));
    tree.write((const char *)(&newNode), sizeof(node));
}

void TreeFile::writeMetaInfo() {
    node p;
    p.n = size;
    p.A[0] = root;
    tree.seekp(0);
    tree.write((const char *)(&p),sizeof(node));
}
