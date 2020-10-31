
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define PORT 7777
#define SERVER_IP "127.0.0.1"

#include "myheader.h"

#define DEBUG 0
#define SIZE 512

TSet_Modify EDB_[B][S] = {};
vector<mpz_class> XSet;



int main() {
	//����
	cout << "����\n";
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hListen;
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tListenAddr = {};
	tListenAddr.sin_family = AF_INET;
	tListenAddr.sin_port = htons(PORT);
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);
	SOCKET hClient = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);
	//����
	cout << "������\n";



	while (1) {
		char cmd[2]="";
		recv(hClient, cmd, 1, 0);

		if (cmd[0] == '1') {
			//TSet �ޱ�
			for (int n = 0; n < B; n++) {
				for (int m = 0; m < S; m++) {
					unsigned char str[512];
					unsigned char bit[LV + 1];
					unsigned char t_p[SIZE] = "";
					mpz_class temp;

					recv(hClient, (char*)t_p, SIZE, 0);

					for (int i = 0; i < LAMBDA; i++)
						EDB_[n][m].label[i] = t_p[i];
					for (int i = LAMBDA, j = 0; j < LV + 1; i++, j++)
						EDB_[n][m].value.eval[j] = t_p[i];
					for (int i = LAMBDA + LV + 1, j = 0; j < LV; i++, j++)
						bit[j] = t_p[i];
					string bit_(reinterpret_cast<char*>(bit));
					bit_ = bit_.substr(0, LV);
					bitset<16> b(bit_);
					int b_int = (int)(b.to_ulong());
					for (int i = (LAMBDA + LV + 1) + LV, j = 0; j < b_int; i++, j++)
						str[j] = t_p[i];
					string str_(reinterpret_cast<char*>(str));
					str_ = str_.substr(0, b_int);
					temp.set_str(str_, 10);
					EDB_[n][m].value.xval = temp;
				}
			}
			cout << "TSET ���� �Ϸ�\n";
			//TSet �ޱ� �Ϸ�
			//XSet �ޱ�
			//������ �ޱ�
			char buf[SIZE];
			recv(hClient, buf, SIZE, 0);
			int x_len = (int)buf[0];
			mpz_class x;
			for (int i = 0; i < x_len; i++) {
				recv(hClient, buf, SIZE, 0);
				x.set_str(buf, 10);
				//cout << x << endl;
				XSet.push_back(x);
			}
			cout << "XSET ����\n";
		}
		
		else if (cmd[0] == '2') {
			cout << "�˻�\n";


			unsigned char buf[SIZE];
			char buff[SIZE];

			recv(hClient, (char *)buf, SIZE, 0);	//����
			if (buf[0] == 100)
				continue;
			
			//stag �ޱ�
			recv(hClient, (char *)buf, SIZE, 0);
			string stag(reinterpret_cast<char*>(buf), LV);

			vector<mpz_class> *xtoken = new vector<mpz_class>[(int)buf[LV]+1];
			mpz_class x;
			for (int i = 0; i < (int)buf[LV]; i++) {
				for (int j = 0; j < (int)buf[LV + 1]-1; j++) {
					recv(hClient, buff, SIZE, 0);
					string xtok = buff;
					x.set_str(xtok, 10);
					xtoken[i].push_back(x);
					cout << "xtoken : " << x << endl;
				}
			}
			//xotoken �ޱ�
			//p �ޱ�
			mpz_class p;
			recv(hClient, buff, SIZE, 0);
			p.set_str(buff,10);

			//cout << "ctr �� : " << (int)buf[LV] << endl;
			//cout << "count �� : " << (int)buf[LV+1] << endl;
			vector<string> E = Server_Search(stag, xtoken, (int)buf[LV], p.get_mpz_t());
			cout << "���� �˻� �Ϸ�\n";
			cout << "���� ����\n";



			buff[0] = (unsigned char)E.size();
			send(hClient, buff, SIZE, 0);
			
			for (int i = 0; i < E.size(); i++) {
				cout << string_to_hex(E[i]) << endl;
				send(hClient, E[i].c_str(), SIZE, 0);
			}

			cout << "���� �˻� �Ϸ�\n";
		}
		else if (cmd[0] == '3') {
			break;
		}
	}
	
	closesocket(hClient);
	closesocket(hListen);
	WSACleanup();

	
	
	

#if DEBUG
#endif

	return 0;
}


vector<string> Server_Search(string stag, vector<mpz_class> xtoken[], int size, mpz_t p) {
	vector<string> E;
	vector<T_modify> V = TSetRetrieve_Modifications(stag);
	boolean b;
	

	cout << "Server_Search\n";
	//cout << "xtoken ũ�� : " << size << endl;
	//cout << "Retrieve ũ�� : " << V.size() << endl;

	mpz_class v;

	int count=0;
	for (int ctr = 0; ctr < size; ctr++) {
		b = true;

		for (int i = 0; i<xtoken[ctr].size(); i++) {
			bool flag = false;
			mpz_powm(v.get_mpz_t(), xtoken[ctr][i].get_mpz_t(), V[ctr].y.get_mpz_t(), p);
			cout << "xtoken ������ : " << v << endl;
			
			// if v not�� XSet then
			if (find(XSet.begin(), XSet.end(), v) != XSet.end()) {
				//cout << "��ġ�ϴ� �� ����\n";
				//cout << string_to_hex(V[ctr].w) << endl;
			}
			else {//���� ���ٸ�
				cout << "��ġ�ϴ� ���� xset�� ����\n";
				b = false;
				break;
			}
		}
		if (b)
			E.push_back(V[ctr].w);
	}

	return E;
}


///////////////////////////////////////////////////////////////////////////////////////////////
vector<T_modify> TSetRetrieve_Modifications(string stag) {	//SERVER
	vector<T_modify> V;
	T_modify temp;

	string tmp, H;
	string b, L, K;
	unsigned char Beta = 1;
	unsigned char num = 127;
	unsigned char b_bit;
	unsigned char L_bit[LAMBDA];
	unsigned char K_bit[LV + 1];
	unsigned char v[LV + 1];
	unsigned char f_id[LV];

	

	int i = 1;

	while (Beta && i < B) {
		tmp = to_string(i);
		string param = AES_F(stag, tmp);
		H = SHA256(param);
		H = H.substr(0, LAMBDA + LV + 1);


		b = H.substr(0, 1);
		b_bit = (0xFF & static_cast<unsigned char>(b[0]));
		b_bit = b_bit & num;
		cout << "H�� : " << string_to_hex(H) << "\t ���� : " << H.length() << endl;
		//cout << "\nb_bit �� (i�ε���) " << i <<" : " << (int)b_bit << endl;

		L = H.substr(0, LAMBDA);
		//cout << "L�� : " << string_to_hex(L) << "\T���� : " << L.length() << endl;
		for (int ii = 0; ii < LAMBDA; ii++)
			L_bit[ii] = (0xFF & static_cast<unsigned char>(L[ii]));

		K = H.substr(LAMBDA, LAMBDA + LV + 1);
		for (int ii = 0; ii < LV + 1; ii++)
			K_bit[ii] = (0xFF & static_cast<unsigned char>(K[ii]));

		cout << "b : " << string_to_hex(b) << endl;
		cout << "L : " << string_to_hex(L) << endl;
		cout << "K : " << string_to_hex(K) << endl;


		// ���� stag��� HASH ����
		bool flag;
		for (int j = 0; j < S; j++) {
			flag = 1;

			for (int k = 0; k < LAMBDA; k++) {
				if (EDB_[b_bit][j].label[k] != L_bit[k])
					flag = 0;
			}

			if (flag) {
				cout << "ã�ҽ��ϴ�\n";

				for (int k = 0; k < LV + 1; k++)
					v[k] = EDB_[b_bit][j].value.eval[k] ^ K_bit[k];
				Beta = v[16];

				for (int k = 0; k < LV; k++)
					f_id[k] = v[k];
				string tmp(reinterpret_cast<char*>(f_id), LV);
				temp.w = tmp.substr(0, LV);
				temp.y = EDB_[b_bit][j].value.xval;
				V.push_back(temp);
			}
		}
		i++;
	}

	if (i != B) {
		//cout << "<��� ���>\n";
		for (int i = 0; i < V.size(); i++)
			cout << string_to_hex(V[i].w) << endl;
		return V;
	}
	else
		cout << "\n\n<�˻� ����� �����ϴ�.>\n";
}









