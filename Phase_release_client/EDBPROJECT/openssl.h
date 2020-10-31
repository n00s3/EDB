#pragma once
//#ifndef OPENSSL_H
//#define OPENSSL_H

#include <openssl/aes.h>
#include <openssl/sha.h>
#include <string>
//#include "openssl/applink.c"

#define BLOCK_SIZE 16
#define FREAD_COUNT 4096
#define KEY_BIT 128
#define IV_SIZE 16
#define RW_SIZE 1
#define SUCC 0
#define FAIL -1


std::string Random_Keygen();
std::string AES_F(std::string Keystr, std::string str);
std::string AES_DEC(std::string Keystr, std::string str);
std::string SHA256(const std::string str);
int file_enc(std::string in, std::string out, std::string Keystr);
int file_dec(std::string in, std::string out, std::string Keystr);

//#endif