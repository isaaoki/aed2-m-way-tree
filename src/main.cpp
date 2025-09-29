#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <limits.h>
using namespace std;

#include "../include/TreeFile.h"
#include "../include/Index.h"

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
                     << get<1>(searchResult.visitedNodes.top()) << "," << searchResult.i.top() << ",";
                cout << (searchResult.found ? "true" : "false") << ")" << endl;

                break;
            case 4:
                cout << "Digite o nro ser inserido: ";
                cin >> x;
                index.insertB(x);
                // inserir corpo celeste no arquivo principal e no indice
                break;
            case 5: 
                isRunning = false;
                break;
            default:
                cout << "Opcao invalida! Tente novamente...";
                break;
        }
    } while (isRunning);

    return 0; // fecha o arquivo e salva-o no disco
}