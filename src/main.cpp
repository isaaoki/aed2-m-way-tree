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
    cout << "2) Imprimir arquivo principal" << endl;
    cout << "3) Buscar planeta por distancia" << endl;
    cout << "4) Inserir novo planeta" << endl;
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
                cout << "BUSCA PELA DISTANCIA" << endl;
                cout << "Digite a distancia: ";
                cin >> x;
                searchResult = index.mSearch(x);

                cout << setw(4) << x << " ("
                     << get<1>(searchResult.visitedNodes.top()) << "," 
                     << searchResult.i.top() << ","
                     << (searchResult.found ? "true" : "false") << ")" << endl;

                // Imprimir dados do corpo celestre do arquivo principal datafile.read(get<1>(searchResult.visitedNodes.top());
                break;
            case 4:
                cout << "INSERIR PLANETA" << endl;
                cout << "Digite o nro ser inserido: ";
                cin >> x;
                index.insertB(x);
                // Inserir corpo celeste no arquivo principal
                break;
            case 5: 
                isRunning = false;
                break;
            default:
                cout << "Opcao invalida! Tente novamente..." << endl;
                break;
        }
        cout << endl;
    } while (isRunning);

    return 0; // fecha o arquivo e salva-o no disco
}