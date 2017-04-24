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

		if (timecheck_a > timecheck_b)//a, b 년도를 비교
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}
		timecheck_a = localtime(&bufA.st_mtime)->tm_mon;
		timecheck_b = localtime(&bufB.st_mtime)->tm_mon;

		if (timecheck_a > timecheck_b)//년도 비교 후 a가 크면 개월 비교
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}

		timecheck_a = localtime(&bufA.st_mtime)->tm_mday;
		timecheck_b = localtime(&bufB.st_mtime)->tm_mday;

		if (timecheck_a > timecheck_b)// 개월 비교 후 a가 크면 날 비교
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}

		timecheck_a = localtime(&bufA.st_mtime)->tm_hour;
		timecheck_b = localtime(&bufB.st_mtime)->tm_hour;

		if (timecheck_a > timecheck_b)//날 비교 후 a가 크면 시간 비교
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}
		timecheck_a = localtime(&bufA.st_mtime)->tm_min;
		timecheck_b = localtime(&bufB.st_mtime)->tm_min;

		if (timecheck_a > timecheck_b)//시간 비교 후 a가 크면 분 비교
		{
			return true;
		}
		if (timecheck_a < timecheck_b)
		{
			return false;
		}

		timecheck_a = localtime(&bufA.st_mtime)->tm_sec;
		timecheck_b = localtime(&bufB.st_mtime)->tm_sec;
		if (timecheck_a > timecheck_b)//분 비교 후 b가 크면 백업 x 
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

	ifstream initialFile(initialFilePath.c_str(), ios::in | ios::binary);//인풋 생성
	ofstream outputFile(outputFilePath.c_str(), ios::out | ios::binary);//아웃풋 생성

	initialFile.seekg(0, ios::end);
	long fileSize = initialFile.tellg();

	//원본경로, 백업경로 다 있을때
	if (initialFile.is_open() && outputFile.is_open())
	{
		//파일을 버퍼에 넣는다 이때 홀수 일때 파일이 짤리니까 +1
		short * buffer = new short[fileSize / 2 + 1];

		//처음부터 찾기 시작한다
		initialFile.seekg(0, ios::beg);
		//버퍼에 있는 것을 읽는다
		initialFile.read((char*)buffer, fileSize);
		//읽은 것을 쓴다
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
		if (i < 2) {//., ..  넘기기
			i++;
			continue;
		}

		if ((fd.attrib&_A_SUBDIR) == 16) {//directory라면
			mkdir((b + fd.name).c_str());//폴더 생성
			find(a + fd.name + "\\", b + fd.name + "\\");//폴더 안에서 파일 scan
			continue;
		}

		if (checkTime(a + fd.name, b + fd.name)) {//파일 체크
			CopyFile(a + fd.name, b + fd.name);
			os << b  << fd.name << endl;//mybackuplog에 뿌리기
			cout << b << fd.name << endl;
		}

	} while (_findnext(handle, &fd) == 0);

	os.close();
	_findclose(handle);

}


int main(int argc, char* argv[])
{
	string a, b;
	//option확인
	if (argc == 2) {
		cout << "please input SourcePath,FinalPath" << endl;
		return 0;
	}
	else if (argc > 3) {//option이 3개 이상 들어오면 앞의 2개만 사용한다.
		cout << "Many input, use first,second" << endl;
	}
	a = argv[1];
	b = argv[2];
	Checkinput(a + "\\", b + "\\");
	ofstream os("mybackuplog.log");//프로그램 실행하면 mybackuplog다시 만들기 위함
	find(a + "\\", b + "\\");
	system("pause");
	os.close();
}