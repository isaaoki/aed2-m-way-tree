// TreeFile.cpp
// Class to abstract the TreeFile manipulation
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

#include "../include/TreeFile.h"

TreeFile::TreeFile(){
    tree.open("../files/TreeFile.bin", ios::in | ios::out | ios::binary);

    if(!tree.is_open()) {
        createTree();
    }
}

TreeFile::~TreeFile(){
    // writeMetaInfo();
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
    tree.seekg(n * sizeof(node));
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

void TreeFile::printNode(node node) {
    cout << node.n << ","
         << right << setw(3) << node.A[0] << ",";
    for (int j = 1; j <= node.n; j++) {
        cout << "(" 
             << setw(3) << node.K[j] << ", "
             << setw(3) << node.A[j] << ")";
    }
    cout << endl;
}

void TreeFile::printTree() {
    cout << "T = " << root << ", m = " << m << endl;
    cout << string(66, '-') << endl;
    cout << left << setw(6) << "No" 
         << "n,A[0],(K[1],A[1]),...,(K[n],A[n])" << endl;
    cout << string(66, '-') << endl;

    tree.seekg(1 * sizeof(node));
    for (int i = 1; i <= size; i++) {
        cout << left << setw(6) << i;
        printNode(getNextNode());
    }
    tree.seekg(1 * sizeof(node));

    cout << "------------------------------------------------------------------" << endl;
}

void TreeFile::writeMetaInfo() {
    node p;
    p.n = size;
    p.A[0] = root;
    tree.seekp(0);
    tree.write((const char *)(&p),sizeof(node));
}

void TreeFile::createTree() {

    string fileName;
    cout << "Arquivo TreeFile.bin nao existe ainda, criando..." << endl;
    cout << "Entre com o nome do arquivo de texto a ser lido (default: mvias.txt): ";
    cin >> fileName;

    ifstream txtFile;
    txtFile.open(string("../" + fileName), ios::in);
    if (!txtFile.is_open()){
        txtFile.open(string("../" + fileName + ".txt"), ios::in);
        if (!txtFile.is_open()){
            txtFile.open("../mvias.txt", ios::in);
            if (!txtFile.is_open()){
                cout << "Erro: arquivo de texto nao pode ser aberto (provavel erro de armazenamento)." << endl;
                abort();
            } else {
                cout << "Nome fornecido invalido ou arquivo não encontrado, lendo mvias.txt..." << endl;
            }
        } else {
            cout << "Lendo " << fileName << ".txt..." << endl;
        }
    } else {
        cout << "Lendo " << fileName << "..." << endl;
    }

    ofstream treeCreation("../files/TreeFile.bin", ios::out | ios::binary);
    if(!treeCreation.is_open()){
        cerr << "Erro: arquivo TreeFile.bin nao pode ser aberto (provavel erro de armazenamento)." << endl;
        abort();
    }

    node p;
    size = 0;
    treeCreation.seekp(1 * sizeof(node));

    txtFile >> root;
    txtFile >> p.n;
    while(! txtFile.eof()) {
        txtFile >> p.A[0];
        for(int i = 1; i <= p.n; i++) {
            txtFile >> p.K[i] >> p.A[i];
        }
        treeCreation.write((const char *)(&p),sizeof(node));
        size++;
        txtFile >> p.n;
    }
    txtFile.close();

    p.n = size;
    p.A[0] = root;
    treeCreation.seekp(0);
    treeCreation.write((const char *)(&p),sizeof(node));
    treeCreation.close();

    tree.open("../files/TreeFile.bin", ios::in | ios::out | ios::binary);

    cout << "Arquivo binario criado com sucesso! Continuando o programa..." << endl << endl;
}

int TreeFile::getSize() {
    return size;
}

int TreeFile::getIndexRoot() {
    return root;
}