// Project1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "NTFS.h"
#include "FAT32.h"

int ReadDrive(LPCWSTR  drive) {
    DWORD bytesRead;
    HANDLE device = NULL;
    BYTE boostSector[512];
    int readPoint = 0;

    device = CreateFile(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, boostSector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        string format = ReadBytes2Str(boostSector, "5", "2", 8);
        if (format.find("FAT") != std::string::npos) {
            cout << "========" << format << "=========";
            ReadDrive_FAT32(device, boostSector);
        }
        else {
            cout << "======== NTFS =========";
            ReadDrive_NTFS(device, boostSector);
        }
    }
    CloseHandle(device);
}

int main()
{
    wstring disk;
    cout << "Nhap ten o dia(vd: F): ";
    wcin >> disk;
    disk = L"\\\\.\\" + disk + L":";

    LPCWSTR device = disk.c_str();
    ReadDrive(device);
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
