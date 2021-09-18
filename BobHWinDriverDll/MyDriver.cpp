#include "pch.h"
#include "MyDriver.h"

MyDriver::MyDriver()
{
	this->isInint = false;
	this->hdevice = NULL;
}

MyDriver::~MyDriver()
{
	CloseHandle(hdevice);
}

bool MyDriver::Inint()
{
	//安装驱动等代码

	hdevice = CreateFile(L"\\\\.\\BobHWin7ReadLink", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hdevice == INVALID_HANDLE_VALUE)
	{
		isInint = false;
		return false;
	}
	isInint = true;
	return true;
}