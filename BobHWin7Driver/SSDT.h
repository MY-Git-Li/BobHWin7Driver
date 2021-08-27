#pragma once
#include "ntifs.h"
#include "ntddk.h"
#include <windef.h>
#include "ntdef.h"

typedef struct _SYSTEM_SERVICE_TABLE
{
	PVOID tablebase;
	PVOID servicecountbase;
	ULONG64 numberpfservice;
	PVOID unknow;
}SYSTEM_SERVICE_TABLE,*PSYSTEM_SERVICE_TABLE;


typedef struct _LDR_DATA_TABLE_ENTRY64 {
	LIST_ENTRY64 InLoadOrderLinks;
	LIST_ENTRY64 InMemoryOrderLinks;
	LIST_ENTRY64 InInitializationOrderLinks;
	PVOID64 DllBase;
	PVOID64 EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	PVOID SectionPointer;
	ULONG CheckSum;
	ULONG TimeDateStamp;
	PVOID LoadedImports;
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
	PVOID ContextInformation;
	PVOID OriginalBase;
	LARGE_INTEGER LoadTime;
} LDR_DATA_TABLE_ENTRY64, * PLDR_DATA_TABLE_ENTRY64;

//得到函数在SSDT中的序列号
ULONG GetSSDTFunIndex(PUCHAR funname);
//得到在SSDT中序列号为index的地址
ULONG64 GetSSDTAddr(PDRIVER_OBJECT pdriver, DWORD index);
//得到在ShadowSSDT中序列号为index的地址
ULONG64 GetShadowSSDTAddr(PDRIVER_OBJECT pdriver, DWORD index);