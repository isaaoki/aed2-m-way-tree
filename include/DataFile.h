// DataFile.h
// Class to abstract the DataFile manipulation

#ifndef DATAFILE_H
#define DATAFILE_H

class DataFile {
    public:
        DataFile();
        ~DataFile();

        struct registry { // celestialBody
            double mass; // in TeraTons or 10^15 Kg
            char name[25];
            float radius; // in Km
            double distanceFromSun; // in GigaMeters or 10^6 Km
            int satellites;
            bool isMoon;
        };

        registry getNextRegistry();
        registry getNthRegistry(int n);
        int writeRegistry(registry newRegistry);
        int removeRegistry(int pos);
        static void printRegistry(registry registry);
        void printFile();
        int getSize();

    private:
        fstream data;
        int size; // total de registros
        void createFile();
        void writeMetaInfo();
};

#endif