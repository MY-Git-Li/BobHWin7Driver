#pragma once
#include "ntifs.h"
#include "ntddk.h"
#include <windef.h>
#include "ntdef.h"

PEPROCESS LookupProcess(HANDLE Pid);
DWORD EnumProcess(STRING processName);
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