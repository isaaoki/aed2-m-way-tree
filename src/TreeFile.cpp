// TreeFile.cpp
// Class to abstract the TreeFile manipulation
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
using namespace std;

#include "../include/TreeFile.h"

// ----------------------------------------------------------------
TreeFile::TreeFile(){
    // Pre: nenhuma.
    // Pos: tenta abrir o arquivo binario, caso nao consiga, cria-o
    // com as meta-informacoes.
    tree.open("../files/mvias.bin", ios::in | ios::out | ios::binary);

    if(!tree.is_open()) {
        createTree();
    } else {
        node p;
        tree.seekg(0);
        tree.read((char *)(&p), sizeof(node));
        size = p.n;
        root = p.A[0];
        tree.seekg(1 * sizeof(node));
    }

}

// ----------------------------------------------------------------
TreeFile::~TreeFile(){
    // Pre: nenhuma.
    // Pos: fecha o arquivo binario.
    writeMetaInfo();
    tree.close();
}

// ----------------------------------------------------------------
TreeFile::node TreeFile::getTreeRoot() {
    // Pre: arquivo binario tree aberto.
    // Pos: retorna o no raiz da arvore.
    node p;
    tree.seekg(root * sizeof(node));
    tree.read((char *)(&p), sizeof(node));
    return p;
}

// ----------------------------------------------------------------
TreeFile::node TreeFile::getNextNode() {
    // Pre: arquivo binario tree aberto.
    // Pos: retorna o no seguinte ao lido anteriormente (util ao se
    // ler o arquivo sequencialmente).
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

// ----------------------------------------------------------------
TreeFile::node TreeFile::getNthNode(int n) {
    // Pre: arquivo binario tree aberto e um inteiro correspondente
    // ao index do no que se deseja obter.
    // Pos: retorna o no correspondente ao index dado caso seja um
    // index valido. Caso contrario, escreve uma mensagem de erro
    // e aborta o programa.
    tree.seekg(n * sizeof(node));
    node p;
    tree.read((char *)(&p), sizeof(node));

    if(! tree.eof()) { // tratamento de erro no qual tenta-se ler um no depois do final do arquivo
        return p;
    } else {
        cout << "Error: inexistent node being accessed."<< endl;
        abort();
    }
}

// ----------------------------------------------------------------
void TreeFile::writeNode(node newNode) { 
    // Pre: arquivo binario tree aberto e uma estrutura de no a ser
    // escrita em memoria secundaria.
    // Pos: Escreve a estrutura de registro em memoria secundaria.
    tree.seekp(++size * sizeof(node));
    tree.write((const char *)(&newNode), sizeof(node));
}

// ----------------------------------------------------------------
void TreeFile::writeNode(node newNode, int pos) {
    // Pre: arquivo binario tree aberto, uma estrutura de no a ser
    // escrita em memoria secundaria e o index da estrutura.
    // Pos: Escreve a estrutura de registro em memoria secundaria na
    // posicao indicada.
    tree.seekp(pos * sizeof(node));
    tree.write((const char*)(&newNode), sizeof(node));

    if (pos > size) size = pos;
}

// ----------------------------------------------------------------
void TreeFile::printNode(node node) {
    // Pre: uma estrutura de no a ser impressa. Nao precisa do 
    // arquivo aberto e e um metodo estatico.
    // Pos: imprime na tela o no dado com uma formatacao amigavel.
    cout << node.n << ","
         << right << setw(3) << node.A[0] << ",";
    for (int j = 1; j <= node.n; j++) {
        cout << "(" 
             << setw(3) << node.K[j] << ", "
             << setw(3) << node.A[j] << ","
             << setw(3) << node.B[j] << ")";
    }
    cout << endl;
}

// ----------------------------------------------------------------
void TreeFile::printTree() {
    // Pre: arquivo binario tree aberto.
    // Pos: imprime na tela a arvore completa, inclusive sua raiz e
    // o valor de m com uma formatacao amigavel.
    cout << "B-TREE" << endl;
    cout << "T (root) = " << root << ", m = " << m << endl;
    cout << "Number of nodes = " << size << endl;
    cout << string(110, '-') << endl;
    cout << left << setw(6) << "No" 
         << "n,A[0],(K[1],A[1],B[1]),...,(K[n],A[n], B[n])" << endl;
    cout << string(110, '-') << endl;

    tree.seekg(1 * sizeof(node));
    for (int i = 1; i <= size; i++) {
        cout << left << setw(6) << i;
        printNode(getNextNode());
    }
    tree.seekg(1 * sizeof(node));

    cout << string(110, '-') << endl;
}

// ----------------------------------------------------------------
int TreeFile::getM() {
    // Pre: classe inicializada.
    // Pos: retorna o valor correspondente ao m da arvore.
    return m;
}

// ----------------------------------------------------------------
void TreeFile::createTree() {
    // Pre: nenhuma.
    // Pos: cria um arquivo de arvore de m-vias vazio.
    ofstream treeCreation("../files/mvias.bin", ios::out | ios::binary);
    if(!treeCreation.is_open()){
        cerr << "Error: mvias.bin file cannot be open (possible memory error)." << endl;
        abort();
    }
    node p;
    p.n = 0;
    p.A[0] = 1;
    treeCreation.seekp(0);
    treeCreation.write((const char *)(&p),sizeof(node));
    p.A[0] = 0;
    p.K[0] = 0;
    treeCreation.write((const char *)(&p),sizeof(node));
    treeCreation.close();

    size = 0;
    root = 1;
    tree.open("../files/mvias.bin", ios::in | ios::out | ios::binary);

    cout << "Binary tree file created with success! Continuing the execution..." << endl << endl;
}

// ----------------------------------------------------------------
int TreeFile::getSize() {
    // Pre: classe inicializada.
    // Pos: retorna o valor correspondente ao tamanho em nos
    // da arvore de m-vias.
    return size;
}

// ----------------------------------------------------------------
int TreeFile::getIndexRoot() {
    // Pre: classe inicializada.
    // Pos: retorna o index da raiz da arvore.
    return root;
}

// ----------------------------------------------------------------
void TreeFile::setIndexRoot(int pos) {
    // Pre: classe inicializada.
    // Pos: muda a variavel interna da posicao da raiz.
    root = pos;
}

// ----------------------------------------------------------------
void TreeFile::writeMetaInfo() {
    // Pre: classe inicializada.
    // Pos: salva as meta informacoes da arvore em memoria secundaria
    // (tamanho e posicao da raiz).
    node p;
    p.n = size;
    p.A[0] = root;
    tree.seekp(0);
    tree.write((const char *)(&p),sizeof(node));
}