#pragma once
#include "ConvertFunct.h"

struct FILE_INFO  //luu cac thong so cua file/thu muc doc tu entry
{
    int indexSector;
    int ID;
    int IDparent;
    int attribute;
    string name;
    string data;
    int sizeData;
};

ostream& operator << (ostream& out, FILE_INFO f);

int ReadDrive_NTFS(LPCWSTR  drive);

FILE_INFO readEntry(BYTE sector[1024]);

int readAttribute(BYTE sector[1024]);

int readMFT(HANDLE device, int posMFT);

void print_RootFolder(queue<FILE_INFO>& src, int IDparent, int n_setw);

