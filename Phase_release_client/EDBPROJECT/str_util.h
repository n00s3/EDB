#pragma once
#ifndef STR_UTIL_H
#define STR_UTIL_H

#include <string>

void hex_to_byte(std::string str, unsigned char *ret);
std::string string_to_hex(const std::string& input);


#endif