#pragma once
#include "pch.h"
#include "winioctl.h"

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

struct r3Buffer {
	ULONG64 Address;
	ULONG64 Buffer;
	ULONG64 size;
};

class MyDriver
{
public:
	MyDriver();
	~MyDriver();
	bool isInint;
	bool Inint();

	template <typename T>
	bool Read(DWORD pid, ULONG64 address, T* ret);

	template <typename T>
	bool Write(DWORD pid, ULONG64 address, T data);
private:
	HANDLE hdevice;
	BOOL installDvr(CONST wchar_t* drvPath, CONST wchar_t* serviceName);
	BOOL startDvr(CONST wchar_t* serviceName);
	BOOL stopDvr(CONST wchar_t* serviceName);
	BOOL unloadDvr(CONST wchar_t* serviceName);
};


template<typename T>
bool MyDriver::Read(DWORD pid, ULONG64 address, T* ret)
{

	DWORD PID = pid;
	DWORD lpBytesReturned;

	if (!DeviceIoControl(hdevice, BOBH_SET, &PID, sizeof(PID), &PID, sizeof(PID), &lpBytesReturned, NULL))
	{
		return false;
	}

	r3Buffer m_r3Buffer;
	m_r3Buffer.Address = address;
	m_r3Buffer.Buffer = { 0 };
	m_r3Buffer.size = sizeof(T);

	if (DeviceIoControl(hdevice, BOBH_READ, &m_r3Buffer, sizeof(m_r3Buffer), &m_r3Buffer, sizeof(m_r3Buffer), &lpBytesReturned, NULL))
	{
		*ret = (T)m_r3Buffer.Buffer;
		return true;
	}
	return false;
}

template<typename T>
bool MyDriver::Write(DWORD pid, ULONG64 address, T data)
{
	DWORD PID = pid;
	DWORD lpBytesReturned;

	if (!DeviceIoControl(hdevice, BOBH_SET, &PID, sizeof(PID), &PID, sizeof(PID), &lpBytesReturned, NULL))
	{
		return false;
	}

	r3Buffer m_r3Buffer;
	m_r3Buffer.Address = address;
	m_r3Buffer.Buffer = (ULONG64)data;
	m_r3Buffer.size = sizeof(T);

	if (DeviceIoControl(hdevice, BOBH_WRITE, &m_r3Buffer, sizeof(m_r3Buffer), &m_r3Buffer, sizeof(m_r3Buffer), &lpBytesReturned, NULL))
	{
		return true;
	}
	return false;
}

