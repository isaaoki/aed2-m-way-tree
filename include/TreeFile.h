// TreeFile.h
// Class to abstract the TreeFile manipulation

#include <stack>

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
        void writeNode(node newNode, int pos);
        void removeNode(int pos);
        static void printNode(node node);
        void printTree();
        int getM();
        int getSize();
        int getIndexRoot();
        void setIndexRoot(int pos);

        stack<int> getFreeNodes(int freeNode);
    private:
        fstream tree;
        int size; // total de nos
        int root;
        stack<int> freeNodes;
        void writeMetaInfo();
        void createTree();
};

#endif