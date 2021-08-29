#pragma once
#include "MyHead.h"
#include "MyHook.h"

typedef  NTSTATUS(*pMyReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress,
	PVOID Buffer, ULONG BufferLength, PULONG ReturnLength);

typedef NTSTATUS(*pMyOpenProcess)(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);


NTSTATUS __fastcall MyOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,  PCLIENT_ID ClientId);

NTSTATUS __fastcall MyReadVirtualMemory( HANDLE ProcessHandle, PVOID BaseAddress,
	 PVOID Buffer, ULONG BufferLength,
	PULONG ReturnLength);


PVOID S_OpenProcess;
PVOID S_ReadVirtualMemory;
UINT64 SSDT_ReadVirtualMemory;
UINT64 SSDT_OpenProcess;