#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <limits.h>
using namespace std;

#include "../include/TreeFile.h"
#include "../include/Index.h"

int main() {
    TreeFile t;
    Index index(&t);
    Index::mSearchResult searchResult;
    int x;
    string isRunning;

    do {
        t.printTree();
        cout << "Chave de busca: ";
        cin >> x;

        searchResult = index.mSearch(x);

        cout << setw(4) << x << " ("
             << searchResult.pos << "," << searchResult.i << ",";
        cout << (searchResult.found ? "true" : "false") << ")" << endl;

        cout << "Continuar busca (s/n)? ";
        cin >> isRunning;
    } while (isRunning != "n");

    return 0; // fecha o arquivo e salva-o no disco
}