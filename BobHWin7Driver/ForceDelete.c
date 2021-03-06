#include "ForceDelete.h"

BOOLEAN Delete_File_Mode1(UNICODE_STRING uPath)
{
	HANDLE fileHandle;
	NTSTATUS result;
	IO_STATUS_BLOCK ioBlock;
	DEVICE_OBJECT* device_object = NULL;
	void* object = NULL;
	OBJECT_ATTRIBUTES fileObject;
	/*UNICODE_STRING uPath;*/

	PEPROCESS eproc = IoGetCurrentProcess();
	//switch context to UserMode
	KeAttachProcess(eproc);

	/*RtlInitUnicodeString(&uPath, path);*/

	InitializeObjectAttributes(&fileObject,
		&uPath,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	result = IoCreateFileSpecifyDeviceObjectHint(
		&fileHandle,
		SYNCHRONIZE | FILE_WRITE_ATTRIBUTES | FILE_READ_ATTRIBUTES | FILE_READ_DATA, //0x100181 
		&fileObject,
		&ioBlock,
		0,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, //FILE_SHARE_VALID_FLAGS,
		FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,//0x60,
		0,
		0,
		CreateFileTypeNone,
		0,
		IO_IGNORE_SHARE_ACCESS_CHECK,
		device_object);
	if (result != STATUS_SUCCESS)
	{
		KdPrint(("error in IoCreateFileSpecifyDeviceObjectHint"));
		goto _Error;
	}

	result = ObReferenceObjectByHandle(fileHandle, 0, 0, 0, &object, 0);

	if (result != STATUS_SUCCESS)
	{
		KdPrint(("error in ObReferenceObjectByHandle"));
		ZwClose(fileHandle);
		goto _Error;
	}

	/*
	METHOD 1
	*/
	((FILE_OBJECT*)object)->SectionObjectPointer->ImageSectionObject = 0;
	((FILE_OBJECT*)object)->DeleteAccess = 1;
	result = ZwDeleteFile(&fileObject);

	ObDereferenceObject(object);
	ZwClose(fileHandle);

	if (result != STATUS_SUCCESS)
	{
		KdPrint(("\n[+]error in ZwDeleteFile"));
		goto _Error;
	}

	result = ZwDeleteFile(&fileObject);
	if (NT_SUCCESS(result))
	{
		KeDetachProcess();
		return TRUE;
	}
_Error:
	KeDetachProcess();
	return FALSE;

}

BOOLEAN Unlock_File_Mode1(UNICODE_STRING uPath)
{

	HANDLE fileHandle;
	NTSTATUS result;
	IO_STATUS_BLOCK ioBlock;
	DEVICE_OBJECT* device_object = NULL;
	void* object = NULL;
	OBJECT_ATTRIBUTES fileObject;
	/*UNICODE_STRING uPath;*/

	PEPROCESS eproc = IoGetCurrentProcess();
	//switch context to UserMode
	KeAttachProcess(eproc);

	/*RtlInitUnicodeString(&uPath, path);*/

	InitializeObjectAttributes(&fileObject,
		&uPath,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	result = IoCreateFileSpecifyDeviceObjectHint(
		&fileHandle,
		SYNCHRONIZE | FILE_WRITE_ATTRIBUTES | FILE_READ_ATTRIBUTES | FILE_READ_DATA, //0x100181 
		&fileObject,
		&ioBlock,
		0,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, //FILE_SHARE_VALID_FLAGS,
		FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,//0x60,
		0,
		0,
		CreateFileTypeNone,
		0,
		IO_IGNORE_SHARE_ACCESS_CHECK,
		device_object);
	if (result != STATUS_SUCCESS)
	{
		KdPrint(("error in IoCreateFileSpecifyDeviceObjectHint"));
		goto _Error;
	}

	result = ObReferenceObjectByHandle(fileHandle, 0, 0, 0, &object, 0);

	if (result != STATUS_SUCCESS)
	{
		KdPrint(("error in ObReferenceObjectByHandle"));
		ZwClose(fileHandle);
		goto _Error;
	}

	/*
	METHOD 1
	*/
	((FILE_OBJECT*)object)->SectionObjectPointer->ImageSectionObject = 0;
	((FILE_OBJECT*)object)->DeleteAccess = 1;
	ObDereferenceObject(object);
	ZwClose(fileHandle);

	KeDetachProcess();
	return TRUE;
_Error:
	KeDetachProcess();
	return FALSE;

}
