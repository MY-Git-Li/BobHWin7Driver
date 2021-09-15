// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "stdlib.h"
#include "windows.h"
#include <locale.h>
#include <iostream>
#include <direct.h> 
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
#define BOBH_HIDEPROCESS CTL_CODE(FILE_DEVICE_UNKNOWN,0x819,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_DELETEFILE CTL_CODE(FILE_DEVICE_UNKNOWN,0x820,METHOD_BUFFERED,FILE_ANY_ACCESS)

typedef struct ModuleBase
{
	ULONG Pid;
	CHAR ModuleName[100];
}UModuleBase, * LPModuleBase;

struct r3Buffer {
	ULONG64 Address;
	ULONG64 Buffer;
	ULONG64 size;
}appBuffer;


string ws2s(const wstring& ws)
{
	string curLocale = setlocale(LC_ALL, NULL);     //curLocale="C"
	setlocale(LC_ALL, "chs");
	const wchar_t* wcs = ws.c_str();
	size_t dByteNum = sizeof(wchar_t) * ws.size() + 1;
	//cout << "ws.size():" << ws.size() << endl;      //5：宽字符串L"ABC我们"有5个自然字符

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
	//cout << "s.size():" << s.size() << endl;         //7：多字节字符串"ABC我们"有7个字节

	wchar_t* dest = new wchar_t[charNum];
	mbstowcs_s(NULL, dest, charNum, source, _TRUNCATE);
	wstring result = dest;
	delete[] dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

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

wchar_t* c2w(char* a)
{
	int iSize;
	wchar_t* pwszUnicode;

	//返回接受字符串所需缓冲区的大小，已经包含字符结尾符'\0'
	iSize = MultiByteToWideChar(CP_ACP, 0, a, -1, NULL, 0); //iSize =wcslen(pwsUnicode)+1=6
	pwszUnicode = (wchar_t*)malloc(iSize * sizeof(wchar_t)); //不需要 pwszUnicode = (wchar_t *)malloc((iSize+1)*sizeof(wchar_t))
	MultiByteToWideChar(CP_ACP, 0, a, -1, pwszUnicode, iSize);
	return pwszUnicode;
}



typedef int(__fastcall* ASAS)(int a,int b);


VOID __fastcall   fun(int a, int b)
{

}


int main()
{
	
	/*PassPG();*/
	HANDLE hdevice = NULL;


	hdevice = CreateFile(L"\\\\.\\BobHWin7ReadLink", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	{
		if (hdevice == INVALID_HANDLE_VALUE)
		{
			printf("打开失败\n");
			system("pause");
			return 0;
		}
		printf("打开成功\n");
		system("pause");
	}
GO_St:

	printf("\n\n\n请输入功能：\n");
	printf("0.安全退出\n1.读写进程内存地址数据\n2.读取进程模块地址\n3.通过名称得到进程PID\n4.保护进程\n");
	printf("5.杀死进程（简单方式）\n6.杀死进程（复杂方式）\n7.隐藏进程\n");
	printf("8.强制删除文件\n");
	int i;
	scanf_s("%d", &i);
	switch (i)
	{
	case 1:
		goto GO_1;
		break;
	case 2:
		goto GO_2;
		break;
	case 3:
		goto GO_3;
		break;
	default:
		printf("输入错误，请从新输入\n");
		goto GO_St;
		break;
	case 4:
		goto GO_4;
		break;
	case 5:
		goto GO_5;
		break;
	case 6:
		goto GO_6;
		break;
	case 0:
		goto GO_END;
		break;
	case 7:
		goto GO_7;
		break;
	case 8:
		goto GO_8;
		break;
	}


GO_1:
	printf("\n\n\n");
	printf("开始读取进程内存地址数据:\n");
	{

		DWORD Pid, Count;

		printf("请输入设置读写的PID：\n");
		scanf_s("%d", &Pid);
		/*printf("你输入的PID为：%d\n", Pid);
		system("pause");

		printf("开始设置读写的PID\n");*/
		DeviceIoControl(hdevice, BOBH_SET, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
		printf("设置PID成功\n");
		system("pause");

		DWORD bianliang = 520;
		printf("写之前变量的地址为%llx,变量的值为%d\n", &bianliang, bianliang);


		system("pause");

		ULONG64 address = 0;
		printf("请输入读的地址为：\n");
		scanf_s("%llx", &address);
	/*	printf("你输入的地址是 %llx\n", address);*/


		appBuffer.Address = address;
		appBuffer.Buffer = { 0 };
		appBuffer.size = sizeof(DWORD);

		/*	printf("开始读内存\n");

			system("pause");*/

		DeviceIoControl(hdevice, BOBH_READ, &appBuffer, sizeof(appBuffer), &appBuffer, sizeof(appBuffer), &Count, NULL);


		printf("读到的数据为 = %d\n", appBuffer.Buffer);


		system("pause");

		printf("开始写内存\n");

		printf("请输入写地址：\n");
		scanf_s("%llx", &address);
	/*	printf("你输入的地址是 %llx\n", address);*/
		appBuffer.Address = address;

		printf("请输入写的多少：\n");
		scanf_s("%d", &appBuffer.Buffer);

		appBuffer.size = sizeof(DWORD);

		system("pause");

		DeviceIoControl(hdevice, BOBH_WRITE, &appBuffer, sizeof(appBuffer), &appBuffer, sizeof(appBuffer), &Count, NULL);

		printf("写之后变量的地址为%x,变量的值为%d\n", &bianliang, bianliang);
		system("pause");

	}
	goto GO_St;

GO_4:
	printf("\n\n\n");
	printf("开始保护进程:\n");
	{
		DWORD Pid, Count;

	/*	printf("开始保护进程\n");*/

		printf("请输入要保护的PID:\n");
		scanf_s("%d", &Pid);
		/*	printf("你输入的的PID为：%d\n", Pid);
			system("pause");*/

		DeviceIoControl(hdevice, BOBH_PROTECT, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
		printf("保护进程完成\n");
		system("pause");

		/*printf("关闭保护进程\n");*/
		DeviceIoControl(hdevice, BOBH_UNPROTECT, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
		printf("关闭进程成功\n");
		system("pause");
	}
	goto GO_St;



GO_2:
	printf("\n\n\n");
	printf("开始读取进程模块地址:\n");
	
	{
		UModuleBase iostruct = { 0 };

		ULONG Pid = 0;

		printf("输入目标进程PID:\n");

		scanf_s("%d", &Pid);

		iostruct.Pid = Pid;

		printf("输入目标进程Dll模块名称:\n");


		string a;
		cin >> a;
		wstring b = s2ws(a);

		/*cout << "普通字符串为" << &b[0] << endl;

		cout <<"普通字符串为" << a << endl;

		wcout << L"宽字符为" << b << endl;*/

	/*	system("pause");*/
		//这里的宽字符太傻逼了
		//KERNELBASE.DLL

		/*RtlCopyMemory(iostruct.ModuleName, b.c_str(), b.size()*2);*/

		RtlCopyMemory(iostruct.ModuleName, a.c_str(), a.size());

		DWORD Count = 0;
		ULONG64 dllbase = 0;
		DeviceIoControl(hdevice, BOBH_GETMODULEADDRESS, &iostruct, sizeof(iostruct), &dllbase, sizeof(dllbase), &Count, NULL);
		std::cout << a;
		printf(" Dll base %llx\n", dllbase);

		system("pause");
	}
	goto GO_St;

GO_3:
	printf("\n\n\n");
	printf("开始通过名称得到进程PID:\n");
	{
	
		printf("开始枚举进程\n");
		DWORD ret = 0, dwrite = 0;
		std::string a;
		printf("请输入进程名称:\n");
		std::cin >> a;
	/*	std::cout << "你输入的是："<< a << std::endl;*/
		
		/*DeviceIoControl(hdevice, BOBH_GETPROCESSID, &mychar, sizeof(mychar), &ret, sizeof(ret), &dwrite, NULL);*/
		DeviceIoControl(hdevice, BOBH_GETPROCESSID, (LPVOID)a.c_str(), a.size(), &ret, sizeof(ret), &dwrite, NULL);
		printf("枚举进程完毕\n");

		printf("得到%s进程的Pid为%d\n", a.c_str(), ret);
		system("pause");
	}
	goto GO_St;

GO_5:
	printf("\n\n\n");
	printf("开始简单杀死进程:\n");
	{
		DWORD Pid, Count;

		/*	printf("开始保护进程\n");*/

		printf("请输入要杀死的进程PID:\n");
		scanf_s("%d", &Pid);
		/*	printf("你输入的的PID为：%d\n", Pid);
			system("pause");*/

		DeviceIoControl(hdevice, BOBH_KILLPROCESS_DIRECT, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
		printf("杀死进程完成\n");
		system("pause");

	}
	goto GO_St;

GO_6:
	printf("\n\n\n");
	printf("开始复杂杀死进程:\n");
	{
		DWORD Pid, Count;

		/*	printf("开始保护进程\n");*/

		printf("请输入要杀死的进程PID:\n");
		scanf_s("%d", &Pid);
		/*	printf("你输入的的PID为：%d\n", Pid);
			system("pause");*/

		DeviceIoControl(hdevice, BOBH_KILLPROCESS_MEMORY, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
		printf("杀死进程完成\n");
		system("pause");

	}
	goto GO_St;

GO_7:
	printf("\n\n\n");
	printf("开始隐藏进程:\n");
	{
		DWORD Pid, Count;

		/*	printf("开始保护进程\n");*/

		printf("请输入要隐藏进程PID:\n");
		scanf_s("%d", &Pid);
		/*	printf("你输入的的PID为：%d\n", Pid);
			system("pause");*/

		DeviceIoControl(hdevice, BOBH_HIDEPROCESS, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
		printf("隐藏进程完成\n");
		system("pause");

	}
	goto GO_St;


GO_8:
	printf("\n\n\n");
	printf("开始强制删除文件----以本程序为例:\n");
	{
		char cons[50] = "\\??\\";
	/*	char a[50] = "\\??\\C:\\Users\\Adminis\\Desktop\\Test.exe";*/
		
		DWORD Count;
		

		wchar_t a[200];
		//得到当前文件路径名
		GetModuleFileName(NULL, a, 200);

		printf("当前文件路径为：\n");

		char* b = w2c(a);
		
		strcat_s(cons, b);

		printf("%s\n", cons);

		/*	_getcwd(a,sizeof(a));
			printf("a = %s\n", a);*/
		system("pause");
		DeviceIoControl(hdevice, BOBH_DELETEFILE, cons, sizeof(cons), &a, sizeof(a), &Count, NULL);
		printf("强制删除文件完成\n");
		system("pause");

	}
	goto GO_St;





GO_END:
	CloseHandle(hdevice);
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
