// TreeFile.h
// Class to abstract the TreeFile manipulation

#ifndef TREEFILE_H
#define TREEFILE_H

class TreeFile {
    public:
        TreeFile();
        ~TreeFile();

        static const int m = 3;
        struct node {
            int n;
            int A[m];
            int K[m];
        };

        node getTreeRoot();
        node getNextNode();
        node getNthNode(int n);
        void writeNode(TreeFile::node newNode);
        void removeNode(int n);

    private:
        fstream tree;
        int size;
        int root;
        void writeMetaInfo();
};

#endif
