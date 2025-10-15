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
            double K[m];
            int B[m];
        };

        node getTreeRoot();
        node getNextNode();
        node getNthNode(int n);
        void writeNode(node newNode);
        void writeNode(node newNode, int pos);
        void removeNode(int pos);
        static void printNode(node node);
        void printTree();
        int getM();
        int getSize();
        int getIndexRoot();
        void setIndexRoot(int pos);

    private:
        fstream tree;
        int nodeSize;
        int size; // total de nos
        int root;
        void writeMetaInfo();
        void createTree();
};

#endif