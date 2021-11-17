#pragma once
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <math.h>
#include <cstdlib>
#include <fstream>
#include <iomanip>
using namespace std;


string dec2hex(int n);

int hex2dec(const char num[]);

// Read bytes and convert 2 int
int ReadBytes2Int(byte sector[], const char row[], const char col[], int num);

// Read bytes and convert 2 string
string ReadBytes2Str(BYTE sector[], const char row[], const char col[], int num);

string Bytes2Str(BYTE sector[], int pos, int num);

int Byte2Int(BYTE sector[], int pos, int num);

//convert byte to unicode encoding utf-8
void Convert_String(BYTE* string_in, wchar_t* string_out, int scnt);