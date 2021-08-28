//#pragma once
//#include "ntifs.h"
//#include "ntddk.h"
//#include <windef.h>
//#include "ntdef.h"
// 
// 
// 想使用infinityhook，但是测试无用，不知如何解决。。。
// 
// 
//#define kprintf(...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, __VA_ARGS__)
//
//
//void __fastcall SyscallStub(
//	_In_ unsigned int SystemCallIndex,
//	_Inout_ void** SystemCallFunction);
//
//typedef NTSTATUS(*NtOpenProcess_t)(
//	_Out_ PHANDLE             ProcessHandle,
//	_In_ ACCESS_MASK          DesiredAccess,
//	_In_ POBJECT_ATTRIBUTES   ObjectAttributes,
//	_In_ PCLIENT_ID           ClientId);
//
//NTSTATUS DetourNtOpenProcess(
//	_Out_ PHANDLE             ProcessHandle,
//	_In_ ACCESS_MASK          DesiredAccess,
//	_In_ POBJECT_ATTRIBUTES   ObjectAttributes,
//	_In_ PCLIENT_ID           ClientId);
//
//NTSTATUS DetourNtCreateFile(
//	_Out_ PHANDLE FileHandle,
//	_In_ ACCESS_MASK DesiredAccess,
//	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
//	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
//	_In_opt_ PLARGE_INTEGER AllocationSize,
//	_In_ ULONG FileAttributes,
//	_In_ ULONG ShareAccess,
//	_In_ ULONG CreateDisposition,
//	_In_ ULONG CreateOptions,
//	_In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
//	_In_ ULONG EaLength);
//
//typedef NTSTATUS(*NtCreateFile_t)(
//	_Out_ PHANDLE FileHandle,
//	_In_ ACCESS_MASK DesiredAccess,
//	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
//	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
//	_In_opt_ PLARGE_INTEGER AllocationSize,
//	_In_ ULONG FileAttributes,
//	_In_ ULONG ShareAccess,
//	_In_ ULONG CreateDisposition,
//	_In_ ULONG CreateOptions,
//	_In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
//	_In_ ULONG EaLength);
//
//static wchar_t IfhMagicFileName[] = L"ifh--";
//
//static NtCreateFile_t OriginalNtCreateFile = NULL;
//static NtOpenProcess_t OriginalNtOpenProcess = NULL;
