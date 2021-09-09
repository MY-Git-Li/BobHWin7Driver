#include "HookElxp.h"
#include "DeiverDefFun.h"


PVOID S_OpenProcess = NULL;
PVOID S_ReadVirtualMemory = NULL;

UINT64 SSDT_ReadVirtualMemory = 0;
UINT64 SSDT_OpenProcess = 0;

PVOID S_WriteVirtualMemory = NULL;
UINT64 SSDT_WriteVirtualMemory = 0;

NTSTATUS __fastcall MyWriteVirtualMemory(
	HANDLE               ProcessHandle,
	PVOID                BaseAddress,
	PVOID                Buffer,
	ULONG                NumberOfBytesToWrite,
	PULONG              NumberOfBytesWritten) 
{
	/*DbgPrint("WriteMemory---ProcessHandle---<%d>  BaseAddress---<%p> ", ProcessHandle, BaseAddress);*/

	/*DbgPrint("写入的数据为：");*/
	KdPrint(("WriteMemory--ProcessHandle--<%d> Address--<%p> 开始", ProcessHandle, BaseAddress));
	for (BYTE i	= 0;i< NumberOfBytesToWrite;i++)
	{
		KdPrint(("ProcessHandle---<%d> Address---<%p> WriteData--<0x%.2x>", ProcessHandle,(LONG64)BaseAddress+i,((PBYTE)Buffer)[i]));
	}
	KdPrint(("WriteMemory---ProcessHandle--<%d> Address--<%p> 完成", ProcessHandle, BaseAddress));
	
	pNtWriteVirtualMemory temp =(pNtWriteVirtualMemory)S_WriteVirtualMemory;

	return temp(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten);
}

NTSTATUS __fastcall MyReadVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress,
	PVOID Buffer, ULONG BufferLength,
	PULONG ReturnLength)
{
	KdPrint(("ReadMemory--ProcessHandle--<%d> Address--<%p> BufferLength--<%ld>\n", ProcessHandle, BaseAddress, BufferLength));

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
	KdPrint(("OpenProcess---PID<%d>", ClientId->UniqueProcess));

	pMyOpenProcess temp = (pMyOpenProcess)S_OpenProcess;

	return (temp)(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
}