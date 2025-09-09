#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;
#include "TreeFile.cpp"

int main() {
    TreeFile t;
    TreeFile::node node;
    node = t.getNthNode(1);
    cout << node.n << endl;
    t.~TreeFile();
}

