#include <iostream>
#include "windows.h"
#pragma comment (lib,"Advapi32.lib")
#pragma comment(lib, "BobHWinDriverDll.lib")                                   
extern "C" __declspec(dllimport) bool InitDriver();


extern "C" __declspec(dllimport) void ReadMemoryDWORD(DWORD pid, ULONG64 addre, DWORD * ret);
extern "C" __declspec(dllimport) void WriteMemoryDWORD(DWORD pid, ULONG64 addre, DWORD ret);

extern "C" __declspec(dllimport) void ReadMemoryDWORD64(DWORD pid, ULONG64 addre, DWORD64 * ret);
extern "C" __declspec(dllimport) void WriteMemoryDWORD64(DWORD pid, ULONG64 addre, DWORD64 ret);

extern "C" __declspec(dllexport) void ReadMemoryBytes(DWORD pid, ULONG64 addre, BYTE * *ret, DWORD sizes);
extern "C" __declspec(dllexport) void WriteMemoryBytes(DWORD pid, ULONG64 addre, BYTE * ret, DWORD sizes);

extern "C" __declspec(dllexport) void ReadMemoryFloat(DWORD pid, ULONG64 addre, float* ret);
extern "C" __declspec(dllexport) void WriteMemoryFloat(DWORD pid, ULONG64 addre, float ret);

extern "C" __declspec(dllexport) void ReadMemoryDouble(DWORD pid, ULONG64 addre, double* ret);
extern "C" __declspec(dllexport) void WriteMemoryDouble(DWORD pid, ULONG64 addre, double ret);

extern "C" __declspec(dllexport) void ForceDeleteFile(const char* path);

extern "C" __declspec(dllexport) DWORD GetPidByProcessName(const char* name);

extern "C" __declspec(dllexport) void ProtectProcess(DWORD pid);

extern "C" __declspec(dllexport) void StopProtectProcess();

extern "C" __declspec(dllexport) ULONG64 GetModuleBaseAddress(DWORD pid, const char* name);

extern "C" __declspec(dllexport) void HideProcess(DWORD pid);

extern "C" __declspec(dllexport) bool UnloadDriver();

char* w2c(wchar_t* a)
{
	char* pszMultiByte;
	int iSize;
	//���ؽ����ַ������軺�����Ĵ�С���Ѿ������ַ���β��'\0'
	iSize = WideCharToMultiByte(CP_ACP, 0, a, -1, NULL, 0, NULL, NULL); //iSize =wcslen(pwsUnicode)+1=6
	pszMultiByte = (char*)malloc(iSize * sizeof(char)); //����Ҫ pszMultiByte = (char*)malloc(iSize*sizeof(char)+1);
	WideCharToMultiByte(CP_ACP, 0, a, -1, pszMultiByte, iSize, NULL, NULL);
	return pszMultiByte;
}
bool AdjustPrivileges() {
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
	LUID luid;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		OutputDebugString(TEXT("����Ȩ��ʧ��,LookupPrivilegeValue"));
		return false;
	}
	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		OutputDebugString(TEXT("����Ȩ��ʧ�� AdjustTokenPrivileges"));
		return false;
	}
	// close handles
	CloseHandle(hToken);
	return true;
}
int main()
{

	if (!InitDriver())
	{
		printf("���豸ʧ��\n");
		system("pause");
		return 0;
	}
	
	printf("���豸�ɹ�\n");

	system("pause");

	printf("\n\n\n");
	printf("��ʼ�õ������ģ���ַ---��wow64����calc.exe��(USER32.dll)Ϊ��\n");
	{
		ULONG64 address = GetModuleBaseAddress(GetPidByProcessName("A1SysTest.exe"), "USER32.dll");
		printf("address<%llx>\n", address);
	}

	printf("\n\n\n");
	printf("��ʼ�õ������ģ���ַ--wow64����DriverMonitor.exe��(USER32.dll)Ϊ��\n");
	{
		printf("pid<%d>", GetPidByProcessName("A1SysTest.exe"));
		ULONG64 address = GetModuleBaseAddress(GetPidByProcessName("A1SysTest.exe"), "USER32.dll");
		printf("address<%llx>\n", address);
	}
	HideProcess(GetPidByProcessName("A1SysTest.exe"));

	/*printf("\n\n\n");
	printf("��ʼ�õ�����pid---��calc.exe��Ϊ��\n");*/
	/*{
		system("pause");

		printf("pid<%d>\n", GetPidByProcessName("calc.exe"));
	}*/

	//printf("\n\n\n");
	//printf("��ʼ�������̣�calc.exe��Ϊ��\n");
	/*{
		system("pause");
		ProtectProcess(GetPidByProcessName("calc.exe"));
		printf("�����ɹ�\n");
		system("pause");
		StopProtectProcess();
		printf("��������ɹ�\n");
	}*/


	//printf("\n\n\n");
	//printf("��ʼǿ��ɾ���ļ�---��������Ϊ����\n");
	// {
	//	system("pause");
	//	wchar_t a[200];
	//	//�õ���ǰ�ļ�·����
	//	GetModuleFileName(NULL, a, 200);
	//	char* b = w2c(a);
	//	ForceDeleteFile(b);
	//}

	printf("\n\n\n");
	printf("��ʼ��ȡ�����ڴ��ַ����(����):\n");
	{

		DWORD Pid, data;
		ULONG64 address = 0;

		printf("���������ö�д��PID��\n");
		scanf_s("%d", &Pid);



		printf("��������ĵ�ַΪ��\n");
		scanf_s("%llx", &address);


		ReadMemoryDWORD(Pid, address, &data);

		printf("����������Ϊ = %d\n", data);


		system("pause");

		printf("��ʼд�ڴ�\n");

		printf("������д��ַ��\n");
		scanf_s("%llx", &address);



		printf("������д�Ķ��٣�\n");
		scanf_s("%d", &data);

		WriteMemoryDWORD(Pid, address, data);

		system("pause");

	}

	/*printf("��ʼ��дȡ����\n");*/
	/*{
		DWORD Pid, data;
		ULONG64 address = 0;

		BYTE buffer[8];
		BYTE* bufferPtr = buffer;

		printf("���������ö�д��PID��\n");
		scanf_s("%d", &Pid);


		printf("��������ĵ�ַΪ��\n");
		scanf_s("%llx", &address);

		ReadMemoryBytes(Pid, address, &bufferPtr, sizeof(buffer));

		for (int i = 0; i < 8; i++)
		{
			printf("0x%x  ", buffer[i]);
		}

		printf("\n��ʼд����\n");
		system("pause");


		BYTE writebuff[8] = { 0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88 };

		WriteMemoryBytes(Pid, address, writebuff, sizeof(writebuff));





	}*/

	/*printf("��ʼ��ȡ������\n");*/
	/*{

		DWORD Pid;
		ULONG64 address = 0;
		float dataf;
		double datad;

		printf("���������ö�д��PID��\n");
		scanf_s("%d", &Pid);

		printf("���������float��ַΪ��\n");
		scanf_s("%llx", &address);

		ReadMemoryFloat(Pid, address, &dataf);

		printf("dataf = %f", dataf);

		system("pause");

		printf("���������double��ַΪ��\n");
		scanf_s("%llx", &address);

		ReadMemoryDouble(Pid, address, &datad);

		printf("datad = %lf", datad);
	}*/

	/*printf("��ʼдȡ������\n");*/
	/*{
		DWORD Pid;
		ULONG64 address = 0;
		float dataf;
		double datad;

		printf("���������ö�д��PID��\n");
		scanf_s("%d", &Pid);

		printf("������д��float��ַΪ��\n");
		scanf_s("%llx", &address);

		dataf = 520.1314;

		WriteMemoryFloat(Pid, address, dataf);

		system("pause");

		printf("������д��double��ַΪ��\n");
		scanf_s("%llx", &address);

		datad = 521.1314;
		WriteMemoryDouble(Pid, address, datad);


	}*/

	system("pause");
}