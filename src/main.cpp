#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <limits.h>
using namespace std;

#include "../include/TreeFile.h"
#include "TreeFile.cpp"
#include "../include/Index.h"
#include "Index.cpp"

void printMenu() {
    cout << "MENU PRINCIPAL" << endl;
    cout << "1) Imprimir indice (arvore B)" << endl;
    cout << "2) Imprimir conteudo do arquivo principal" << endl;
    cout << "3) Buscar elemento no indice" << endl;
    cout << "4) Inserir novo elemento" << endl;
    cout << "5) Sair" << endl;
    cout << "Escolha uma opcao: ";
}

int main() {
    TreeFile t;
    Index index(&t);
    Index::mSearchResult searchResult;
    bool isRunning = true;
    int x, opcao;
    string corpo;
    float massa;

    do {
        printMenu();
        cin >> opcao;

        switch (opcao) {
            case 1: 
                t.printTree();
                break;
            case 2:
                // imprimir conteudo do arquivo principal
                break;
            case 3:
                cout << "Digite a chave de busca: ";
                cin >> x;
                searchResult = index.mSearch(x);

                cout << setw(4) << x << " ("
                     << searchResult.pos << "," << searchResult.i << ",";
                cout << (searchResult.found ? "true" : "false") << ")" << endl;

                break;
            case 4:
                cout << "Digite o nome do corpo a ser inserido: ";
                cin >> corpo;
                cout << "Digite a massa do corpo: ";
                cin >> massa; 
                // inserir corpo celeste no arquivo principal e no indice
                break;
            case 5: 
                isRunning = false;
                break;
            default:
                cout << "Opcao invalida! Tente novamente...";
                break;
        }
    } while (!isRunning);

    return 0; // fecha o arquivo e salva-o no disco
}