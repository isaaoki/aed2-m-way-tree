// Index.h
// Index usage and manipulation

#ifndef INDEX_H
#define INDEX_H

#include "../include/TreeFile.h"
#include <stack>

class Index {
    public:
        Index(TreeFile* t);
        ~Index();

        struct mSearchResult {
            int pos;
            int i;
            bool found;
            stack<tuple<TreeFile::node, int>> visitedNodes;
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
