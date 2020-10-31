#include "str_util.h"

std::string string_to_hex(const std::string& input) {
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}

void hex_to_byte(std::string str, unsigned char *ret) {
	std::string hex = "0123456789ABCDEF";
	int number[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };

	int cnt = 0;
	//unsigned char *ret = new unsigned char[str.length()/2];
	for (int i = 0; i < str.length(); i += 2) {
		ret[cnt] = number[hex.find(str[i])] * 16;
		ret[cnt] += number[hex.find(str[i + 1])];
		cnt++;
	}
}
