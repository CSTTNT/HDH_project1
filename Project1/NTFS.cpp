#include "ConvertFunct.h"
#include "NTFS.h"


void ReadDrive_NTFS(HANDLE device, BYTE sector[]) //doc BootSector
{
    cout << "Read Boost Sector Success!\n\n";
    cout << "- So byte/sector: " << ReadBytes2Int(sector, "0", "B", 2) << endl;
    int Sc = ReadBytes2Int(sector, "0", "D", 1);
    cout << "- So sector/cluster: " << Sc << endl;
    cout << "- Tong so sector: " << ReadBytes2Int(sector, "2", "8", 8) << endl;
    int cMFT = ReadBytes2Int(sector, "3", "0", 8);
    cout << "- Cluster bat dau cua MFT: " << cMFT << endl;
    cout << "- Cluster cua MFT mirror: " << ReadBytes2Int(sector, "3", "8", 8) << endl;
  
    //doc tap tin/thu muc trong MFT
    readMFT(device, Sc,cMFT);
}

FILE_INFO readEntry(BYTE sector[1024]) 
// Voi entry luu TapTin/Thumuc => doc entry de lay nhung thong so can thiet 
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
    int sizeDataDATA = Byte2Int(sector, posOBJ + sizeOBJ + 16, 4);
    //int n = 2 * (sizeDataDATA + 1);
    
    
    
    int len = f_info.name.length();
    //xuat data neu file txt
    if (((int)f_info.name[len - 2]) == 116 && ((int)f_info.name[len - 4]) == 120 && ((int)f_info.name[len - 6]) == 116) {
        BYTE* data = new BYTE[sizeDataDATA + 1];
        f_info.data = new wchar_t[sizeDataDATA + 1];
        int i = 0;
        while (i < sizeDataDATA) {
            data[i] = sector[posOBJ + sizeOBJ + posDataDATA];
            posDataDATA++;
            i++;
        }

        Convert_String(data, f_info.data, sizeDataDATA);
        delete[] data;
        
    }
    f_info.sizeData = sizeDataDATA;
    return f_info;
}

int readAttribute(BYTE sector[1024]) //doc attribute cua entry de xac dinh ThuMuc/TapTin
{
    int posSTAND = ReadBytes2Int(sector, "1", "4", 2);
    int sizeSTAND = Byte2Int(sector, posSTAND + 4, 4);
    int posDataFILE = Byte2Int(sector, posSTAND + sizeSTAND + 20, 2);
    return Byte2Int(sector, posSTAND + sizeSTAND + posDataFILE + 56, 4);
} 

int readClusterMFT(HANDLE device, int posMFT) // doc so cluster co trong bang MFT tai entry $MFT
{
    BYTE sectorMFTentry[1024];
    DWORD BytesMFTentry;
    SetFilePointer(device, (512 * posMFT), NULL, FILE_BEGIN); 
    if (!ReadFile(device, sectorMFTentry, 1024, &BytesMFTentry, NULL)) {
        cout << "Error in Reading MFT";
        return -1;
    }
    int posSTAND = ReadBytes2Int(sectorMFTentry, "1", "4", 2);
    int sizeSTAND = Byte2Int(sectorMFTentry, posSTAND + 4, 4);
    int sizeFILENAME = Byte2Int(sectorMFTentry, posSTAND + sizeSTAND + 4, 4);
    int posDATA = posSTAND + sizeSTAND + sizeFILENAME;
    int sizeDATA = Byte2Int(sectorMFTentry, posDATA + 4, 4);
    BYTE* data= new BYTE[sizeDATA]; // luu tru Attribute $DATA 
    for (int i = 0; i < sizeDATA; i++)
        data[i] = sectorMFTentry[posDATA + i]; 

    // doc byte thu 32 header Attribute $DATA xac dinh vung noi dung cua $DATA
    // bat dau tai offset nao?
    int a = Byte2Int(sectorMFTentry, posDATA + 32, 2);
    string off = dec2hex(a);
   
    // doc so cluster cua MFT tai byte thu 2 o vung noi dung 
    string row = "", col = "";
    row += off[0], col += off[1];
    int n_clus = (int) data[16 * hex2dec(row.c_str()) + hex2dec(col.c_str()) + 1];
    delete[] data;
    return n_clus;
}

int readMFT(HANDLE device, int Sc, int cMFT)  // doc bang MFT (duyet cac entry co trong MFT)
{
    unsigned long posMFT = Sc * cMFT; // sector bat dau cua MFT
    if (device != NULL) {
        BYTE sectorMFTentry[1024];
        DWORD BytesMFTentry;
        queue<FILE_INFO> MFTentries; // luu nhung entry la thu muc hoac tap tin
        int n_clus = readClusterMFT(device, posMFT); // so cluster cua MFT
        unsigned long n_sec = Sc * n_clus;
        for (int i = 2; i < n_sec; i += 2)
        {
            SetFilePointer(device, (512 * (posMFT + i)), NULL, FILE_BEGIN);
            if (!ReadFile(device, sectorMFTentry, 1024, &BytesMFTentry, NULL)) {
                cout << "Error in Reading MFT";
                return 0;
            }
            int attr = readAttribute(sectorMFTentry);
            if (attr == 268435456 || attr == 32) // 0x00000010 or 0x20000000  =>  Tap Tin or Thu muc
            {

                FILE_INFO f = readEntry(sectorMFTentry);
                f.indexSector += to_string(posMFT + i) + ", " + to_string(posMFT + i + 1);
                f.attribute = attr;
                MFTentries.push(f);
            }
        }
        cout << "\nCay thu muc goc: \n\n";
        print_RootFolder(MFTentries, 5, 0);
    }
    return 1;
}

ostream& operator << (ostream& out, FILE_INFO f) // phu tro xuat TapTin/ThuMuc
{
    
    if (f.attribute == 268435456) // thu muc
        out << f.name << " (sector: " << f.indexSector << ")\n";
    else
        out << f.name << " (sector: " << f.indexSector << " ; Kich thuoc: " << f.sizeData << " Byte)" << " => Hay mo tap tin bang ung dung tuong thich\n";
    return out;
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
                
                int i = temp.name.length();
                //xuat data neu file txt
                if (((int)temp.name[i - 2]) == 116 && ((int)temp.name[i - 4]) == 120 && ((int)temp.name[i - 6]) == 116) {
                    cout << setw(n_setw) << left << " " << "- ";
                    cout << temp.name << " (sector: " << temp.indexSector << " ; Kich thuoc: " << temp.sizeData << " Byte)\n";
                    cout << setw(n_setw) << left << "   ";
                    cout << "Noi dung: "; 
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << temp.data;
                    _setmode(_fileno(stdout), _O_TEXT);
                    cout << endl;
                    delete[] temp.data;
                }
                else {
                    cout << setw(n_setw) << left << " " << "- " << temp;
                }
            }
        }
        src.push(src.front());
        src.pop();
        k--;
    }
    
}

