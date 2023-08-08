#include <string>
#include <iostream>
#include <sstream>

#ifdef __LINUX__
#include <cpuid.h>
#endif
#ifdef __MINGW__
#include <Windows.h>
#endif

#define C1 21500
#define C2 20237
typedef unsigned short      WORD;
WORD key = 1314;

#ifdef __LINUX__
std::string GetCPUId()
{
	std::string strCPUId;
	unsigned int level = 1;
	unsigned eax = 3, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	int first = ((eax >> 24) & 0xff) | ((eax << 8) & 0xff0000) | ((eax >> 8) & 0xff00) | ((eax << 24) & 0xff000000);
	int last = ((edx >> 24) & 0xff) | ((edx << 8) & 0xff0000) | ((edx >> 8) & 0xff00) | ((edx << 24) & 0xff000000);
	std::stringstream ss;
	ss << std::hex << first;
	ss << std::hex << last;
	ss >> strCPUId;
	return strCPUId;
}
#endif

#ifdef __MINGW__
std::string GetCPUId() {
	INT32 dwBuf[4];
	std::string strCPUId;
	char buf[32] = { 0 };
	__cpuidex(dwBuf, 1, 1);
	memset(buf, 0, 32);
	sprintf_s(buf, 32, "%08X", dwBuf[3]);
	strCPUId += buf;
	memset(buf, 0, 32);
	sprintf_s(buf, 32, "%08X", dwBuf[0]);
	strCPUId += buf;
	return strCPUId;
}

std::string GetHardDiskSerialNumber() {
	std::string serialNumber;
	DWORD serialNumberLong;
	if (GetVolumeInformationA("C:\\", nullptr, 0, &serialNumberLong, nullptr, nullptr, nullptr, 0)) {
		char serialNumberBuffer[100];
		sprintf_s(serialNumberBuffer, "%08lX", serialNumberLong);
		serialNumber = serialNumberBuffer;
	}
	return serialNumber;
}
#endif

std::string Encrypt(std::string ID, WORD Key)
{
	std::string Result, str;
	int i, j;

	Result = ID;
	for (i = 0; i < ID.length(); i++)
	{
		Result[i] = ID[i] ^ (Key >> 5);
		Key = ((BYTE)Result[i] + Key) * C1 + C2;
	}
	ID = Result;
	Result = "";
	for (i = 0; i < ID.length(); i++)
	{
		j = (BYTE)ID[i];
		str = "12";
		str[0] = 65 + j / 26;
		str[1] = 65 + j % 26;
		Result += str;
	}
	return Result;
}


std::string Decrypt(std::string ID, WORD Key)
{
	std::string Result, str;
	int i, j;

	Result = "";
	for (i = 0; i < ID.length() / 2; i++)
	{
		j = ((BYTE)ID[2 * i] - 65) * 26;

		j += (BYTE)ID[2 * i + 1] - 65;
		str = "1";
		str[0] = j;
		Result += str;
	}
	ID = Result;
	for (i = 0; i < ID.length(); i++)
	{
		Result[i] = (BYTE)ID[i] ^ (Key >> 5);
		Key = ((BYTE)ID[i] + Key) * C1 + C2;
	}
	return Result;
}

int main() {
	std::string serialNumber = GetHardDiskSerialNumber();
	if (!serialNumber.empty()) {
		std::cout << "Hard Disk Serial Number: " << serialNumber << std::endl;
	}
	else {
		std::cout << "Failed to get Hard Disk Serial Number." << std::endl;
	}
	std::string cpuid = GetCPUId();
	std::string inputkey = cpuid + "-" + serialNumber;
	std::string encryptcupid = Encrypt(inputkey, key);
	std::cout << "Encrypt cpuid is : " << encryptcupid << std::endl;

	std::string decryptcpuid = Decrypt(encryptcupid, key);
	std::cout << "Decrypt cpuid is : " << decryptcpuid << std::endl;

	if (encryptcupid != "TESTCPUID") {
		std::cout << "Sorry i can not help you" << std::endl;
		return -1;
	}

	std::cin.get();
}
