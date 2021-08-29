#pragma once
#include "MyHead.h"
#include "MyHook.h"

typedef  NTSTATUS(*pMyReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress,
	PVOID Buffer, ULONG BufferLength, PULONG ReturnLength);

NTSTATUS __fastcall MyReadVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress,
	PVOID Buffer, ULONG BufferLength,
	PULONG ReturnLength);



typedef NTSTATUS(*pMyOpenProcess)(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);

NTSTATUS __fastcall MyOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,  PCLIENT_ID ClientId);



typedef NTSTATUS(*pNtWriteVirtualMemory)(
	HANDLE  ProcessHandle,
	PVOID                BaseAddress,
	PVOID                Buffer,
	ULONG                NumberOfBytesToWrite,
	PULONG              NumberOfBytesWritten);

NTSTATUS __fastcall MyWriteVirtualMemory(
	HANDLE               ProcessHandle,
	PVOID                BaseAddress,
	PVOID                Buffer,
	ULONG                NumberOfBytesToWrite,
	PULONG              NumberOfBytesWritten);



PVOID S_WriteVirtualMemory;
UINT64 SSDT_WriteVirtualMemory;
ULONG Head_WriteVirtualMemory;


PVOID S_OpenProcess;
UINT64 SSDT_OpenProcess;
ULONG Head_OpenProcess;

PVOID S_ReadVirtualMemory;
UINT64 SSDT_ReadVirtualMemory;
ULONG Head_ReadVirtualMemory;