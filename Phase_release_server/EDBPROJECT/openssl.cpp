#include "openssl.h"
#include <sstream>
#include <time.h>

using namespace std;

#define LV	16
#define B	128

string SHA256(const string str)
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);
	stringstream ss;

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
		ss << hex << hash[i];

	return ss.str();
}


string AES_F(string Keystr, string str) {
	AES_KEY aes_key;
	unsigned char Key[LV] = {};
	for (int i = 0; i < LV; i++)
		Key[i] = Keystr[i];

	AES_set_encrypt_key(Key, 128, &aes_key);

	unsigned char strArr[B] = { };
	unsigned char ciphertext[B] = { };
	for (int i = 0; i < str.length(); i++)
		strArr[i] = str[i];

	AES_encrypt(strArr, ciphertext, &aes_key);		//ciphertext¿¡ ÀúÀå
	stringstream ss;

	for (int i = 0; i < LV; i++)
		ss << hex << ciphertext[i];

	return ss.str();
}


string Random_Keygen() {	//16byte key
	srand((unsigned int)time(NULL));
	int random;

	//while (1) {
		unsigned char ret[LV] = {};
		for (int i = 0; i < LV; i++) {
			random = (rand() % 255) +1;
			ret[i] = random;
		}
		string str(reinterpret_cast<char*>(ret), LV);
		//if (str.substr(0, LV).length() == LV)
			return str.substr(0, LV);
	//}
	//return "abcdefghijklmno";
}