#include <iostream>
#include "windows.h"

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

int main()
{
	if (!InitDriver())
	{
		printf("打开设备失败\n");
	}
	printf("打开设备成功\n");



	/*printf("\n\n\n");
	printf("开始读取进程内存地址数据(整数):\n");*/
	/*{

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

	}*/

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