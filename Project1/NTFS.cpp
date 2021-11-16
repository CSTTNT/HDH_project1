#include "ConvertFunct.h"
#include "NTFS.h"



ostream& operator << (ostream& out, FILE_INFO f) // phu tro xuat TapTin/ThuMuc
{
    int i = f.name.length();
    //xuat data neu file txt
    if (((int)f.name[i - 2]) == 116 && ((int)f.name[i - 4]) == 120 && ((int)f.name[i - 6]) == 116)
        out << f.name << " (sector: " << f.indexSector << " ; Noi dung: " << f.data << " ; Kich thuoc: " << f.sizeData << " Byte)\n";
    else if (f.attribute == 268435456) // thu muc
        out << f.name << " (sector: " << f.indexSector << ")\n";
    else
        out << f.name << " (sector: " << f.indexSector << " ; Kich thuoc: " << f.sizeData << " Byte)" << " => Hay mo tap tin bang ung dung tuong thich\n";
    return out;
}

int ReadDrive_NTFS(LPCWSTR  drive) //doc o dia
{
    DWORD bytesRead;
    HANDLE device = NULL;
    BYTE sector[512];
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

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {

        cout << "Read Boost Sector Success!\n\n";
        cout << "- So byte/sector: " << ReadBytes2Int(sector, "0", "B", 2) << endl;
        int Sc = ReadBytes2Int(sector, "0", "D", 1);
        cout << "- So sector/cluster: " << Sc << endl;
        cout << "- Tong so sector: " << ReadBytes2Int(sector, "2", "8", 8) << endl;
        int cMFT = ReadBytes2Int(sector, "3", "0", 8);
        cout << "- Cluster bat dau cua MFT: " << cMFT << endl;
        cout << "- Cluster cua MFT mirror: " << ReadBytes2Int(sector, "3", "8", 8) << endl;
        /*system("pause");
        system("cls");*/

        //doc tap tin/thu muc
        readMFT(device, Sc * cMFT);


    }
    CloseHandle(device);
}

FILE_INFO readEntry(BYTE sector[1024]) // Lay cac thong so can thiet cua entry
{
    FILE_INFO f_info;
    f_info.ID = ReadBytes2Int(sector, "2", "C", 4);
    int posSTAND = ReadBytes2Int(sector, "1", "4", 2);
    int sizeSTAND = Byte2Int(sector, posSTAND + 4, 4);
    int sizeFILENAME = Byte2Int(sector, posSTAND + sizeSTAND + 4, 4);
    int posDataFILE = Byte2Int(sector, posSTAND + sizeSTAND + 20, 2);
    int sizeDataFILE = Byte2Int(sector, posSTAND + sizeSTAND + 16, 4);
    f_info.IDparent = Byte2Int(sector, posSTAND + sizeSTAND + posDataFILE, 5);
    f_info.name = Bytes2Str(sector, posSTAND + sizeSTAND + posDataFILE + 66, sizeDataFILE - 66);
    int posOBJ = posSTAND + sizeSTAND + sizeFILENAME;
    int sizeOBJ = Byte2Int(sector, posOBJ + 4, 4);
    int posDataDATA = Byte2Int(sector, posOBJ + sizeOBJ + 20, 2);
    string data = "";
    while (sector[posOBJ + sizeOBJ + posDataDATA] != 0xFF) {
        data += sector[posOBJ + sizeOBJ + posDataDATA];
        posDataDATA++;
    }
    f_info.data = data;
    f_info.sizeData = Byte2Int(sector, posOBJ + sizeOBJ + 16, 4);
    return f_info;
}

int readAttribute(BYTE sector[1024]) //doc attribute cua entry de xac dinh ThuMuc/TapTin
{
    int posSTAND = ReadBytes2Int(sector, "1", "4", 2);
    int sizeSTAND = Byte2Int(sector, posSTAND + 4, 4);
    int posDataFILE = Byte2Int(sector, posSTAND + sizeSTAND + 20, 2);
    return Byte2Int(sector, posSTAND + sizeSTAND + posDataFILE + 56, 4);
}

int readMFT(HANDLE device, int posMFT)  // doc bang MFT (duyet cac entry co trong MFT)
{
    if (device != NULL) {
        BYTE sectorMFTentry[1024];
        DWORD BytesMFTentry;
        int i = 0;
        queue<FILE_INFO> MFTentries; // luu entry thu muc va tap tin
        while (i < 200) // ko biet diem dung cua MFT => duyet tam 100 entry
        {
            SetFilePointer(device, (512 * (posMFT + i)), NULL, FILE_BEGIN);
            if (!ReadFile(device, sectorMFTentry, 1024, &BytesMFTentry, NULL)) {
                cout << "Error in Reading MFT";
                return 0;
            }
            int attr = readAttribute(sectorMFTentry);
            if (attr == 268435456 || attr == 32) // 0x00000010 or 0x20000000   Tap Tin or Thu muc
            {
                FILE_INFO f = readEntry(sectorMFTentry);
                f.indexSector = posMFT + i;
                f.attribute = attr;
                MFTentries.push(f);
            }
            i += 2;
        }
        cout << "\nCay thu muc goc: \n\n";
        print_RootFolder(MFTentries, 5, 0);
    }
    return 1;
}

void print_RootFolder(queue<FILE_INFO>& src, int IDparent, int n_setw) //in cay thu muc goc
{
    int k = src.size();
    while (k) {
        FILE_INFO temp = src.front();
        if (temp.IDparent == IDparent) {
            if (temp.attribute == 268435456)//thu muc
            {
                cout << setw(n_setw) << left << " " << "+ " << temp;
                src.pop();
                print_RootFolder(src, temp.ID, n_setw + 5);
                k = src.size();
                continue;
            }
            else {
                src.pop();
                cout << setw(n_setw) << left << " " << "- " << temp;
            }
        }
        src.push(src.front());
        src.pop();
        k--;
    }
}