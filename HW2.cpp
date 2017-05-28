#include<iostream>
#include<fstream>
#include<string>

using namespace std;
#define DEFAULT_BUCKET_SIZE 2
#define DEFAULT_SLOT_SIZE 128

class students {
public:
	char name[20];
	unsigned sid;
	float score;
	unsigned aid;
	int size;
};

class HashMap {
private:
	int bucketSize;
	int slotSize;
	students **table;

	void resize() {
		cout << "resize ½ÃÀÛ" << endl;
		int oldTableSize = bucketSize;
		bucketSize += 1;
		students** oldTable = table;
		table = new students*[bucketSize];
		for (int i = 0; i < bucketSize; ++i) {
			table[i] = new students[slotSize];
		}
		for (int i = 0; i < bucketSize; i++) {
			for (int j = 0; j < slotSize; ++j) {
				table[i][j].sid = 0;
				strcpy(table[i][j].name, "0");
				table[i][j].score = 0;
				table[i][j].aid = 0;
				table[i][j].size = 0;
			}
		}
		for (int hashi = 0; hashi < oldTableSize; hashi++) {
			if (oldTable[hashi][0].size != 0) {
				for (int hashj = 0; hashj < oldTable[hashi][0].size; hashj++){
					put(oldTable[hashi][hashj].sid, oldTable[hashi][hashj].name, oldTable[hashi][hashj].score, oldTable[hashi][hashj].aid);
				}
			}
		}
		delete[] oldTable;
	}
	int hashfunction(int key) {
		int result;
		result = (key % bucketSize);
		return result;
	}
public:
	HashMap() {
		bucketSize = 2;
		slotSize = 128;
		table = new students*[bucketSize];
		for (int i = 0; i < bucketSize; ++i) {
			table[i] = new students[slotSize];
		}
		for (int i = 0; i < bucketSize; i++) {
			for (int j = 0; j < slotSize; j++) {
				table[i][j].sid = 0;
				strcpy(table[i][j].name, "0");
				table[i][j].score = 0;
				table[i][j].aid = 0;
				table[i][j].size = 0;
			}
		}
	}
	void put(int sid, char name[20], float score, int aid) {
		int hash1 = (sid % bucketSize);
		if (table[hash1][0].size < 128) {
			for (int i = 0; i < 128; i++) {
				if (table[hash1][i].sid == 0) {
					table[hash1][i].sid = sid;
					strcpy(table[hash1][i].name, name);
					table[hash1][i].score = score;
					table[hash1][i].aid = aid;
					table[hash1][0].size++;
					break;
				}
			}
		}
		else {
			resize();
			put(sid, name, score, aid);
		}
	}
	void print() {
		for (int i = 0; i < bucketSize; i++) {
			cout << "bucket" << i << " bucketSize" << table[i][0].size << endl;
			for (int j = 0; j < table[i][0].size; j++) {
				//cout << table[i][j].name << " " << table[i][j].sid << " " << " " << table[i][j].score << " " << table[i][j].aid << endl;
				;
			}
		}
	}
	~HashMap() {
		for (int hashi = 0; hashi < bucketSize; hashi++) {
			delete[] table[hashi];
		}
		delete[] table;
	}
};

int main() {
	ifstream fi;
	int num;
	int sid1;
	int aid1;
	float score1;
	char name1[20];
	HashMap a;
	fi.open("sampleData.csv");
	string bufstr;

	getline(fi, bufstr, ' ');
	num = atoi(bufstr.c_str());
	for (int i = 0; i < num; i++) {
		getline(fi, bufstr, ',');
		strcpy(name1, bufstr.c_str());
		getline(fi, bufstr, ',');
		sid1 = atoi(bufstr.c_str());
		getline(fi, bufstr, ',');
		score1 = atof(bufstr.c_str());
		getline(fi, bufstr);
		aid1 = atoi(bufstr.c_str());
		a.put(sid1, name1, score1, aid1);
	}
	a.print();
	fi.close();
	return 0;
}