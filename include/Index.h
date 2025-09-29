// Index.h
// Index usage and manipulation
#include <stack>
#include <tuple>
using namespace std;

#ifndef INDEX_H
#define INDEX_H

#include "../include/TreeFile.h"

class Index {
    public:
        Index(TreeFile* t);
        ~Index();

        struct mSearchResult {
            stack<tuple<TreeFile::node, int>> visitedNodes; // Nós percorridos e posição
            stack<int> i; // Ai dos nós percorridos
            bool found;
        };

        mSearchResult mSearch(int x);
        void insertB(int x);
                
    private:
        TreeFile* treeFile;
        
        int linearSearch(vector<int> &K, int x, int n);
        mSearchResult mSearch(TreeFile* treeFile, int x);
        void insertB(TreeFile* treeFile, int x);

};

#endif
