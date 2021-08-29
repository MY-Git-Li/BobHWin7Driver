// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "stdlib.h"
#include "windows.h"
#include <locale.h>
#include <iostream>
#include "PassPG.h"


using namespace std;

#define BOBH_SET CTL_CODE(FILE_DEVICE_UNKNOWN,0x810,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_READ CTL_CODE(FILE_DEVICE_UNKNOWN,0x811,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN,0x812,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_PROTECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x813,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_UNPROTECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x814,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_KILLPROCESS_DIRECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x815,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_KILLPROCESS_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN,0x816,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_GETMODULEADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN,0x817,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_GETPROCESSID CTL_CODE(FILE_DEVICE_UNKNOWN,0x818,METHOD_BUFFERED,FILE_ANY_ACCESS)

typedef struct ModuleBase
{
	ULONG Pid;
	WCHAR ModuleName[100];
}UModuleBase, * LPModuleBase;

struct r3Buffer {
	ULONG64 Address;
	ULONG64 Buffer;
	ULONG64 size;
}appBuffer;
typedef struct _MyCHAR
{
	CHAR _char[100];
}MYCHAR, * PMyCHAR;



string ws2s(const wstring& ws)
{
	string curLocale = setlocale(LC_ALL, NULL);     //curLocale="C"
	setlocale(LC_ALL, "chs");
	const wchar_t* wcs = ws.c_str();
	size_t dByteNum = sizeof(wchar_t) * ws.size() + 1;
	cout << "ws.size():" << ws.size() << endl;      //5：宽字符串L"ABC我们"有5个自然字符

	char* dest = new char[dByteNum];
	wcstombs_s(NULL, dest, dByteNum, wcs, _TRUNCATE);
	string result = dest;
	delete[] dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

wstring s2ws(const string& s)
{
	string curLocale = setlocale(LC_ALL, NULL);  //curLocale="C"
	setlocale(LC_ALL, "chs");
	const char* source = s.c_str();
	size_t charNum = s.size() + 1;
	cout << "s.size():" << s.size() << endl;         //7：多字节字符串"ABC我们"有7个字节

	wchar_t* dest = new wchar_t[charNum];
	mbstowcs_s(NULL, dest, charNum, source, _TRUNCATE);
	wstring result = dest;
	delete[] dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}






typedef int(__fastcall* ASAS)(int a,int b);


VOID __fastcall   fun(int a, int b)
{

}


int main()
{
	
	/*PassPG();*/
	//HANDLE hdevice = NULL;


	//hdevice = CreateFile(L"\\\\.\\BobHWin7ReadLink", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
	//
	//{
	//	if (hdevice == INVALID_HANDLE_VALUE)
	//	{
	//		printf("打开失败\n");
	//		system("pause");
	//		return 0;
	//	}
	//	printf("打开成功\n");
	//	system("pause");

	//	DWORD Pid, Count;

	//	printf("请输入设置读写的PID\n");
	//	scanf_s("%d", &Pid);
	//	printf("你输入的PID为：%d\n", Pid);
	//	system("pause");

	//	printf("开始设置读写的PID\n");
	//	DeviceIoControl(hdevice, BOBH_SET, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
	//	printf("设置PID成功\n");
	//	system("pause");

	//	DWORD bianliang = 520;
	//	printf("写之前变量的地址为%llx,变量的值为%d\n", &bianliang, bianliang);


	//	system("pause");

	//	ULONG64 address=0;
	//	printf("请输入读的地址为\n");
	//	scanf_s("%llx", &address);
	//	printf("你输入的地址是 %llx\n", address);


	//	appBuffer.Address = address;
	//	appBuffer.Buffer = { 0 };
	//	appBuffer.size = sizeof(DWORD);

	//	printf("开始读内存\n");

	//	system("pause");

	//	DeviceIoControl(hdevice, BOBH_READ, &appBuffer, sizeof(appBuffer), &appBuffer, sizeof(appBuffer), &Count, NULL);


	//	printf("读到的数据为 = %d\n", appBuffer.Buffer);


	//	system("pause");

	//	printf("开始写内存\n");

	//	printf("请输入写地址\n");
	//	scanf_s("%llx", &address);
	//	printf("你输入的地址是 %llx\n", address);
	//	appBuffer.Address = address;

	//	printf("请输入写的多少\n");
	//	scanf_s("%d", &appBuffer.Buffer);

	//	appBuffer.size = sizeof(DWORD);

	//	system("pause");

	//	DeviceIoControl(hdevice, BOBH_WRITE, &appBuffer, sizeof(appBuffer), &appBuffer, sizeof(appBuffer), &Count, NULL);

	//	printf("写之后变量的地址为%x,变量的值为%d\n", &bianliang, bianliang);
	//	system("pause");


	//	printf("开始保护进程\n");

	//	printf("请输入要保护的PID\n");
	//	scanf_s("%d", &Pid);
	//	printf("你输入的的PID为：%d\n", Pid);
	//	system("pause");

	//	DeviceIoControl(hdevice, BOBH_PROTECT, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
	//	printf("保护进程完成\n");
	//	system("pause");

	//	printf("关闭保护进程\n");
	//	DeviceIoControl(hdevice, BOBH_UNPROTECT, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
	//	printf("关闭进程完成\n");
	//	system("pause");
	//}
	//
	//

	///*DeviceIoControl(hdevice, BOBH_UNPROTECT, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);*/
	////{

	////	UModuleBase iostruct = { 0 };

	////	ULONG Pid = 0;

	////	printf("输入目标进程PID\n");

	////	scanf_s("%d", &Pid);

	////	iostruct.Pid = Pid;

	////	printf("输入目标进程Dll模块名称\n");


	////	string a;
	////	cin >> a;
	////	wstring b = s2ws(a);

	////	/*cout << "普通字符串为" << &b[0] << endl;

	////	cout <<"普通字符串为" << a << endl;

	////	wcout << L"宽字符为" << b << endl;*/

	////	system("pause");
	////	//这里的宽字符太傻逼了
	////	//KERNELBASE.DLL

	////	RtlCopyMemory(iostruct.ModuleName, b.c_str(), b.size()*2);

	////	DWORD Count = 0;
	////	ULONG64 dllbase = 0;
	////	DeviceIoControl(hdevice, BOBH_GETMODULEADDRESS, &iostruct, sizeof(iostruct), &dllbase, sizeof(dllbase), &Count, NULL);
	////	printf("Dll base %llx\n", dllbase);

	////	system("pause");
	////}

	///*{
	//	MYCHAR mychar = { 0 };
	//	printf("开始枚举进程\n");
	//	DWORD ret = 0, dwrite = 0;
	//	char a[] = "";
	//	printf("请输入进程名称\n");
	//	scanf_s("%s", a);
	//	printf("你输入的是%s", a);
	//	system("pause");
	//	RtlCopyMemory(mychar._char, a, strlen(a));

	//	DeviceIoControl(hdevice, BOBH_GETPROCESSID, &mychar, sizeof(mychar), &ret, sizeof(ret), &dwrite, NULL);

	//	printf("枚举进程完毕\n");

	//	printf("得到%s进程的Pid为%d\n", mychar._char, ret);
	//	system("pause");
	//}*/
	//
	//CloseHandle(hdevice);
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
