#include "openssl.h"
#include <sstream>
#include <time.h>

using namespace std;

#define LV	16
#define B	128
#define LAMBDA 16

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

string AES_DEC(string Keystr, string str) {
	AES_KEY aes_key;
	unsigned char Key[LV] = {};
	for (int i = 0; i < LV; i++)
		Key[i] = Keystr[i];

	AES_set_decrypt_key(Key, 128, &aes_key);

	unsigned char strArr[B] = "";
	unsigned char text[B] = "";
	for (int i = 0; i < str.length(); i++)
		strArr[i] = str[i];

	AES_decrypt(strArr, text, &aes_key);
	stringstream ss;
	for (int i = 0; i < LV; i++)
		ss << hex << text[i];

	return ss.str();
}

string Random_Keygen() {	//16byte key
	srand((unsigned int)time(NULL));
	int random;

	unsigned char ret[LV] = {};
	for (int i = 0; i < LV; i++) {
		random = (rand() % 255) +1;
		ret[i] = random;
	}
	string str(reinterpret_cast<char*>(ret), LV);
	
	return str.substr(0, LV);
}




int file_enc(string in, string out, string Keystr) {
	char in_file[256], out_file[256];
	strcpy(in_file, in.c_str());
	strcpy(out_file, out.c_str());


	unsigned char iv[IV_SIZE];
	AES_KEY aes_key;

	int i = 0;
	int len = 0;
	int padding_len = 0;
	unsigned char buf[FREAD_COUNT + BLOCK_SIZE];

	FILE *fp = fopen(in_file, "rb");
	if (fp == NULL) 
		return FAIL;

	FILE *wfp = fopen(out_file, "wb");
	if (wfp == NULL)
		return FAIL;

	memset(iv, 0, sizeof(iv)); // init iv

	unsigned char Key[LAMBDA] = {};	//LAMBDA
	for (int i = 0; i < LAMBDA; i++)//LAMBDA
		Key[i] = Keystr[i];

	AES_set_encrypt_key(Key, KEY_BIT, &aes_key);
	while (len = fread(buf, RW_SIZE, FREAD_COUNT, fp)) {
		if (FREAD_COUNT != len)
			break;

		AES_cbc_encrypt(buf, buf, len, &aes_key, iv, AES_ENCRYPT);
		fwrite(buf, RW_SIZE, len, wfp);
	}


	padding_len = BLOCK_SIZE - len % BLOCK_SIZE;
	memset(buf + len, padding_len, padding_len);

	AES_cbc_encrypt(buf, buf, len + padding_len, &aes_key, iv, AES_ENCRYPT);
	fwrite(buf, RW_SIZE, len + padding_len, wfp);

	fclose(wfp);
	fclose(fp);

	return SUCC;
}


int file_dec(string in, string out, string Keystr) {
	char in_file[256], out_file[256];
	strcpy(in_file, in.c_str());
	strcpy(out_file, out.c_str());

	unsigned char iv[IV_SIZE];
	AES_KEY aes_key;

	unsigned char buf[FREAD_COUNT + BLOCK_SIZE];
	int len = 0;
	int total_size = 0;
	int save_len = 0;
	int w_len = 0;

	FILE *fp = fopen(in_file, "rb");
	if (fp == NULL)
		return FAIL;

	FILE *wfp = fopen(out_file, "wb");
	if (wfp == NULL)
		return FAIL;

	memset(iv, 0, sizeof(iv)); // the same iv
	unsigned char Key[LAMBDA] = {};	//LAMBDA
	for (int i = 0; i < LAMBDA; i++)//LAMBDA
		Key[i] = Keystr[i];

	AES_set_decrypt_key(Key, KEY_BIT, &aes_key);

	fseek(fp, 0, SEEK_END);
	total_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	while (len = fread(buf, RW_SIZE, FREAD_COUNT, fp)) {
		if (FREAD_COUNT == 0) {
			break;
		}
		save_len += len;
		w_len = len;

		AES_cbc_encrypt(buf, buf, len, &aes_key, iv, AES_DECRYPT);
		if (save_len == total_size)
			w_len = len - buf[len - 1];

		fwrite(buf, RW_SIZE, w_len, wfp);
	}

	fclose(wfp);
	fclose(fp);

	return SUCC;
}