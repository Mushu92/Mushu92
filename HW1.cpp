#include <fstream>
#include <string>
#include <io.h>
#include <iostream>
#include <windows.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <direct.h>


using namespace std;
char* timeToString(struct tm *time);

char* timeToString(struct tm *time) {
	static char totaltime[20];

	sprintf(totaltime, "%04d-%02d-%02d %02d:%02d:%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
	return totaltime;
}

bool checkTime(string a, string b)
{
	struct _stat bufA, bufB;
	const char* filenameA = a.c_str();
	const char* filenameB = b.c_str();

	if (_stat(filenameA, &bufA) != 0)
	{
		switch (errno)
		{
		case ENOENT:
			fprintf(stderr, "File %s not found.\n", filenameA);
			break;
		case EINVAL:
			fprintf(stderr, "Invalid parameter to _stat.\n");
			break;
		default:
			fprintf(stderr, "Unexpected error in _stat.\n");
		}
	}

	if (_stat(filenameB, &bufB) != 0)
	{
		switch (errno)
		{
		case ENOENT:
			return true;
			break;
		case EINVAL:
			fprintf(stderr, "Invalid parameter to _stat.\n");
			break;
		default:
			fprintf(stderr, "Unexpected error in _stat.\n");
		}

	}

	else
	{
		int timecheck_a, timecheck_b;

		timecheck_a = localtime(&bufA.st_mtime)->tm_year;
		timecheck_b = localtime(&bufB.st_mtime)->tm_year;

		if (timecheck_a > timecheck_b)//a, b �⵵�� ��
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}
		timecheck_a = localtime(&bufA.st_mtime)->tm_mon;
		timecheck_b = localtime(&bufB.st_mtime)->tm_mon;

		if (timecheck_a > timecheck_b)//�⵵ �� �� a�� ũ�� ���� ��
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}

		timecheck_a = localtime(&bufA.st_mtime)->tm_mday;
		timecheck_b = localtime(&bufB.st_mtime)->tm_mday;

		if (timecheck_a > timecheck_b)// ���� �� �� a�� ũ�� �� ��
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}

		timecheck_a = localtime(&bufA.st_mtime)->tm_hour;
		timecheck_b = localtime(&bufB.st_mtime)->tm_hour;

		if (timecheck_a > timecheck_b)//�� �� �� a�� ũ�� �ð� ��
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}
		timecheck_a = localtime(&bufA.st_mtime)->tm_min;
		timecheck_b = localtime(&bufB.st_mtime)->tm_min;

		if (timecheck_a > timecheck_b)//�ð� �� �� a�� ũ�� �� ��
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}

		timecheck_a = localtime(&bufA.st_mtime)->tm_sec;
		timecheck_b = localtime(&bufB.st_mtime)->tm_sec;
		if (timecheck_a > timecheck_b)//�� �� �� b�� ũ�� ��� x 
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}
	}
}

void Checkinput(string a, string b) {

	struct _stat bufA, bufB;
	if (_stat(a.c_str(), &bufA) != 0)
	{
		switch (errno)
		{
		case ENOENT:
			fprintf(stderr, "Directory %s backup complete.\n", a.c_str());
			exit(1);
			break;
		case EINVAL:
			fprintf(stderr, "Invalid parameter to _stat.\n");
			exit(1);
			break;
		default:
			fprintf(stderr, "Unexpected error in _stat.\n");
			exit(1);
		}
	}
	if (_stat(b.c_str(), &bufB) != 0)
	{
		switch (errno)
		{
		case ENOENT:
			fprintf(stderr, "Directory %s not found so make directory. \n", b.c_str());
			mkdir(b.c_str());
			break;
		case EINVAL:
			fprintf(stderr, "Invalid parameter to _stat.\n");
			exit(1);
		default:
			fprintf(stderr, "Unexpected error in _stat.\n");
			exit(1);
		}
	}
}

int CopyFile(string initialFilePath, string outputFilePath)
{

	ifstream initialFile(initialFilePath.c_str(), ios::in | ios::binary);//��ǲ ����
	ofstream outputFile(outputFilePath.c_str(), ios::out | ios::binary);//�ƿ�ǲ ����

	initialFile.seekg(0, ios::end);
	long fileSize = initialFile.tellg();

	//�������, ������ �� ������
	if (initialFile.is_open() && outputFile.is_open())
	{
		//������ ���ۿ� �ִ´� �̶� Ȧ�� �϶� ������ ©���ϱ� +1
		short * buffer = new short[fileSize / 2 + 1];

		//ó������ ã�� �����Ѵ�
		initialFile.seekg(0, ios::beg);
		//���ۿ� �ִ� ���� �д´�
		initialFile.read((char*)buffer, fileSize);
		//���� ���� ����
		outputFile.write((char*)buffer, fileSize);
		delete[] buffer;
	}

	else if (!outputFile.is_open())
	{
		cout << "1 I couldn't open " << outputFilePath << " for copying!\n";
		return 0;
	}
	else if (!initialFile.is_open())
	{
		cout << "2 I couldn't open " << initialFilePath << " for copying!\n";
		return 0;
	}

	initialFile.close();
	outputFile.close();

	return 1;
}

void find(string a, string b)
{
	int i = 0;
	string path = a + "*.*";
	struct _finddata_t fd;
	intptr_t handle;

	ofstream os("mybackuplog.log", ios::app);

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L) cout << "No files in directory" << endl;
	do
	{
		if (i < 2) {//., ..  �ѱ��
			i++;
			continue;
		}

		if ((fd.attrib&_A_SUBDIR) == 16) {//directory���
			mkdir((b + fd.name).c_str());//���� ����
			find(a + fd.name + "\\", b + fd.name + "\\");//���� �ȿ��� ���� scan
			continue;
		}

		if (checkTime(a + fd.name, b + fd.name)) {//���� üũ
			CopyFile(a + fd.name, b + fd.name);
			os << b  << fd.name << endl;//mybackuplog�� �Ѹ���
			cout << b << fd.name << endl;
		}

	} while (_findnext(handle, &fd) == 0);

	os.close();
	_findclose(handle);

}


int main(int argc, char* argv[])
{
	string a, b;
	//optionȮ��
	if (argc == 2) {
		cout << "please input SourcePath,FinalPath" << endl;
		return 0;
	}
	else if (argc > 3) {//option�� 3�� �̻� ������ ���� 2���� ����Ѵ�.
		cout << "Many input, use first,second" << endl;
	}
	a = argv[1];
	b = argv[2];
	Checkinput(a + "\\", b + "\\");
	ofstream os("mybackuplog.log");//���α׷� �����ϸ� mybackuplog�ٽ� ����� ����
	find(a + "\\", b + "\\");
	system("pause");
	os.close();
}