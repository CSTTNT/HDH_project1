#pragma once
#include "ConvertFunct.h"


struct Entry {
    // Main entry
    BYTE fileName[8];
    BYTE extension[3];
    BYTE Attributes;
    BYTE reserved;
    BYTE createTime_ms;
    //This is file name of sub entry
    BYTE createTime[2];
    BYTE createDate[2];
    BYTE accessedDate[2];
    // 2 bytes in 14
    BYTE startCluster[2];
    BYTE modifiedTime[2];
    BYTE modifiedDate[2];
    // 2 bytes in 1A
    BYTE moreStartCluster[2];
    // size of Main entry/ Last bytes of name in sub entry
    BYTE sizeofFile[4];
};

// Print name of entry
void printName(Entry entry, int mode);
// find Clusters of entry based on FAT
void findCluster(HANDLE device, int FATpos, vector<int>& clusArr, int firstClus);
// find Sectors of entry based on FAT
void findSector(vector<int> clusArr, vector<int>& secArr, int SC, int SB, int NF, int SF);
// Read RDET
void readRDET(HANDLE device, int FATpos, int rdetPos, int SB, int SC, int NF, int SF);
//Read DATA of .TXT file
void readData(HANDLE device, vector<int> secArr);
//Read BoostSector
void ReadDrive_FAT32(HANDLE drive, BYTE boostSector[]);