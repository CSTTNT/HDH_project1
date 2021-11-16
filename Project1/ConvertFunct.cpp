#include "ConvertFunct.h"

int hex2dec(const char num[]) {
    int len = strlen(num);
    int base = 1;
    int temp = 0;
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] >= '0' && num[i] <= '9') {
            temp += (num[i] - 48) * base;
            base = base * 16;
        }
        else if (num[i] >= 'A' && num[i] <= 'F') {
            temp += (num[i] - 55) * base;
            base = base * 16;
        }
        else if (num[i] >= 'a' && num[i] <= 'f') {
            temp += (num[i] - 87) * base;
            base = base * 16;
        }
    }
    return temp;
}

int ReadBytes2Int(byte sector[], const char row[], const char col[], int num) {
    int col_dec = hex2dec(col);
    int row_dec = hex2dec(row);
    int dec = 0;
    int mu = 0;
    for (int i = col_dec; i <= col_dec + num - 1; i++) {
        if (sector[row_dec * 16 + i] >= 0 && sector[row_dec * 16 + i] <= 15) {
            int temp = (int)sector[row_dec * 16 + i];
            dec += temp * pow(16, mu);
        }
        else {
            dec += (int)sector[row_dec * 16 + i] * pow(16, mu);
        }
        mu += 2;
    }
    return dec;
}

void ReadBytes2Str(BYTE sector[], const char row[], const char col[], int num) {
    int col_dec = hex2dec(col);
    int row_dec = hex2dec(row);
    for (int i = col_dec; i < col_dec + num; i++) {
        cout << sector[row_dec * 16 + i];
    }
    cout << endl;
}

string Bytes2Str(BYTE sector[], int pos, int num) {
    string result = "";
    while (num > 0) {
        result += sector[pos];
        num--;
        pos++;
    }
    return result;
}

int Byte2Int(BYTE sector[], int pos, int num) {
    int dec = 0;
    int mu = 0;
    while (num > 0) {
        if (sector[pos] >= 0 && sector[pos] <= 15) {
            int temp = (int)sector[pos];
            dec += temp * pow(16, mu);
        }
        else {
            dec += (int)sector[pos] * pow(16, mu);
        }
        mu += 2;
        num--;
        pos++;
    }
    return dec;
}