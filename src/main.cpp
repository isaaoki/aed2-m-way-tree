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
    cout << "5) Exit" << endl;
    cout << "Choose an option: ";
}

int main() {
    TreeFile t;
    DataFile data;
    Index index(&t);
    Index::mSearchResult searchResult;
    bool isRunning = true;
    DataFile::registry r;
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
                cout << "Enter mass: ";
                cin >> r.mass;
                searchResult = index.mSearch(r.mass);
                
                cout << "\nmSearch result: ";
                cout << setw(4) << r.mass << " ("
                     << get<1>(searchResult.visitedNodes.top()) << "," 
                     << searchResult.i.top() << ","
                     << (searchResult.found ? "true" : "false") << ")" << endl;
                cout << "READ ACCESS: " << searchResult.read << endl;

                if (searchResult.found) {
                    cout << string(100, '-') << endl;
                    cout << left 
                    << setw(18) << "Mass (10^15 Kg)" 
                    << setw(24) << "Name"
                    << setw(12) << "Radius (km)"
                    << setw(28) << "Distance from Sun (10^6 Km)"
                    << setw(12) << "Satellites"
                    << setw(8) << "isMoon?" << endl;

                    data.printRegistry(data.getNthRegistry(searchResult.b));

                    cout << string(100, '-') << endl;
                } else {
                    cout << "Result not found!" << endl;
                }
                
                break;
            case 4:
                cout << "INSERT CELESTIAL BODY" << endl;
                cout << "Enter whith the planet's data: " << endl;
                cout << "\nMass (10^15 Kg): ";
                cin >> r.mass;
                cout << "Name (max: 24): ";
                cin >> r.name;
                cout << "Radius (Km): ";
                cin >> r.radius;
                cout << "Distance from Sun (10^6 Km): ";
                cin >> r.distanceFromSun;
                cout << "Number Sattelites: ";
                cin >> r.satellites;
                cout << "Is moon (T or F)? ";
                cin >> moon;

                if (moon == 't' || moon == 'T') r.isMoon = true;
                else r.isMoon = false;                

                B = data.writeRegistry(r);
                access = index.insertB(r.mass, B);

                cout << "\nREAD ACCESS: " << get<0>(access) << endl;
                cout << "WRITE ACCESS: " << get<1>(access) << endl;
                break;
            case 5: 
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