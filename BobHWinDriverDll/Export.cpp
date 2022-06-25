#include "Export.h"
#include "pch.h"
#include "global.h"
#include <stdlib.h>

void ftoc(float fvalue, unsigned char* arr)
{
	unsigned char* pf;
	unsigned char* px;
	unsigned char i;   //计数器 
	pf = (unsigned char*)&fvalue;            /*unsigned char型指针取得浮点数的首地址*/
	px = arr;                               /*字符数组arr准备存储浮点数的四个字节,px指针指向字节数组arr*/

	for (i = 0; i < 4; i++)
	{
		*(px + i) = *(pf + i);     /*使用unsigned char型指针从低地址一个字节一个字节取出*/
	}
}
float ByteToFloat(unsigned char* byteArray)
{
	return *((float*)byteArray);
}

void dtoc(double dvalue, unsigned char* arr)
{
	unsigned char* pf;
	unsigned char* px;
	unsigned char i;   //计数器 
	pf = (unsigned char*)&dvalue;            /*unsigned char型指针取得浮点数的首地址*/
	px = arr;                               /*字符数组arr准备存储浮点数的四个字节,px指针指向字节数组arr*/

	for (i = 0; i < 8; i++)
	{
		*(px + i) = *(pf + i);     /*使用unsigned char型指针从低地址一个字节一个字节取出*/
	}
}

double ByteToDouble(unsigned char* byteArray)
{
	return *((double*)byteArray);
}

extern "C" __declspec(dllexport) char* w2c(wchar_t* a)
{
	char* pszMultiByte;
	int iSize;
	//返回接受字符串所需缓冲区的大小，已经包含字符结尾符'\0'
	iSize = WideCharToMultiByte(CP_ACP, 0, a, -1, NULL, 0, NULL, NULL); //iSize =wcslen(pwsUnicode)+1=6
	pszMultiByte = (char*)malloc(iSize * sizeof(char)); //不需要 pszMultiByte = (char*)malloc(iSize*sizeof(char)+1);
	WideCharToMultiByte(CP_ACP, 0, a, -1, pszMultiByte, iSize, NULL, NULL);
	return pszMultiByte;
}

extern "C" __declspec(dllexport) wchar_t* c2w(char* a)
{
	int iSize;
	wchar_t* pwszUnicode;

	//返回接受字符串所需缓冲区的大小，已经包含字符结尾符'\0'
	iSize = MultiByteToWideChar(CP_ACP, 0, a, -1, NULL, 0); //iSize =wcslen(pwsUnicode)+1=6
	pwszUnicode = (wchar_t*)malloc(iSize * sizeof(wchar_t)); //不需要 pwszUnicode = (wchar_t *)malloc((iSize+1)*sizeof(wchar_t))
	MultiByteToWideChar(CP_ACP, 0, a, -1, pwszUnicode, iSize);
	return pwszUnicode;
}


extern "C" __declspec(dllexport) bool InitDriver()
{
	return myDriver->Inint();
}

bool UnloadDriver()
{
	return myDriver->UnLoad();
}


extern "C" __declspec(dllexport) void ReadMemoryDWORD(DWORD pid, ULONG64 addre, DWORD * ret)
{
	if (myDriver->isInint)
	{
		myDriver->Read(pid, addre, ret);
	}
}

extern "C" __declspec(dllexport) void ReadMemoryDWORD64(DWORD pid, ULONG64 addre, DWORD64 * ret)
{
	if (myDriver->isInint)
	{
		myDriver->Read(pid, addre, ret);
	}
}

extern "C" __declspec(dllexport) void ReadMemoryBytes(DWORD pid, ULONG64 addre, BYTE * *ret, DWORD sizes)
{
	BYTE* buff = (BYTE*)malloc(sizes);
	for (int i = 0; i < sizes; i++)
	{
		myDriver->Read(pid, addre + i, &buff[i]);
	}
	memcpy(*ret, buff, sizes);
	free(buff);
}

extern "C" __declspec(dllexport) void ReadMemoryFloat(DWORD pid, ULONG64 addre, float* ret)
{
	BYTE buff[4] = { 0 };
	BYTE* buffpyr = buff;
	ReadMemoryBytes(pid, addre, &buffpyr, sizeof(buff));

	*ret = *((float*)buffpyr);

}

extern "C" __declspec(dllexport) void ReadMemoryDouble(DWORD pid, ULONG64 addre, double* ret)
{
	BYTE buff[8] = { 0 };
	BYTE* buffpyr = buff;
	ReadMemoryBytes(pid, addre, &buffpyr, sizeof(buff));

	*ret = *((double*)buffpyr);
}

extern "C" __declspec(dllexport) void WriteMemoryBytes(DWORD pid, ULONG64 addre, BYTE * data, DWORD sizes)
{
	for (int i = 0; i < sizes; i++)
	{
		myDriver->Write(pid, addre + i, data[i]);
	}
}

extern "C" __declspec(dllexport) void WriteMemoryDWORD(DWORD pid, ULONG64 addre, DWORD ret)
{
	if (myDriver->isInint)
	{
		myDriver->Write(pid, addre, ret);
	}
}

extern "C" __declspec(dllexport) void WriteMemoryDWORD64(DWORD pid, ULONG64 addre, DWORD64 ret)
{
	if (myDriver->isInint)
	{
		myDriver->Write(pid, addre, ret);
	}
}

extern "C" __declspec(dllexport) void WriteMemoryDouble(DWORD pid, ULONG64 addre, double ret)
{
	BYTE buff[8] = { 0 };
	dtoc(ret, buff);
	WriteMemoryBytes(pid, addre, buff, sizeof(buff));
}

extern "C" __declspec(dllexport) void WriteMemoryFloat(DWORD pid, ULONG64 addre, float ret)
{
	BYTE buff[4] = { 0 };
	ftoc(ret, buff);
	WriteMemoryBytes(pid, addre, buff, sizeof(buff));
}

extern "C" __declspec(dllexport) void ForceDeleteFile(const char* path)
{
	if (myDriver->isInint)
	{
		myDriver->ForceDeleteFile(path);
	}
}

extern "C" __declspec(dllexport) DWORD GetPidByProcessName(const char* name)
{
	DWORD pid = 0;
	if (myDriver->isInint)
	{
		pid = myDriver->GetPidByProcessName(name);
	}
	return pid;
}

extern "C" __declspec(dllexport) void ProtectProcess(DWORD pid)
{
	if (myDriver->isInint)
	{
		myDriver->ProtectProcess(pid);
	}
}

extern "C" __declspec(dllexport) void StopProtectProcess()
{
	if (myDriver->isInint)
	{
		myDriver->StopProtectProcess();
	}
}

extern "C" __declspec(dllexport) ULONG64 GetModuleBaseAddress(DWORD pid, const char* name)
{
	ULONG64 address = 0;
	if (myDriver->isInint)
	{
		address = myDriver->GetModuleBaseAddress(pid, name);
	}
	return address;
}

extern "C" __declspec(dllexport) void HideProcess(DWORD pid)
{
	if (myDriver->isInint)
	{
		myDriver->HideProcess(pid);
	}
}