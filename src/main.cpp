#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <limits.h>
#include <tuple>
using namespace std;

#include "../include/TreeFile.h"
#include "../include/Index.h"
#include "../include/DataFile.h"

void printMenu() {
    cout << "MAIN MENU" << endl;
    cout << "1) Print index (b-tree)" << endl;
    cout << "2) Print main file" << endl;
    cout << "3) Search celestial body by mass" << endl;
    cout << "4) Insert new celestial body" << endl;
    cout << "5) Remove celestial body by mass" << endl;
    cout << "6) Exit" << endl;
    cout << "Choose an option: ";
}

stack<int> printStack(stack<int> s) { // Uso: stackASerImpressa = printStack(stackASerImpressa)
    stack<int> temp;
    stack<int> result;
    for(int i = s.size(); i > 0; i--) {
        cout << s.top() << ",";
        temp.push(s.top());
        s.pop();
    }
    cout << endl;
    for(int i = temp.size(); i > 0; i--) {
        result.push(temp.top());
        temp.pop();
    }
    return result;
}

int main() {
    TreeFile t;
    DataFile data;
    Index index(&t, &data);
    Index::mSearchResult searchResult;
    DataFile::registry r;
    TreeFile::node node;
    bool isRunning = true;
    int option, B;
    char moon;
    tuple<int, int> access;
    
    do {
        printMenu();
        cin >> option;
        cout << endl;

        switch (option) {
            case 1:
                t.printTree();
                break;
            case 2:
                data.printFile();
                break;
            case 3:
                cout << "SEARCH CELESTIAL BODY BY MASS" << endl;
                cout << "Enter mass (10^15 Kg): ";
                cin >> r.mass;
                searchResult = index.mSearch(r.mass);
                node = get<0>(searchResult.visitedNodes.top());
                
                cout << endl << "mSearch result: ";
                cout << setw(4) << r.mass << " ("
                     << get<1>(searchResult.visitedNodes.top()) << "," 
                     << searchResult.i.top() << ","
                     << (searchResult.found ? "true" : "false") << ")" << endl;
                cout << "READ ACCESS: " << searchResult.read << endl;

                if (searchResult.found) {
                    cout << string(110, '-') << endl;
                    cout << left 
                    << setw(17) << "Mass (10^15 Kg)" 
                    << setw(26) << "Name"
                    << setw(13) << "Radius (km)"
                    << setw(29) << "Distance from Sun (10^6 Km)"
                    << setw(12) << "Satellites"
                    << setw(9) << "isMoon?" << endl;

                    data.printRegistry(data.getNthRegistry(node.B[searchResult.i.top()]));

                    cout << string(110, '-') << endl;
                } else {
                    cout << "Error: result not found." << endl;
                }
                
                break;
            case 4:
                cout << "INSERT CELESTIAL BODY" << endl;
                cout << "Enter with the planet's data: " << endl;
                cout << "- Mass (10^15 Kg): ";
                cin >> r.mass;
                cout << "- Name (max: 24): ";
                cin >> r.name;
                cout << "- Radius (Km): ";
                cin >> r.radius;
                cout << "- Distance from Sun (10^6 Km): ";
                cin >> r.distanceFromSun;
                cout << "- Number of Sattelites: ";
                cin >> r.satellites;
                cout << "- Is moon (T or F)? ";
                cin >> moon;

                if (moon == 't' || moon == 'T') r.isMoon = true;
                else r.isMoon = false;                

                access = index.insertB(&r);

                cout << endl << "READ ACCESS: " << get<0>(access) << endl;
                cout << "WRITE ACCESS: " << get<1>(access) << endl;
                break;
            case 5: 
                cout << "REMOVE CELESTIAL BODY BY MASS" << endl;
                cout << "Enter mass (10^15 Kg): ";
                cin >> r.mass;
                
                access = index.deleteB(r.mass);

                cout << endl << "READ ACCESS: " << get<0>(access) << endl;
                cout << "WRITE ACCESS: " << get<1>(access) << endl;
                break;
            case 6: 
                isRunning = false;
                break;
            default:
                cout << "Invalid option! Try again..." << endl;
                break;
        }
        cout << endl;
    } while (isRunning);

    return 0; // fecha o arquivo e salva-o no disco
}