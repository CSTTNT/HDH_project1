#pragma once
#include "ConvertFunct.h"

struct FILE_INFO  //luu cac thong so cua file/thu muc doc tu entry
{
    string indexSector;
    int ID;
    int IDparent;
    int attribute;
    string name;
    wchar_t* data;
    int sizeData;
};

ostream& operator << (ostream& out, FILE_INFO f);

void ReadDrive_NTFS(HANDLE device, BYTE sector[]);

FILE_INFO readEntry(BYTE sector[1024]);

int readAttribute(BYTE sector[1024]);

int readClusterMFT(HANDLE device, int posMFT);

int readMFT(HANDLE device, int Sc, int cMFT);

void print_RootFolder(queue<FILE_INFO>& src, int IDparent, int n_setw);

