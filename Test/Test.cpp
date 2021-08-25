// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "stdlib.h"
#include "windows.h"

#define BOBH_SET CTL_CODE(FILE_DEVICE_UNKNOWN,0x810,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_READ CTL_CODE(FILE_DEVICE_UNKNOWN,0x811,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN,0x812,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_PROTECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x813,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_UNPROTECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x814,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_KILLPROCESS_DIRECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x815,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define BOBH_KILLPROCESS_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN,0x816,METHOD_BUFFERED,FILE_ANY_ACCESS)

struct r3Buffer {
	ULONG64 Address;
	ULONG64 Buffer;
	ULONG64 size;
}appBuffer;

int main()
{
	HANDLE hdevice = NULL;


	hdevice = CreateFile(L"\\\\.\\BobHWin7ReadLink", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);

	if (hdevice == INVALID_HANDLE_VALUE)
	{
		printf("打开失败\n");
		system("pause");
		return 0;
	}
	printf("打开成功\n");
	system("pause");

	DWORD Pid, Count;
	
	printf("请输入PID\n");
	scanf_s("%d", &Pid);
	printf("你输入的的PID为：%d\n",Pid);
	system("pause");

	printf("开始设置PID\n");
	DeviceIoControl(hdevice, BOBH_SET, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
	printf("设置PID成功\n");
	system("pause");

	DWORD bianliang = 520;
	printf("写之前变量的地址为%x,变量的值为%d\n",&bianliang,bianliang);

	
	system("pause");

	appBuffer.Address = (ULONG64)&bianliang;
	appBuffer.Buffer = { 0 };
	appBuffer.size = sizeof(appBuffer);

	printf("开始读内存\n");


	printf("读之前的appBuffer.Address = %x\n", appBuffer.Address);
	printf("读之前的appBuffer.Buffer = %d\n", appBuffer.Buffer);
	printf("读之前的appBuffer.size = %d\n", appBuffer.size);
	system("pause");

	DeviceIoControl(hdevice, BOBH_READ, &appBuffer, sizeof(appBuffer), &appBuffer, sizeof(appBuffer), &Count, NULL);

	printf("读之后的appBuffer.Address = %x\n", appBuffer.Address);
	printf("读之后的appBuffer.Buffer = %d\n", appBuffer.Buffer);
	printf("读之后的appBuffer.size = %d\n", appBuffer.size);

	system("pause");

	printf("开始写内存\n");
	printf("请输入写的多少\n");
	scanf_s("%d", &appBuffer.Buffer);

	appBuffer.size = sizeof(appBuffer);

	printf("写之前的appBuffer.Address = %x\n", appBuffer.Address);
	printf("写之前的appBuffer.Buffer = %d\n", appBuffer.Buffer);
	printf("写之前的appBuffer.size = %d\n", appBuffer.size);
	system("pause");

	DeviceIoControl(hdevice, BOBH_WRITE, &appBuffer, sizeof(appBuffer), &appBuffer, sizeof(appBuffer), &Count, NULL);

	printf("写之后变量的地址为%x,变量的值为%d\n", &bianliang, bianliang);
	system("pause");


	printf("开始保护进程\n");

	printf("请输入要保护的PID\n");
	scanf_s("%d", &Pid);
	printf("你输入的的PID为：%d\n", Pid);
	system("pause");

	DeviceIoControl(hdevice, BOBH_PROTECT, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
	printf("保护进程完成\n");
	system("pause");

	printf("关闭保护进程\n");
	DeviceIoControl(hdevice, BOBH_UNPROTECT, &Pid, sizeof(Pid), &Pid, sizeof(Pid), &Count, NULL);
	printf("关闭进程完成\n");
	system("pause");


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
