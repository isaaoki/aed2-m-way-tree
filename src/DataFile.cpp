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
        createFile();
    } else {
        registry r;
        data.seekp(0);
        data.read((char *)(&r), sizeof(registry));
        size = r.satellites;
        data.seekg(1 * sizeof(registry));
    }
}

// ----------------------------------------------------------------
DataFile::~DataFile(){
    // Pre: nenhuma.
    // Pos: fecha o arquivo binario.
    writeMetaInfo();
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
        cout << "Error: inexistent registry being accessed."<< endl;
        abort();
    }
}

// ----------------------------------------------------------------
int DataFile::writeRegistry(registry newRegistry) { 
    // Pre: arquivo binario data aberto e uma estrutura de registro 
    // a ser escrita em memoria secundaria.
    // Pos Escreve a estrutura de registro em memoria secundaria e
    // retorna o index onde a mesma foi escrita.
    data.seekp(++size * sizeof(registry));
    data.write((const char *)(&newRegistry), sizeof(registry));
    return size;
}

// ----------------------------------------------------------------
void DataFile::printRegistry(registry registry) {
    // Pre: uma estrutura de registro a ser impressa. Nao precisa do 
    // arquivo aberto e e um metodo estatico.
    // Pos: imprime na tela o registro dado com uma formatacao amigavel.
    cout << left << fixed << setprecision(2)
         << setw(17) << registry.mass 
         << setw(26) << registry.name 
         << setw(13) << registry.radius
         << setw(29) << registry.distanceFromSun
         << setw(12) << registry.satellites
         << setw(9) << (registry.isMoon ? "YES" : "NO") << endl;
}

// ----------------------------------------------------------------
void DataFile::printFile() {
    // Pre: arquivo binario data aberto.
    // Pos: imprime na tela o arquivo de dados completo, inclusive 
    // seu tamanho com uma formatacao amigavel.
    cout << "CELESTIAL BODIES" << endl;
    cout << "Number of registries = " << size << endl;
    cout << string(110, '-') << endl;

    cout << left 
         << setw(17) << "Mass (10^15 Kg)" 
         << setw(26) << "Name"
         << setw(13) << "Radius (km)"
         << setw(29) << "Distance from Sun (10^6 Km)"
         << setw(12) << "Satellites"
         << setw(9) << "isMoon?" << endl;

    cout << string(110, '-') << endl;

    data.seekg(1 * sizeof(registry));
    for (int i = 1; i <= size; i++) {
        printRegistry(getNextRegistry());
    }
    data.seekg(1 * sizeof(registry));

    cout << string(110, '-') << endl;
}

// ----------------------------------------------------------------
void DataFile::createFile() {
    // Pre: nenhuma.
    // Pos: cria um arquivo de arvore de m-vias vazio.
    ofstream dataCreation("../files/data.bin", ios::out | ios::binary);
    if(!dataCreation.is_open()){
        cerr << "Error: data.bin file cannot be open (possible memory error)." << endl;
        abort();
    }
    registry r;
    r.satellites = 0;
    dataCreation.seekp(0);
    dataCreation.write((const char *)(&r),sizeof(registry));
    dataCreation.close();

    size = 0;
    data.open("../files/data.bin", ios::in | ios::out | ios::binary);

    cout << "Binary data file created with success! Continuing the execution..." << endl << endl;
}

// ----------------------------------------------------------------
void DataFile::writeMetaInfo() {
    // Pre: classe inicializada.
    // Pos: salva as meta informacoes do arquivo em memoria secundaria
    // (tamanho).
    registry r;
    r.satellites = size;
    data.seekp(0);
    data.write((const char *)(&r), sizeof(registry));
}