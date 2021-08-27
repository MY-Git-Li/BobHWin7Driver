#pragma once
#include "ntifs.h"
#include "ntddk.h"
#include <windef.h>
#include "ntdef.h"


extern NTKERNELAPI PVOID PsGetProcessWow64Process(_In_ PEPROCESS Process);
extern PVOID PsGetProcessPeb(_In_ PEPROCESS Process);
extern NTKERNELAPI UCHAR* PsGetProcessImageFileName(IN PEPROCESS Process); //未公开的进行导出即可
extern NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process);//未公开进行导出 


PEPROCESS LookupProcess(HANDLE Pid);
DWORD GetPidByEnumProcess(STRING processName);
VOID Unload(PDRIVER_OBJECT DriverObject);
VOID KeReadProcessMemory(ULONG64 add, PVOID buffer, SIZE_T size);
VOID KeWriteProcessMemory(ULONG64 add, PVOID buffer, SIZE_T size);
NTSTATUS SetPID(DWORD pid);
NTSTATUS DispatchPassThru(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS KeKillProcessSimple(DWORD pid);
BOOLEAN KeKillProcessZeroMemory(DWORD pid);
OB_PREOP_CALLBACK_STATUS MyObjectPreCallback
(
	__in PVOID  RegistrationContext,
	__in POB_PRE_OPERATION_INFORMATION  pOperationInformation
);
NTSTATUS ProtectProcessStart(DWORD pid);
NTSTATUS ProtectProcessStop();
ULONGLONG KeGetMoudleAddress(_In_ ULONG pid, _In_ PUNICODE_STRING name);
NTSTATUS DispatchDevCTL(PDEVICE_OBJECT DeviceObject, PIRP Irp);
//
//关闭内存写保护的代码
//
KIRQL WPOFFx64();
//
//打开内存写保护的代码
//
void WPONx64(
	KIRQL irql);