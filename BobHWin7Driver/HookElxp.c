#include "HookElxp.h"
#include "DeiverDefFun.h"


PVOID S_OpenProcess = NULL;
PVOID S_ReadVirtualMemory = NULL;
UINT64 SSDT_ReadVirtualMemory = NULL;
UINT64 SSDT_OpenProcess = NULL;


NTSTATUS __fastcall MyReadVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress,
	PVOID Buffer, ULONG BufferLength,
	PULONG ReturnLength)
{
	DbgPrint("ProcessHandle---<%d>  BaseAddress---<%p> ", ProcessHandle, BaseAddress);

	pMyReadVirtualMemory pTypeAdd2 = (pMyReadVirtualMemory)S_ReadVirtualMemory;

	return (pTypeAdd2)(ProcessHandle, BaseAddress, Buffer, BufferLength, ReturnLength);;
}

NTSTATUS __fastcall MyOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId)
{
	/*PEPROCESS process = 0;
	if (STATUS_SUCCESS == PsLookupProcessByProcessId(ClientId->UniqueProcess, &process))
	{
		if (strcmp(PsGetProcessImageFileName(process), "NewTzmTool.exe") == 0)
		{
			KdPrint(("受保护进程：%s", PsGetProcessImageFileName(process)));
			return STATUS_PNP_INVALID_ID;
		}
	}*/
	DbgPrint("OpenProcess---PID<%d>", ClientId->UniqueProcess);

	pMyOpenProcess temp = (pMyOpenProcess)S_OpenProcess;
	return (temp)(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
}