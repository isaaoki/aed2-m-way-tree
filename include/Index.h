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
            stack<tuple<TreeFile::node, int>> visitedNodes; // Nos percorridos e posicao
            stack<int> i; // Ai dos nos percorridos
            int b;
            bool found;
            int read = 0;
        };

        mSearchResult mSearch(double x);
        tuple<int, int> insertB(double x, int b);
                
    private:
        TreeFile* treeFile;
        
        int linearSearch(vector<double> &K, double x, int n);
        mSearchResult mSearch(TreeFile* treeFile, double x);
        tuple<int, int> insertB(TreeFile* treeFile, double x, int b);

};

#endif
