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
        size = r.mass;
        freeRegistries = getFreeRegistries(r.satellites);
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
    // Pos: Escreve a estrutura de registro em memoria secundaria e
    // retorna o index onde a mesma foi escrita.
    int pos;
    if(freeRegistries.empty()) {
        pos = ++size;
        data.seekp(pos * sizeof(registry));
    } else {
        pos = freeRegistries.top();
        data.seekp(freeRegistries.top() * sizeof(registry));
        freeRegistries.pop();
    }
    data.write((const char *)(&newRegistry), sizeof(registry));
    return pos;
}

// ----------------------------------------------------------------
int DataFile::writeRegistry(registry newRegistry, int pos) { 
    // Pre: arquivo binario data aberto, uma estrutura de registro 
    // a ser escrita em memoria secundaria e o index da estrutura.
    // Pos: Escreve a estrutura de registro em memoria secundaria 
    // na posicao indicada e retorna o index onde a mesma foi escrita.
    data.seekp(pos * sizeof(registry));
    data.write((const char *)(&newRegistry), sizeof(registry));
    if (pos > size) size = pos;
    return size;
}

// ----------------------------------------------------------------
void DataFile::removeRegistry(int pos) {
    // Pre: arquivo binario data aberto e a posicao do registro a
    // ser marcado como removido.
    // Pos: marca o registro como removido e decrementa o tamanho da 
    // arvore.
    registry emptyRegistry;
    emptyRegistry.mass = -1;
    emptyRegistry.satellites = pos;
    if(!freeRegistries.empty()) {
        data.seekp(freeRegistries.top() * sizeof(registry));
        data.write((const char*)(&emptyRegistry), sizeof(registry));
    }
    freeRegistries.push(pos);
}

// ----------------------------------------------------------------
void DataFile::printStack(stack<int> s) {
    // Pre: uma pilha de inteiros. Nao precisa do arquivo aberto e
    // e um metodo estatico.
    // Pos: imprime na tela a pilha com uma formatação amigavel.
    stack<int> temp = s;
    cout << "Stack of free registries: ";
    for(int i = s.size(); i > 1; i--) {
        cout << temp.top() << ", ";
        temp.pop();
    }
    cout << temp.top() << "." << endl;
    cout << endl;
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
    registry registry;
    cout << "CELESTIAL BODIES" << endl;
    cout << "Number of Celestial Bodies = " << size - freeRegistries.size() << ", Number of lines on the file = " << size << endl;
    cout << string(110, '-') << endl;

    cout << left 
         << setw(17) << "Mass (10^15 Kg)" 
         << setw(26) << "Name"
         << setw(13) << "Radius (km)"
         << setw(29) << "Distance from Sun (10^6 Km)"
         << setw(12) << "Satellites"
         << setw(9) << "isMoon?" << endl;

    cout << string(110, '-') << endl;

    if(!freeRegistries.empty()) {
        registry.mass = -1;
        registry.satellites = 0;
        writeRegistry(registry, freeRegistries.top());
    }
    data.seekg(1 * sizeof(registry));
    for (int i = 1; i <= size; i++) {
        registry = getNextRegistry();
        if(registry.mass != -1) {
            printRegistry(registry);
        } else {
            cout << string(48, '-') << "empty registry" << string(48, '-') << endl;
        }
    }
    data.seekg(1 * sizeof(registry));

    cout << string(110, '-') << endl;
    if(!freeRegistries.empty())
        printStack(freeRegistries);
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
    r.mass = 0;
    dataCreation.seekp(0);
    dataCreation.write((const char *)(&r),sizeof(registry));
    dataCreation.close();

    size = 0;
    data.open("../files/data.bin", ios::in | ios::out | ios::binary);

    cout << "Binary data file created with success! Continuing the execution..." << endl << endl;
}

// ----------------------------------------------------------------
stack<int> DataFile::getFreeRegistries(int freeRegistry) {
    // Pre: classe inicializada e um inteiro correspondente a posicao
    // do registro apagado atual.
    // Pos: retorna uma pilha com os registros livres em disco.
    stack<int> result;
    int currFreeRegistry = freeRegistry;

    while(currFreeRegistry != 0) {
        result.push(currFreeRegistry);
        currFreeRegistry = getNthRegistry(currFreeRegistry).satellites;
    }
    
    return result;
}

// ----------------------------------------------------------------
void DataFile::writeMetaInfo() {
    // Pre: classe inicializada.
    // Pos: salva as meta informacoes do arquivo em memoria secundaria
    // (tamanho).
    registry lastFreeRegistry;
    registry zero;
    zero.mass = size;
    if(freeRegistries.empty()) {
        zero.satellites = 0;
    } else {
        lastFreeRegistry.satellites = 0;
        data.seekp(freeRegistries.top() * sizeof(registry));
        data.write((const char *)(&lastFreeRegistry),sizeof(registry));

        for(int i = freeRegistries.size(); i > 1; i--) {
            freeRegistries.pop();
        }
        zero.satellites = freeRegistries.top();
    }
    data.seekp(0);
    data.write((const char *)(&zero), sizeof(registry));
}