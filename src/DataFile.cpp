// DataFile.h
// Class to abstract the DataFile manipulation

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
using namespace std;

#include "../include/DataFile.h"

// ----------------------------------------------------------------
DataFile::DataFile(){
    // Pre: nenhuma.
    // Pos: tenta abrir o arquivo binario, caso nao consiga, cria-o
    // com as meta-informacoes.
    data.open("../files/data.bin", ios::in | ios::out | ios::binary);

    if(!data.is_open()) {
        // createTree();
    } else {
        /*
        node p;
        tree.seekg(0);
        tree.read((char *)(&p), sizeof(node));
        size = p.n;
        root = p.A[0];
        tree.seekg(1 * sizeof(node));
        */
    }
}

// ----------------------------------------------------------------
DataFile::~DataFile(){
    // Pre: nenhuma.
    // Pos: fecha o arquivo binario.
    data.close();
}

// ----------------------------------------------------------------
DataFile::registry DataFile::getNextRegistry() {
    // Pre: arquivo binario data aberto.
    // Pos: retorna o registro seguinte ao lido anteriormente (util
    // ao se ler o arquivo sequencialmente).
    registry r;
    data.read((char *)(&r), sizeof(registry));

    if(! data.eof()) { // tratamento de erro no qual tenta-se ler um registro depois do final do arquivo
        return r;
    } else {
        data.seekg(0);
        data.read((char *)(&r), sizeof(registry));
        return r;
    }
}

// ----------------------------------------------------------------
DataFile::registry DataFile::getNthRegistry(int n) {
    // Pre: arquivo binario data aberto e um inteiro correspondente
    // ao index do registro que se deseja obter.
    // Pos: retorna o registro correspondente ao index dado caso seja
    // um index valido. Caso contrario, escreve uma mensagem de erro
    // e aborta o programa.
    data.seekg(n * sizeof(registry));
    registry r;
    data.read((char *)(&r), sizeof(registry));

    if(! data.eof()) { // tratamento de erro no qual tenta-se ler um registro depois do final do arquivo
        return r;
    } else {
        cout << "Erro: tentativa de acessar registro inexistente."<< endl;
        abort();
    }
}

// ----------------------------------------------------------------
void DataFile::writeRegistry(registry newRegistry) { 
    data.seekp(++size * sizeof(registry));
    data.write((const char *)(&newRegistry), sizeof(registry));
}

// ----------------------------------------------------------------
void DataFile::printRegistry(registry registry) {
    // Pre: uma estrutura de registro a ser impressa. Nao precisa do 
    // arquivo aberto e e um metodo estatico.
    // Pos: imprime na tela o registro dado com uma formatacao amigavel.
    cout << fixed << setprecision(2) << setw(12) << registry.mass << " * 10^15 Kg, "
    << right << setw(24) << registry.name << ", "
    << left << fixed << setprecision(3) << setw(6) << registry.radius << " Km, "
    << left << fixed << setprecision(1) << setw(4) << registry.distanceFromSun << " * 10^6 Km, "
    << left << fixed << setprecision(1) << setw(4) << registry.density << " Kg/m^3, "
    cout << endl;
}