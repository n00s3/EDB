#pragma once
#include "str_util.h"
#include "openssl.h"
#include <gmp_util.h>

#include <iostream>
#include <fstream>
#include <utility>				//pair
#include <vector>
#include <map>
#include <algorithm>
#include <bitset>
#include <iomanip>
#include <stdlib.h>
#include <Winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32")

using namespace std;

#define B					128
#define S					128
#define LAMBDA				10		// 80  bit
#define LV					16		// 128 bit
#define KAPPA				128		// 128 bit

#define NUMBER_OF_TRIALS    30
#define BYTE_WIDTH          8       // one byte = 8 bits
#define BYTE_BITLENGTH      8
#define EQUAL				0
#define MSB_FIRST           1       // most significant word first

typedef struct Value {
	unsigned char eval[LV + 1];
	mpz_class xval;
}Value;

typedef struct TSet_Modify {
	unsigned char label[LAMBDA];
	Value value;
}TSet_Modify;

typedef struct T_modify {
	string		w;
	mpz_class	y;
}T_modify;

vector<string> Server_Search(string stag, vector<mpz_class> xtoken[], int size, mpz_t p);



vector<T_modify> TSetRetrieve_Modifications(string stag);

