// Index.h
// Index usage and manipulation
#include <stack>
#include <tuple>
using namespace std;

#ifndef INDEX_H
#define INDEX_H

#include "../include/TreeFile.h"
#include "../include/DataFile.h"

class Index {
    public:
        Index(TreeFile* t, DataFile* data);
        ~Index();

        struct mSearchResult {
            stack<tuple<TreeFile::node, int>> visitedNodes; // Nós percorridos e posicao na árvore
            stack<int> i; // Ai dos nos percorridos
            bool found;  
            int read = 0; // Numero de leituras
        };

        mSearchResult mSearch(int x);
        tuple<int, int> insertB(DataFile::registry* newRegistry);
        tuple<int, int> deleteB(int x);
        void bulkInsert();

    private:
        TreeFile* treeFile;
        DataFile* dataFile;
        
        int linearSearch(vector<int> &K, int x, int n);
        void shiftLeft(TreeFile::node &node, int start);
        void replaceWithSuccessor(TreeFile::node &nodeP, int &indexP, int &posP, int &read, int &write, mSearchResult &searchResult);
        void redistributeRight(TreeFile::node &nodeP, TreeFile::node &nodeR, TreeFile::node &nodeQ, int j, int posP, int posR, int posQ, int &write);
        void redistributeLeft(TreeFile::node &nodeP, TreeFile::node &nodeR, TreeFile::node &nodeQ, int j, int posP, int posR, int posQ, int &write);
        void mergeNodes(TreeFile::node &nodeLeft, TreeFile::node &nodeParent, TreeFile::node &nodeRight, int j, int posLeft, int posParent, int posRight, int &write);
};

#endif
