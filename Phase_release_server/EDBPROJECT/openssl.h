#pragma once
#ifndef OPENSSL_H
#define OPENSSL_H

#include <openssl/aes.h>
#include <openssl/sha.h>
#include <string>
//#include "openssl/applink.c"

std::string Random_Keygen();
std::string AES_F(std::string Keystr, std::string str);
std::string SHA256(const std::string str);

#endif