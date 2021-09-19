#pragma once
#include "windows.h"

void ftoc(float fvalue, unsigned char* arr);

float ByteToFloat(unsigned char* byteArray);

void dtoc(double dvalue, unsigned char* arr);


float ByteToDouble(unsigned char* byteArray);


extern "C" __declspec(dllexport) bool InitDriver();

extern "C" __declspec(dllexport) void ReadMemoryDWORD(DWORD pid, ULONG64 addre, DWORD * ret);


extern "C" __declspec(dllexport) void ReadMemoryDWORD64(DWORD pid, ULONG64 addre, DWORD64 * ret);


extern "C" __declspec(dllexport) void ReadMemoryBytes(DWORD pid, ULONG64 addre, BYTE * *ret, DWORD sizes);


extern "C" __declspec(dllexport) void ReadMemoryFloat(DWORD pid, ULONG64 addre, float* ret);


extern "C" __declspec(dllexport) void ReadMemoryDouble(DWORD pid, ULONG64 addre, double* ret);


extern "C" __declspec(dllexport) void WriteMemoryBytes(DWORD pid, ULONG64 addre, BYTE * data, DWORD sizes);


extern "C" __declspec(dllexport) void WriteMemoryDWORD(DWORD pid, ULONG64 addre, DWORD ret);


extern "C" __declspec(dllexport) void WriteMemoryDWORD64(DWORD pid, ULONG64 addre, DWORD64 ret);


extern "C" __declspec(dllexport) void WriteMemoryDouble(DWORD pid, ULONG64 addre, double ret);


extern "C" __declspec(dllexport) void WriteMemoryFloat(DWORD pid, ULONG64 addre, float ret);


extern "C" __declspec(dllexport) void ForceDeleteFile(const char* path);

extern "C" __declspec(dllexport) DWORD GetPidByProcessName(const char* name);

extern "C" __declspec(dllexport) void ProtectProcess(DWORD pid);

extern "C" __declspec(dllexport) void StopProtectProcess();

extern "C" __declspec(dllexport) ULONG64 GetModuleBaseAddress(DWORD pid, const char* name);