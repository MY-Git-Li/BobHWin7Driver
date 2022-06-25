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
	//返回接受字符串所需缓冲区的大小，已经包含字符结尾符'\0'
	iSize = WideCharToMultiByte(CP_ACP, 0, a, -1, NULL, 0, NULL, NULL); //iSize =wcslen(pwsUnicode)+1=6
	pszMultiByte = (char*)malloc(iSize * sizeof(char)); //不需要 pszMultiByte = (char*)malloc(iSize*sizeof(char)+1);
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
		OutputDebugString(TEXT("提升权限失败,LookupPrivilegeValue"));
		return false;
	}
	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		OutputDebugString(TEXT("提升权限失败 AdjustTokenPrivileges"));
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
		printf("打开设备失败\n");
		system("pause");
		return 0;
	}
	
	printf("打开设备成功\n");

	system("pause");

	printf("\n\n\n");
	printf("开始得到程序的模块地址---非wow64程序（calc.exe）(USER32.dll)为例\n");
	{
		ULONG64 address = GetModuleBaseAddress(GetPidByProcessName("A1SysTest.exe"), "USER32.dll");
		printf("address<%llx>\n", address);
	}

	printf("\n\n\n");
	printf("开始得到程序的模块地址--wow64程序（DriverMonitor.exe）(USER32.dll)为例\n");
	{
		printf("pid<%d>", GetPidByProcessName("A1SysTest.exe"));
		ULONG64 address = GetModuleBaseAddress(GetPidByProcessName("A1SysTest.exe"), "USER32.dll");
		printf("address<%llx>\n", address);
	}
	HideProcess(GetPidByProcessName("A1SysTest.exe"));

	/*printf("\n\n\n");
	printf("开始得到程序pid---（calc.exe）为例\n");*/
	/*{
		system("pause");

		printf("pid<%d>\n", GetPidByProcessName("calc.exe"));
	}*/

	//printf("\n\n\n");
	//printf("开始保护进程（calc.exe）为例\n");
	/*{
		system("pause");
		ProtectProcess(GetPidByProcessName("calc.exe"));
		printf("保护成功\n");
		system("pause");
		StopProtectProcess();
		printf("解除保护成功\n");
	}*/


	//printf("\n\n\n");
	//printf("开始强制删除文件---（本程序为例）\n");
	// {
	//	system("pause");
	//	wchar_t a[200];
	//	//得到当前文件路径名
	//	GetModuleFileName(NULL, a, 200);
	//	char* b = w2c(a);
	//	ForceDeleteFile(b);
	//}

	printf("\n\n\n");
	printf("开始读取进程内存地址数据(整数):\n");
	{

		DWORD Pid, data;
		ULONG64 address = 0;

		printf("请输入设置读写的PID：\n");
		scanf_s("%d", &Pid);



		printf("请输入读的地址为：\n");
		scanf_s("%llx", &address);


		ReadMemoryDWORD(Pid, address, &data);

		printf("读到的数据为 = %d\n", data);


		system("pause");

		printf("开始写内存\n");

		printf("请输入写地址：\n");
		scanf_s("%llx", &address);



		printf("请输入写的多少：\n");
		scanf_s("%d", &data);

		WriteMemoryDWORD(Pid, address, data);

		system("pause");

	}

	/*printf("开始读写取数组\n");*/
	/*{
		DWORD Pid, data;
		ULONG64 address = 0;

		BYTE buffer[8];
		BYTE* bufferPtr = buffer;

		printf("请输入设置读写的PID：\n");
		scanf_s("%d", &Pid);


		printf("请输入读的地址为：\n");
		scanf_s("%llx", &address);

		ReadMemoryBytes(Pid, address, &bufferPtr, sizeof(buffer));

		for (int i = 0; i < 8; i++)
		{
			printf("0x%x  ", buffer[i]);
		}

		printf("\n开始写数组\n");
		system("pause");


		BYTE writebuff[8] = { 0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88 };

		WriteMemoryBytes(Pid, address, writebuff, sizeof(writebuff));





	}*/

	/*printf("开始读取浮点数\n");*/
	/*{

		DWORD Pid;
		ULONG64 address = 0;
		float dataf;
		double datad;

		printf("请输入设置读写的PID：\n");
		scanf_s("%d", &Pid);

		printf("请输入读的float地址为：\n");
		scanf_s("%llx", &address);

		ReadMemoryFloat(Pid, address, &dataf);

		printf("dataf = %f", dataf);

		system("pause");

		printf("请输入读的double地址为：\n");
		scanf_s("%llx", &address);

		ReadMemoryDouble(Pid, address, &datad);

		printf("datad = %lf", datad);
	}*/

	/*printf("开始写取浮点数\n");*/
	/*{
		DWORD Pid;
		ULONG64 address = 0;
		float dataf;
		double datad;

		printf("请输入设置读写的PID：\n");
		scanf_s("%d", &Pid);

		printf("请输入写的float地址为：\n");
		scanf_s("%llx", &address);

		dataf = 520.1314;

		WriteMemoryFloat(Pid, address, dataf);

		system("pause");

		printf("请输入写的double地址为：\n");
		scanf_s("%llx", &address);

		datad = 521.1314;
		WriteMemoryDouble(Pid, address, datad);


	}*/

	system("pause");
}