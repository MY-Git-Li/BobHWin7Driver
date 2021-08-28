//#include "HOOK.h"
//
//void __fastcall SyscallStub(
//	_In_ unsigned int SystemCallIndex,
//	_Inout_ void** SystemCallFunction)
//{
//	// 
//	// Enabling this message gives you VERY verbose logging... and slows
//	// down the system. Use it only for debugging.
//	//
//
//
//	kprintf("[+] infinityhook: SYSCALL %lu: 0x%p [stack: 0x%p].\n", SystemCallIndex, *SystemCallFunction, SystemCallFunction);
//
//
//	UNREFERENCED_PARAMETER(SystemCallIndex);
//
//	//
//	// In our demo, we care only about nt!NtCreateFile calls.
//	//
//	if (*SystemCallFunction == OriginalNtOpenProcess)
//	{
//		//
//		// We can overwrite the return address on the stack to our detoured
//		// NtCreateFile.
//		//
//		*SystemCallFunction = DetourNtOpenProcess;
//	}
//}
//
//NTSTATUS DetourNtOpenProcess(
//	_Out_ PHANDLE             ProcessHandle,
//	_In_ ACCESS_MASK          DesiredAccess,
//	_In_ POBJECT_ATTRIBUTES   ObjectAttributes,
//	_In_ PCLIENT_ID           ClientId)
//{
//	//
//	// We're going to filter for our "magic" file name.
//	//
//	if (ClientId->UniqueProcess == (HANDLE)(3900))
//	{
//		DesiredAccess = 0x8; // PROCESS_VM_OPERATION
//	}
//
//	//
//	// We're uninterested, call the original.
//	//
//	return OriginalNtOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
//}
//
//
//
//
//
//
//
//
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
//	_In_ ULONG EaLength)
//{
//	//
//	// We're going to filter for our "magic" file name.
//	//
//	if (ObjectAttributes &&
//		ObjectAttributes->ObjectName &&
//		ObjectAttributes->ObjectName->Buffer)
//	{
//		//
//		// Unicode strings aren't guaranteed to be NULL terminated so
//		// we allocate a copy that is.
//		//
//		PWCHAR ObjectName = (PWCHAR)ExAllocatePool(NonPagedPool, ObjectAttributes->ObjectName->Length + sizeof(wchar_t));
//		if (ObjectName)
//		{
//			memset(ObjectName, 0, ObjectAttributes->ObjectName->Length + sizeof(wchar_t));
//			memcpy(ObjectName, ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length);
//
//			//
//			// Does it contain our special file name?
//			//
//			if (wcsstr(ObjectName, IfhMagicFileName))
//			{
//				kprintf("[+] infinityhook: Denying access to file: %wZ.\n", ObjectAttributes->ObjectName);
//
//				ExFreePool(ObjectName);
//
//				//
//				// The demo denies access to said file.
//				//
//				return STATUS_ACCESS_DENIED;
//			}
//
//			ExFreePool(ObjectName);
//		}
//	}
//
//	//
//	// We're uninterested, call the original.
//	//
//	return OriginalNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
//}
