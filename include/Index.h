// Index.h
// Index usage and manipulation

#ifndef INDEX_H
#define INDEX_H

#include "../include/TreeFile.h"

class Index {
    public:
        Index(TreeFile* t);
        ~Index();

        struct mSearchResut {
            int pos;
            int i;
            bool found;
        };

        mSearchResut mSearch(int x);

    private:
        TreeFile* treeFile;
        
        int linearSearch(vector<int> &K, int x, int n);
        mSearchResut mSearch(TreeFile* treeFile, int x);

};

#endif
