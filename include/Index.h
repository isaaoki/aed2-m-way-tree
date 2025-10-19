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
            stack<tuple<TreeFile::node, int>> visitedNodes; // Nos percorridos e posicao
            stack<int> i; // Ai dos nos percorridos
            int b;
            bool found;
            int read = 0;
        };

        mSearchResult mSearch(int x);
        tuple<int, int> insertB(DataFile::registry* newRegistry);
                
    private:
        TreeFile* treeFile;
        DataFile* dataFile;
        
        int linearSearch(vector<int> &K, int x, int n);
        mSearchResult mSearch(TreeFile* treeFile, int x);
        tuple<int, int> insertB(DataFile::registry* newRegistry, TreeFile* treeFile, DataFile* dataFile);

};

#endif
