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
        cout << "Erro: tentativa de acessar no inexistente."<< endl;
        abort();
    }
}

// ----------------------------------------------------------------
void TreeFile::writeNode(node newNode) { 
    tree.seekp(++size * sizeof(node));
    tree.write((const char *)(&newNode), sizeof(node));
}

void TreeFile::writeNode(node newNode, int pos) {
    tree.seekp(pos * sizeof(node));
    tree.write((const char*)(&newNode), sizeof(node));
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
             << setw(3) << node.A[j] << ")";
    }
    cout << endl;
}

// ----------------------------------------------------------------
void TreeFile::printTree() {
    // Pre: arquivo binario tree aberto.
    // Pos: imprime na tela a arvore completa, inclusive sua raiz e
    // o valor de m com uma formatacao amigavel.
    cout << "ARVORE B" << endl;
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

    cout << string(66, '-') << endl;
}

// ----------------------------------------------------------------
void TreeFile::createTree() {
    // Pre: existencia de um arquivo txt como modelo de criacao nos
    // seguintes moldes: 
    /*
        <raiz>
        <n> <A0> <K1> <A1> ... <Kn> <An>
        ...
        <n> <A0> <K1> <A1> ... <Kn> <An>
    */
    // Pos: traduz o arquivo txt com o nome inserido pelo usuario em
    // um arquivo binario e o abre para a utilizacao do programa.
    string fileName;
    cout << "Arquivo mvias.bin nao existe ainda, criando..." << endl;
    cout << "Entre com o nome do arquivo de texto a ser lido (default: mvias.txt): ";
    cin >> fileName;

    // processo de abertura do arquivo de texto com o nome fornecido 
    // pelo usuario em modo de leitura:
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

    // processo de abertura do arquivo binario em modo de escrita:
    ofstream treeCreation("../files/mvias.bin", ios::out | ios::binary);
    if(!treeCreation.is_open()){
        cerr << "Erro: arquivo mvias.bin nao pode ser aberto (provavel erro de armazenamento)." << endl;
        abort();
    }

    // escrita no arquivo binario
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

    tree.open("../files/mvias.bin", ios::in | ios::out | ios::binary);

    cout << "Arquivo binario criado com sucesso! Continuando o programa..." << endl << endl;
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

void TreeFile::setIndexRoot(int pos) {
    root = pos;
}

// ----------------------------------------------------------------
void TreeFile::writeMetaInfo() {
    node p;
    p.n = size;
    p.A[0] = root;
    tree.seekp(0);
    tree.write((const char *)(&p),sizeof(node));
}