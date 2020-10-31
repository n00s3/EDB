
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define PORT 7777
#define SERVER_IP "127.0.0.1"

#include "myheader.h"

#define DEBUG 1
#define SIZE 512

TSet_Modify EDB_[B][S] = {};
vector<mpz_class> XSet;


void read_directory(const string &name, vector<string> &v) {
	string pattern(name);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;

	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			v.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}

vector<string> tokenize_getline(const string& data, const char delimiter = ' ') {
	vector<string> result;
	string token;
	stringstream ss(data);

	while (getline(ss, token, delimiter))
		result.push_back(token);

	return result;
}



int main() {
	srand(time(NULL));

	//소켓 연결
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hSocket;
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));


	string KeyS, KeyT;
	vector<mpz_class> Kd, Kx, Kz;
	vector<string> State[B];
	mpz_t p, g, q;

	string cmd;
	bool flag = false;
	while (1) {
		cout << "=====\n";
		cout << "1. 저장\n";
		cout << "2. 검색\n";
		cout << "3. 종료\n";
		cout << "=====\n >> ";
		cin >> cmd;


		if (cmd.length() != 1 || cmd[1]!='\0') {
			cout << "잘못 입력했습니다.\n";
			continue;
		}


		//send to server cmd;
		send(hSocket, cmd.c_str(), 1, 0);

		
		//저장
		if (cmd[0] == '1') {
			EDBSetup_Modifications();
			
			//TSet 패킷 생성
			for (int n = 0; n < B; n++) {
				for (int m = 0; m < S; m++) {
					unsigned char t_p[SIZE] = "";
					string str;
					string bit;
					bitset<16> b;

					for (int i = 0; i < LAMBDA; i++)
						t_p[i] = EDB_[n][m].label[i];
					for (int i = LAMBDA, j = 0; j < LV + 1; i++, j++)
						t_p[i] = EDB_[n][m].value.eval[j];
					str=EDB_[n][m].value.xval.get_str(10);
					b = str.length();	//xval 사이즈	16비트
					bit = b.to_string();	//xval 사이즈 문자열 ==> 16바이트 문자열
					for (int i = LAMBDA + LV + 1, j = 0; j < LV; i++, j++)
						t_p[i] = bit[j];
					for (int i = (LAMBDA + LV + 1) + LV, j = 0; j < str.length(); i++, j++)
						t_p[i] = str[j];
					int p_size = strlen((char*)t_p);


					bitset<16> b_(bit);
					int b_int = (int)(b_.to_ulong());
					send(hSocket, (char *)t_p, SIZE, 0);
				}
			}
			//TSet 전송


			//XSet 전송
			char buf[SIZE];
			int x_len = XSet.size();
			buf[0] = x_len;
			send(hSocket, buf, SIZE, 0);
			string x;
			//XSet 개수 , for(XSet 길이, 문자열)
			for (int i = 0; i < XSet.size(); i++) {
				x = XSet[i].get_str(10);
				send(hSocket, x.c_str(), SIZE, 0);
			}

		}

		else if (cmd[0] == '2') {
			if (!flag) {
				//KeyS, KeyT load begin
				unsigned char KS[LV];
				unsigned char KT[LV];

				ifstream input_s("KeyS.bin", ios::in | ios::binary);
				input_s >> KeyS;
				hex_to_byte(KeyS, KS);
				string str(reinterpret_cast<char*>(KS));
				KeyS = str.substr(0, LV);
				input_s.close();

				ifstream input_t("KeyT.bin", ios::in | ios::binary);
				input_t >> KeyT;
				hex_to_byte(KeyT, KT);
				string str_(reinterpret_cast<char*>(KT));
				KeyT = str_.substr(0, LV);
				input_t.close();

				cout << "KeyS : " << string_to_hex(KeyS) << endl;
				cout << "KeyT : " << string_to_hex(KeyT) << endl;
				//KeyS, KeyT load end

				cout << "\n\n";

				//Kd, Kx, Kz load begin
				FILE *load_Kd, *load_Kx, *load_Kz;

				mpz_t load_mpz_kd;
				mpz_t load_mpz_kx;
				mpz_t load_mpz_kz;
				mpz_class mpz_kd, mpz_kx, mpz_kz;
				mpz_init(load_mpz_kd);
				mpz_init(load_mpz_kx);
				mpz_init(load_mpz_kz);

				cout << "Kd 불러오는 중..\n";
				load_Kd = fopen("KeyD.bin", "rb");
				for (int i = 0; i < LV * 8; i++) {
					_readMpzFromFile(load_Kd, load_mpz_kd);
					mpz_init_set(mpz_kd.get_mpz_t(), load_mpz_kd);
					Kd.push_back(mpz_kd);
				}
				fclose(load_Kd);

				cout << "Kx 불러오는 중..\n";
				load_Kx = fopen("KeyX.bin", "rb");
				for (int i = 0; i < LV * 4; i++) {
					_readMpzFromFile(load_Kx, load_mpz_kx);
					mpz_init_set(mpz_kx.get_mpz_t(), load_mpz_kx);
					Kx.push_back(mpz_kx);
				}
				fclose(load_Kx);

				cout << "Kz 불러오는 중..\n";
				load_Kz = fopen("KeyZ.bin", "rb");
				for (int i = 0; i < LV * 4; i++) {
					_readMpzFromFile(load_Kz, load_mpz_kz);
					mpz_init_set(mpz_kz.get_mpz_t(), load_mpz_kz);
					Kz.push_back(mpz_kz);
				}
				fclose(load_Kz);

				cout << "Kd, Kx, Kz 불러오기 완료\n";
				cout << "\n\n";

				//State load begin
				int i_index = 0, i = 0;
				fstream dataFile;
				string buffer;
				cout << "State 파일 불러오기" << endl;
				dataFile.open("State.dat", ios::in);
				while (!dataFile.eof()) {
					getline(dataFile, buffer);
					if (i_index % 2 == 0) {		//키워드일 경우
						State[i].push_back(buffer);
						cout << "키워드 명 " << buffer << endl;
					}
					else {						//파일명일 경우 슬라이스
						for (int cnt = 0; cnt < buffer.length() / 16; cnt++) {
							string temp = buffer.substr(cnt * 16, (cnt + 1) * 16);
							State[i].push_back(temp.substr(0, LV));
							cout << "파일명 : " << temp.substr(0, LV) << endl;
						}
						i++;
					}
					i_index++;
				}
				dataFile.close();
				//State load end

				//P, G load beign
				FILE *rd_pg;
				mpz_init(p);	mpz_init(g);	mpz_init(q);
				if ((rd_pg = fopen("PG.bin", "rb")) == NULL)
					printf("error! \n");
				_readMpzFromFile(rd_pg, p);
				_readMpzFromFile(rd_pg, g);
				_readMpzFromFile(rd_pg, q);
				fclose(rd_pg);
				cout << "P : " << p << endl;
				cout << "G : " << g << endl;
				cout << "Q : " << q << endl;
				//P, G load end
			}
			flag = true;

			//검색
			vector<string> w;
			string keyword;
			int ii = 0;
			while (1) {
				cout << "["<<ii+1<<"] 검색할 키워드 입력(!입력시 종료) : ";
				cin >> keyword;
				if (!keyword.compare("!"))
					break;

				w.push_back(keyword);
				ii++;
			}
			Client_Search(KeyS, KeyT, Kd, Kz, Kx, State, w, p, g, hSocket);
			//send to server w, xtoken, p
		}

		else if (cmd[0] == '3') {
			break;
		}
	}

	closesocket(hSocket);
	WSACleanup();



	


#if DEBUG
#endif


	return 0;
}




string TSetSetup_Modifications(T_modify (*T_)[S], vector<string> W) {
	int size, random;
	string KeyT;
	string stag;

	//임시KeyT 고정 할당
	//srand(time(NULL));
	KeyT = Random_Keygen();
	unsigned char num = 127;
	unsigned char b_bit;
	unsigned char L_bit[LAMBDA];
	unsigned char K_bit[LV + 1];
	unsigned char Beta;
	string b, L, K, H;

	bool Free[B][S] = { 0 };

	cout << "KeyT : " << string_to_hex(KeyT) << endl;
	size = W.size();
	for (int i = 0; i < size; i++) {
		//stag 생성
		stag = AES_F(KeyT, W[i]);

		cout << W[i] <<"<stag> : " << string_to_hex(stag) << "\tlen : " << stag.length();

		// V={id1, id2 ...}
		int cnt = 0;
		for (int j = 1; j < S; j++) {
			if (T_[i][j].w.compare(""))
				cnt++;
			else
				break;
		}

		for (int j = 1; j <= cnt; j++) {
			//hash 256 생성
			string tmp = to_string(j);
			H = SHA256(AES_F(stag, tmp));		//stag(W, Kt) 값과 암호화된 Fileid
			//hash 216 ==> b | L | K
			H = H.substr(0, LAMBDA + LV + 1);
			cout << "\n" << W[i] << " 216BIT 슬라이싱 " << " HASH : " << string_to_hex(H) << "   len : " << H.length() << endl;

			b = H.substr(0, 1);
			b_bit = (0xFF & static_cast<unsigned char>(b[0]));
			cout << "b : 8bit : " << bitset<8>(b_bit) << " ==>  : 7bit : " << bitset<8>(b_bit&num) << "\n";
			b_bit = b_bit & num;

			L = H.substr(0, LAMBDA);
			for (int i = 0; i < LAMBDA; i++)
				L_bit[i] = (0xFF & static_cast<unsigned char>(L[i]));

			K = H.substr(LAMBDA, LAMBDA + LV + 1);
			for (int i = 0; i < LV + 1; i++)
				K_bit[i] = (0xFF & static_cast<unsigned char>(K[i]));


			cout << "\nb : " << string_to_hex(b) << "   len : " << b.length() << endl;
			cout << "L : " << string_to_hex(L) << "   len : " << L.length() << endl;
			cout << "K : " << string_to_hex(K) << "   len : " << K.length() << endl;

			//Free 에서 위치 지정
			random = rand() % S;
			while (Free[b_bit][random]) { // B=b
				random = rand() % S;
				Free[b_bit][random] = 1;
			}

			//베타값 지정
			if (j < cnt)
				Beta = 1;
			else if (j == cnt)
				Beta = 0;



			//TSet[b, j].label ← L
			for (int i = 0; i < LAMBDA; i++)
				EDB_[b_bit][random].label[i] = (0xFF & static_cast<unsigned char>(L[i]));
			//TSet[b, j].value ← (beta | fileid) XOR K
			for (int k = 0; k < LV; k++)
				EDB_[b_bit][random].value.eval[k] = (0xFF & static_cast<unsigned char>(T_[i][j].w[k]));
			EDB_[b_bit][random].value.eval[16] = Beta;
			for (int k = 0; k < LV + 1; k++)
				EDB_[b_bit][random].value.eval[k] = EDB_[b_bit][random].value.eval[k] ^ K_bit[k];

			EDB_[b_bit][random].value.xval = T_[i][j].y;

			cout << "TSet[" << (unsigned int)b_bit << "]" << "[" << random << "] 지정 완료\n";
		}
		cout << "\n";
	}


	return KeyT;
}




void EDBSetup_Modifications() {
	vector<string> dir;
	vector<string> ddir;
	read_directory("./files", dir);
	
	for (int i = 0; i < dir.size(); i++) {
		if (dir[i].length() > 3) {
			ddir.push_back(dir[i].substr(0, LV));
		}
	}

	
	//파일 갯수(임의 지정)
	string keyword;
	mpz_t p, g, q;
	mpz_init(p);	mpz_init(g);	mpz_init(q);
	vector<mpz_class> Kd, Kx, Kz;
	
	//KeyS 할당
	string KeyS = Random_Keygen();
	//P, G 생성
	//OPRFSetup();
	Sleep(100);


	//P, G load
	FILE *rd_pg;
	if ((rd_pg = fopen("PG.bin", "rb")) == NULL)
		printf("error! \n");
	_readMpzFromFile(rd_pg, p);
	_readMpzFromFile(rd_pg, g);
	_readMpzFromFile(rd_pg, q);
	fclose(rd_pg);
	cout << "P, G 로드 완료\n\n";


	//Kd, Kx, Kz 생성 후 저장
	FILE *KeyD, *KeyX, *KeyZ;
	KeyD = fopen("KeyD.bin", "wb");
	Kd = OPRFKeyGen(KeyD, KAPPA, LV * 8, 1);
	fclose(KeyD);
	
	KeyX = fopen("KeyX.bin", "wb");
	Kx = OPRFKeyGen(KeyX, KAPPA, LV * 4, 2);
	fclose(KeyX);
	
	KeyZ = fopen("KeyZ.bin", "wb");
	Kz = OPRFKeyGen(KeyZ, KAPPA, LV * 4, 3);
	fclose(KeyZ);
	Sleep(100);
	

		

////////////////
	//vector<mpz_class> Kd, Kx, Kz;
	cout << "Kd, Kx, Kz 불러오는 중..\n";
	FILE *load_Kd, *load_Kx, *load_Kz;

	mpz_t load_mpz_kd;
	mpz_t load_mpz_kx;
	mpz_t load_mpz_kz;
	mpz_class mpz_kd, mpz_kx, mpz_kz;
	mpz_init(load_mpz_kd);
	mpz_init(load_mpz_kx);
	mpz_init(load_mpz_kz);

	cout << "Kd 불러오는 중..\n";
	load_Kd = fopen("KeyD.bin", "rb");
	for (int i = 0; i < LV * 8; i++) {
		_readMpzFromFile(load_Kd, load_mpz_kd);
		mpz_init_set(mpz_kd.get_mpz_t(), load_mpz_kd);
		Kd.push_back(mpz_kd);
	}
	fclose(load_Kd);

	cout << "Kx 불러오는 중..\n";
	load_Kx = fopen("KeyX.bin", "rb");
	for (int i = 0; i < LV * 4; i++) {
		_readMpzFromFile(load_Kx, load_mpz_kx);
		mpz_init_set(mpz_kx.get_mpz_t(), load_mpz_kx);
		Kx.push_back(mpz_kx);
	}
	fclose(load_Kx);

	cout << "Kz 불러오는 중..\n";
	load_Kz = fopen("KeyZ.bin", "rb");
	for (int i = 0; i < LV * 4; i++) {
		_readMpzFromFile(load_Kz, load_mpz_kz);
		mpz_init_set(mpz_kz.get_mpz_t(), load_mpz_kz);
		Kz.push_back(mpz_kz);
	}
	fclose(load_Kz);
///////////////////



	int size;
	vector<string> W;
	multimap<string, string> mm;

	
	string k_name;
	
	for (int i = 0; i < ddir.size(); i++) {
		cout << ddir[i] << " 키워드 입력 : ";
		cin >> k_name;

		vector<string> result = tokenize_getline(k_name, ',');
		for (int j = 0; j < result.size(); j++) {
			mm.insert(pair<string, string>(result[j], ddir[i]));
			W.push_back(result[j]);
		}
	}

	//키워드 중복 제거
	sort(W.begin(), W.end());
	W.erase(unique(W.begin(), W.end()), W.end());
	size = W.size();

	
	//전체 출력
	multimap<string, string>::iterator iter;
	for (iter = mm.begin(); iter != mm.end(); iter++)
		cout << iter->first << ", " << iter->second << endl;

	//T 임시 출력
	cout << "\n---modification keyword list, W ---" << endl;

	string key;
	for (int i = 0; i < size; i++) {
		cout << "===" << W.at(i) << "===\n";
		key = W.at(i);

		for (iter = mm.begin(); iter != mm.end(); iter++)
			if (!(iter->first.compare(key)))
				cout << iter->second << "\tlen : " << iter->second.length() << "\n";

		cout << endl;
	}
	


	// T - 키워드 크기만큼 할당 
	vector<vector<string> > T(size, vector<string>(S, "\0"));
	T_modify T_[B][S];
	vector<string> State[B];
	for (int i = 0; i < size; i++) {
		int j = 0;
		State[i].push_back(W.at(i));
		T[i][0] = W.at(i);
		T_[i][0].w = W.at(i);

		//Build a list Dw of file ID's that contain w
		for (iter = mm.begin(); iter != mm.end(); iter++) {
			if (!(iter->first.compare(T[i][0]))) {
				T[i][j + 1] = iter->second;
				j++;
			}
		}

		// 키 KeyE 생성 = AES(KeyS, T[i][0])
		string KeyE = AES_F(KeyS, T[i][0]);
		cout << "KeyE : " << string_to_hex(KeyE) << "\tlen : " << KeyE.length() << endl;
		

		//KeyE로 각 file id 암호화
		string org_file_id, org_keyword=T[i][0];
		mpz_class xid, z, xtag;
		for (int ctr = 0; ctr < j; ctr++) {
			org_keyword = W.at(i);
			org_file_id = T[i][1 + ctr];

			file_enc("./files/" + T[i][1 + ctr] + ".txt", "./enc/" + T[i][1 + ctr] + ".enc", KeyS);

			State[i].push_back(org_file_id);
			//cout << "original file id : " << org_file_id << endl;
			xid = G_op(Kd, org_file_id, p, g);
			
			//cout << org_file_id << "-org_file_id의 xid : " <<xid<< endl;
			unsigned char z_ = (unsigned char)W.at(i)[0] ^ ctr;
			org_keyword[0] = (char)z_;
			z = G_op(Kz, org_keyword, p, g); //G(kZ , w ^ ctr)
			cout << "z 값 = zctr: " << z << endl << endl;
			cout << "CTR키워드 : " << org_keyword << "-"<<ctr<<  endl;

			

			mpz_class y;
			mpz_invert(y.get_mpz_t(), z.get_mpz_t(), q); // y=z^(-1)
			y = xid * y;
			mpz_mod(y.get_mpz_t(), y.get_mpz_t(), q);

			T_[i][1 + ctr].w = AES_F(KeyE, T[i][1 + ctr]);	
			T_[i][1 + ctr].y = y;							// T[i, ctr + 1] <- (e, y)

			xtag = G_op(Kx, T[i][0], p, g);
			xtag = xtag * xid;
			mpz_powm(xtag.get_mpz_t(), g, xtag.get_mpz_t(), p);

			XSet.push_back(xtag);
			//cout << xtag << "\tXSET 추가\n";
		}


	}//T 세팅 완료





	cout << "생성된 XSet 출력\n";
	for (int i = 0; i < XSet.size(); i++)
		cout << '[' << i+1 << "]\t" << XSet[i] << endl;

	//State 정렬
	vector<string> temp;
	for (int i = size-1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (State[j].size() > State[j + 1].size()) {
				temp = State[j];
				State[j] = State[j + 1];
				State[j + 1] = temp;
			}
		}
	}


	//State 저장
	fstream dataFile;
	dataFile.open("State.dat", ios::out);
	for (int i = 0; i < size; i++) {
		dataFile.write(State[i][0].c_str(), State[i][0].size());
		dataFile.write("\n", 1);
		for (int j = 1; j < State[i].size(); j++) {
			dataFile.write(State[i][j].c_str(), LV);
		}
		if(i!=size-1)
			dataFile.write("\n", 1);
	}
	dataFile.close();



	fstream dataFile1;
	char temp_buf[10]="";
	dataFile1.open("State1.dat", ios::out);
	for (int i = 0; i < size; i++) {
		dataFile1.write(State[i][0].c_str(), State[i][0].size());
		dataFile1.write("\n", 1);
		//for (int j = 1; j < State[i].size(); j++) {
		//	dataFile1.write(State[i][j].c_str(), LV);
		//}
		_itoa(State[i].size()-1, temp_buf, 10);
		dataFile1.write(temp_buf, 10);
		if (i != size - 1)
			dataFile1.write("\n", 1);
	}
	dataFile1.close();

	
	

	
	//저장된 T_ 출력
	size = T.size();
	cout << "\n--- T ---\n";
	for (int i = 0; i < size; i++) {
		cout << "===" << T_[i][0].w << "===" << endl;
		for (int j = 1; j < S; j++) {
			if (T_[i][j].w != "")
				cout << string_to_hex(T_[i][j].w) << "\tlen : " << T_[i][j].w.length() << "\n";
		}
		cout << endl;
	}
	

	//TSetSetup에 T, W 인자 전달
	string KeyT = TSetSetup_Modifications(T_, W);


	//KeyS와 KeyT 파일 저장
	ofstream output_s("KeyS.bin", ios::out | ios::binary);
	KeyS = string_to_hex(KeyS);
	output_s << KeyS;
	output_s.close();
	ofstream output_t("KeyT.bin", ios::out | ios::binary);
	KeyT = string_to_hex(KeyT);
	output_t << KeyT;
	output_t.close();


	cout << "KeyS 저장 : " << KeyS << endl;
	cout << "KeyT 저장 : " << KeyT << endl;
	//TSet_, XSet 서버에 전송 sock

	cout << "\n\n\nEDB 셋 완료\n\n";
}


//Client's OXT Search
void Client_Search(string KeyS, string KeyT, 
	vector<mpz_class> Kd, vector<mpz_class> Kz, vector<mpz_class> Kx, 
	vector<string> State[], vector<string> w, mpz_t p, mpz_t g, SOCKET hSocket) {
	
	vector<string> State_w[B];
	string keyword;
	
	//State 개수
	int cnt = 0;
	for (int i = 0; i < B; i++) {
		if (State[i].size() != 0)
			cnt++;
	}
	cout << "state개수 : " << cnt <<endl;


	int count = 0;
	for (int i = 0; i < cnt; i++) {
		for (int j = 0; j < w.size(); j++) {
			if (!State[i][0].compare(w[j])) {
				State_w[count] = State[i];
				count++;
				break;
			}
		}
	}


	unsigned char code[SIZE];
	if (count != w.size()) {
		code[0] = 100;
		cout << "키워드 없음\n";
		send(hSocket, (char*)code, SIZE, 0);
		return;
	}
	else
		send(hSocket, (char*)code, SIZE, 0);





//	for (int i = 0; i < w.size(); i++)
//		cout << "State 키워드 출력 :" << State_w[i][0] << endl;
	
	//검색키 생성
	string stag = TSetGetTag(KeyT, State_w[0][0]);
	
	cout << "State 출력" << endl;
	for (int i = 0; i < count; i++) {
		cout << State_w[i][0] << endl;
		cout << "index : " << i << endl;
		for (int j = 1; j < State_w[i].size(); j++)
			cout << State_w[i][j] << " ";
		cout << endl;
	}
	keyword = State_w[0][0];

	//p,g load
	//cout << "xtoken 배열 크기 : " << State_w[0].size() << endl; 
	vector<mpz_class> *xtoken = new vector<mpz_class>[State_w[0].size()];
	
	cout << "xtoken 생성중..\n";
	int ctr;
	mpz_class tmp;
	////////////////////////////////////////////////////////////////
	string bak = State_w[0][0];
	for (ctr = 0; ctr < State_w[0].size() - 1; ctr++) {
		State_w[0][0] = bak;
		unsigned char one = (unsigned char)State_w[0][0][0] ^ ctr;
		State_w[0][0][0] = (char)one;
		cout << "CTR키워드 : " << State_w[0][0] << " - " << ctr << endl;
		mpz_class z_ctr = G_op(Kz, State_w[0][0], p, g);
		//cout << "z_Ctr : " << z_ctr << "\n\n";


		for (int i = 1; i < count; i++) {
			cout << "xtoken을 " << State_w[i][0] << "로 생성\n";
			tmp = z_ctr * G_op(Kx, State_w[i][0], p, g);
			mpz_powm(tmp.get_mpz_t(), g, tmp.get_mpz_t(), p);
			xtoken[ctr].push_back(tmp);
			cout << "xtoken 생성완료\n";
		}
	}


	cout << "stag, xtoken, p 전송\n";
	//send (stag, xtoken) to server
	unsigned char buf[SIZE];
	for (int i = 0; i < LV; i++)
		buf[i] = stag[i];
	buf[LV] = State_w[0].size()-1;	buf[LV+1] = count;
	send(hSocket, (char*)buf, SIZE, 0);
	for (int i = 0; i < State_w[0].size() - 1; i++) {
		for (int j = 0; j < count-1; j++) {
			string xtok = xtoken[i][j].get_str();
			cout << "xtoken : " << xtoken[i][j] << endl;
			send(hSocket, xtok.c_str(), SIZE, 0);
		}
	}
	//cout << xtoken[0][0] << "전송\n";
	//p전송
	mpz_class pp(p);
	send(hSocket, pp.get_str().c_str(), SIZE, 0);
	//cout << "ctr값 : " << State_w[0].size() - 1 << "=" << ctr-1<< endl;
	//cout << "count값 : " << count << endl;

	//free(xtoken);
	cout << "서버 검색 완료\n";



	recv(hSocket, (char*)buf, SIZE, 0);
	vector<string> EE;

	int len = (int)buf[0];
	if (len == 0) {
		cout << "검색 결과 없음\n";
		return;
	}


	for (int i = 0; i < len; i++) {
		recv(hSocket, (char*)buf, SIZE, 0);
		string str(reinterpret_cast<char*>(buf), LV);
		EE.push_back(str);
		cout << string_to_hex(str) << endl;
	}

	
	cout << "KeyE 생성\n";
	string KeyE = AES_F(KeyS, keyword);
	
	vector<string> Dw;
	for (int i = 0; i < EE.size(); i++) {
		cout << "복호화된 복구 파일 : " << AES_DEC(KeyE, EE[i]) << endl;
		Dw.push_back(AES_DEC(KeyE, EE[i]));
	}
}









//run by the user
string TSetGetTag(string KeyT, string w) {
	string stag = AES_F(KeyT, w);

	return stag;
}



void OPRFSetup() {
	//big_int ret;
	const uint32_t  lambda = 1024;
	gmp_randstate_t rstate;
	uint64_t        seed = (unsigned long)time(0);

	/// randomizer by a Mersenne Twister algorithm
	gmp_randinit_mt(rstate);
	gmp_randseed_ui(rstate, seed);

	mpz_t           p, g, y, x, p_minus_1, q, zero;
	mpz_t           t1, t2, t3;
	FILE *fp_pg;

	mpz_init(zero); mpz_set_ui(zero, 0);
	mpz_init(p); mpz_set_ui(p, 0);
	mpz_init(g); mpz_set_ui(g, 0);
	mpz_init(y); mpz_set_ui(y, 0);
	mpz_init(x); mpz_set_ui(x, 0);
	mpz_init(p_minus_1); mpz_set_ui(p_minus_1, 0);
	mpz_init(q); mpz_set_ui(q, 0);                      /// \f$p-1=2q\f$ for some primes \f$p\f$ and \f$q\f$

	cout << "p, g 생성 중..\n";
	/// find the large prime $p$
	mpz_urandomb(q, rstate, lambda - 1);     /// random \f$\lambda-1\f$-bit integer

	while (1) {
		if (1 == mpz_probab_prime_p(q, NUMBER_OF_TRIALS)) { /// surely or probably prime
			mpz_mul_ui(p, q, 2ul);      /// \f$p=2q\f$
			mpz_add_ui(p, p, 1ul);      /// \f$p=2q+1\f$
			if (1 == mpz_probab_prime_p(p, NUMBER_OF_TRIALS))
				break;
		}
		mpz_urandomb(q, rstate, lambda - 1);
	}
	mpz_sub_ui(p_minus_1, p, 1ul);

	cout << "p : " << p << endl;

	//gen g
	mpz_init(t1); mpz_init(t2); mpz_init(t3);	
	mpz_urandomb(g, rstate, lambda);
	while (1) {
		//mpz_powm(t1, g, q, p);                  //! \f$ t1=g^{q} \pmod{p} \f$
		mpz_powm_ui(t2, g, 2ul, p);             //! \f$t_2=g^{2} \pmod{p} \f$
		mpz_powm(t3, g, q, p);
		mpz_class a(t2);
		mpz_class b(t3);
		//mpz_powm(t3, g, p_minus_1, p);          //! \f$t_3=g^{p-1} \pmod{p} \f$
		//if ((EQUAL != mpz_cmp_ui(t1, 1ul))) { //&& 
			//(EQUAL != mpz_cmp_ui(t2, 1ul)) &&
			//(EQUAL == mpz_cmp_ui(t3, 1ul)))
		if(a!=1 && b==1)
			break;
		//}
		mpz_urandomb(g, rstate, lambda);
	}
	cout << "g : " << g << endl;

	fp_pg = fopen("PG.bin", "wb");
	_writeMpzToFile(fp_pg, p);
	_writeMpzToFile(fp_pg, g);
	_writeMpzToFile(fp_pg, q);
	fclose(fp_pg);
	cout << "\np, g파일 생성\n";

}



vector<mpz_class> OPRFKeyGen(FILE *Key,int k, int l, unsigned long ctr) {
	vector<mpz_class> key;
	mpz_class tmp;
	gmp_randstate_t rstate;
	uint64_t        seed = (unsigned long)time(0) + ctr;
	gmp_randinit_mt(rstate);
	gmp_randseed_ui(rstate, seed);


	/// randomizer by a Mersenne Twister algorithm
	for (int i = 0; i < l; i++) {
		mpz_init(tmp.get_mpz_t()); 
		mpz_set_ui(tmp.get_mpz_t(), 0);
		mpz_urandomb(tmp.get_mpz_t(), rstate, k);
		key.push_back(tmp);
		_writeMpzToFile(Key, tmp.get_mpz_t());
	}
	
	return key;
}

mpz_class G_op(vector<mpz_class> K, string x ,mpz_t p, mpz_t g) {
	int len = x.length();
	int cnt = 0;

	mpz_t temp;
	mpz_class y(g);
	mpz_class tmp=1;
	
	unsigned char str_x[16] = {0};
	for (int i = 0; i < len; i++)
		str_x[i] = x[i];
	
	unsigned char bit_mask[8] = {1, 2, 4, 8, 16, 32, 64, 128};


	for (int i = 0; i < len; i++) {
		for (int j = 0; j < 8; j++) {
			if ((str_x[i] & bit_mask[j]) == bit_mask[j]) {
				tmp = tmp * K[cnt];
			}
			cnt++;
		}
	}
	mpz_powm(y.get_mpz_t(), g, tmp.get_mpz_t(), p);

	return y;
}

int _writeMpzToFile(FILE* f, const mpz_t x)
{
	size_t      len = mpz_size(x) * sizeof(mp_limb_t);
	uint8_t*    buf = (uint8_t *)calloc(len, sizeof(uint8_t));

	/// uncoditionally little endian-ness
	for (size_t i = 0; i < BYTE_WIDTH; i++) {
		uint8_t b = (len >> 8 * i) % 256;
		fwrite(&b, sizeof(uint8_t), sizeof(uint8_t), f);
	}
	/// convert a byte array to mpz_t
	mpz_export(buf, &len, MSB_FIRST, sizeof(uint8_t), 0, 0, x);
	fwrite(buf, sizeof(uint8_t), len, f);

	memset(buf, 0, len);
	free(buf);

	return 1;
}

int _readMpzFromFile(FILE* f, mpz_t x)
{
	size_t      len = 0;
	uint8_t*    buf = 0;

	for (size_t i = 0; i < BYTE_WIDTH; i++) {
		uint8_t b = 0;

		fread(&b, 1, 1, f);
		len += (b << 8 * i);
	}
	buf = (uint8_t *)calloc(len, sizeof(uint8_t));
	fread(buf, 1, len, f);

	/// convert mpz_t to a byte array
	mpz_import(x, len, MSB_FIRST, sizeof(uint8_t), 0, 0, buf);

	memset(buf, 0, len);
	free(buf);

	return 1;
}













