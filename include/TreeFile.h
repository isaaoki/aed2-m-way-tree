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
            int B[m];
        };

        node getTreeRoot();
        node getNextNode();
        node getNthNode(int n);
        void writeNode(node newNode);
        static void printNode(node node);
        void printTree();
        int getSize();
        int getIndexRoot();

    private:
        fstream tree;
        int size; // total de nos
        int root;
        void writeMetaInfo();
        void createTree();
};

#endif
