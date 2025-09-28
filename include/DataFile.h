
// DataFile.h
// Class to abstract the DataFile manipulation

#ifndef DATAFILE_H
#define DATAFILE_H

class DataFile {
    public:
        DataFile();
        ~DataFile();

        struct registry {
            int k;
        };

        registry getNextRegistry();
        registry getNthRegistry(int n);
        static void printRegistry(registry registry);
        void printFile();
        int getSize();

    private:
        fstream data;
        int size; // total de registros
        void createFile();
};

#endif
