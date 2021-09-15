#include "Driverdef.h"
#include "DeiverDefFun.h"
#include "SSDT.h"
#include "HookElxp.h"
#include "LDE64x64.h"
#include "ForceDelete.h"

//得到系统版本号
VOID GetVersion()
{
	/*ULONG NtBuildNumber;*/
	RTL_OSVERSIONINFOW osi;
	osi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	RtlFillMemory(&osi, sizeof(RTL_OSVERSIONINFOW), 0);
	RtlGetVersion(&osi);
	KdPrint(("当前系统版本:%ld.%ld.%ld\n", osi.dwMajorVersion,osi.dwMinorVersion,osi.dwBuildNumber));
	//return NtBuildNumber;
}
NTSTATUS DispatchDevCTL(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);//取得IRP 对象
	PVOID buffer = Irp->AssociatedIrp.SystemBuffer;//获给的缓存区
	ULONG CTLcode = irpsp->Parameters.DeviceIoControl.IoControlCode;//得到自定义的控制码
	ULONG uInSize = irpsp->Parameters.DeviceIoControl.InputBufferLength;//输入的长度
	ULONG uOutSize = irpsp->Parameters.DeviceIoControl.OutputBufferLength;//输出的长度
	PVOID tmpbuffer = NULL;
	switch (CTLcode)
	{
	case BOBH_READ:
		RtlCopyMemory(&appBuffer, buffer, uInSize);
		KdPrint(("读收到的地址是:%x \r\n", appBuffer.Address));
		tmpbuffer = ExAllocatePool(NonPagedPool, appBuffer.size + 1);
		RtlFillMemory(tmpbuffer, appBuffer.size + 1, 0);
		KdPrint(("tmpbuffer地址是%x 内容为%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));
		KeReadProcessMemory(appBuffer.Address, tmpbuffer, appBuffer.size);
		KdPrint(("tmpbuffer地址是%x 内容为%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));
		RtlCopyMemory(&appBuffer.Buffer, tmpbuffer, sizeof(tmpbuffer));
		RtlCopyMemory(buffer, &appBuffer, uInSize);
		ExFreePool(tmpbuffer);
		status = STATUS_SUCCESS;
		break;
	case BOBH_WRITE:
		RtlCopyMemory(&appBuffer, buffer, uInSize);
		KdPrint(("写收到的地址是:%x \r\n", appBuffer.Address));

		tmpbuffer = ExAllocatePool(NonPagedPool, appBuffer.size + 1);
		RtlFillMemory(tmpbuffer, appBuffer.size + 1, 0);
		KdPrint(("tmpbuffer地址是%x 内容为%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));

		RtlCopyMemory(tmpbuffer, &appBuffer.Buffer, appBuffer.size);
		KdPrint(("tmpbuffer地址是%x 内容为%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));

		KeWriteProcessMemory(appBuffer.Address, tmpbuffer, appBuffer.size);

		ExFreePool(tmpbuffer);
		status = STATUS_SUCCESS;

		break;
	case BOBH_SET:
	{
		DWORD PID;
		RtlCopyMemory(&PID, buffer, uInSize);
		status = SetPID(PID);
		/*status = STATUS_SUCCESS;*/
		break;
	}
	case BOBH_PROTECT:
	{
		DWORD PID;
		RtlCopyMemory(&PID, buffer, uInSize);
		status = ProtectProcessStart(PID);
	/*	status = STATUS_SUCCESS;*/
		break;
	}
	case BOBH_UNPROTECT:
	{
		status = ProtectProcessStop();
	/*	status = STATUS_SUCCESS;*/
		break;
	}
	case BOBH_KILLPROCESS_DIRECT:
	{
		DWORD PID;
		RtlCopyMemory(&PID, buffer, uInSize);
		status =	KeKillProcessSimple(PID);
		/*status = STATUS_SUCCESS;*/
		break;
	}
	case BOBH_KILLPROCESS_MEMORY:
	{
		DWORD PID;
		RtlCopyMemory(&PID, buffer, uInSize);
		if (KeKillProcessZeroMemory(PID))
		{
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}
		
		break;
	}
	case BOBH_GETMODULEADDRESS:
	{
		LPModuleBase TempBase = (LPModuleBase)buffer;
		if (TempBase->Pid <= 0)
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		STRING moudlenname_ansi = { 0 };

		UNICODE_STRING moudlename = { 0 };
		ULONGLONG outdllbase = 0;


		RtlInitString(&moudlenname_ansi, TempBase->ModuleName);

		RtlAnsiStringToUnicodeString(&moudlename, &moudlenname_ansi, TRUE);
		

		/*RtlInitUnicodeString(&moudlename, TempBase->ModuleName);*/
		KdPrint(("要寻找的moudlename为%wZ \r\n", moudlename));

		outdllbase = KeGetMoudleAddress(TempBase->Pid, &moudlename);

		*(PULONGLONG)buffer = outdllbase;

		status = STATUS_SUCCESS;
		break;
	}
	case BOBH_GETPROCESSID:
	{

		PUCHAR a = ExAllocatePool(NonPagedPool,uInSize);
	
		RtlCopyMemory(a, buffer, uInSize);

		STRING process = { 0 };

		RtlInitString(&process, a);
		KdPrint(("process %Z \r\n", process));
		

		DWORD pid = 0;
		pid = GetPidByEnumProcess(process);
		*(PDWORD)buffer = pid;

		if (pid > 0)
		{
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}

		ExFreePool(a);

		break;
	}
	case BOBH_HIDEPROCESS:
	{
		DWORD PID;
		RtlCopyMemory(&PID, buffer, uInSize);
		status = HideProcess(PID);
		break;
	}
	case BOBH_DELETEFILE:
	{
		PUCHAR path = ExAllocatePool(NonPagedPool, uInSize);
		RtlCopyMemory(path, buffer, uInSize);
		KdPrint(("要删除的文件路径为:%s", path));
		
		STRING a_ansi = { 0 };
		UNICODE_STRING a_unicode = { 0 };

		RtlInitString(&a_ansi, path);
		RtlAnsiStringToUnicodeString(&a_unicode, &a_ansi, TRUE);


		if (Delete_File_Mode1(a_unicode))
		{
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}
		
		break;
	}


	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	Irp->IoStatus.Information = uOutSize;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	NTSTATUS status;
	int i;
	//设置驱动卸载事件
	DriverObject->DriverUnload = Unload;
	//创建设备对象
	status = IoCreateDevice(DriverObject, 0, &myDeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("[BobHWin7]创建设备对象失败 \r\n"));
		return status;
	}
	//创建符号链接
	status = IoCreateSymbolicLink(&symLinkName, &myDeviceName);
	if (!NT_SUCCESS(status)) {
		KdPrint(("[BobHWin7]创建符号链接失败 \r\n"));
		IoDeleteDevice(DeviceObject);
		return status;
	}
	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = DispatchPassThru;
	}
	//为读写专门指定处理函数
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDevCTL;
	//监控进程对象
	KdPrint(("[BobHWin7]成功载入驱动，开始LDR \r\n"));
	PLDR_DATA_TABLE_ENTRY ldr;
	ldr = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	ldr->Flags |= 0x20;
	KdPrint(("[BobHWin7]LDR修改成功 \r\n"));
	



	GetVersion();

	//KdPrint(("开始HOOK"));

	//LDE_init();

	//PSYSTEM_SERVICE_TABLE service = GetSystemServiceTable_Generalmethod(DriverObject);

	//
	//SSDT_OpenProcess = GetSSDTAddr(service, GetSSDTFunIndex("NtOpenProcess"));

	//Head_OpenProcess = GetPatchSize(SSDT_OpenProcess);

	//KdPrint(("NtOpenProcess head--<%d>",Head_OpenProcess));

	//StartHOOK((UINT64)SSDT_OpenProcess, (UINT64)&MyOpenProcess,/*(20)*/(USHORT)Head_OpenProcess, &S_OpenProcess);




	//SSDT_ReadVirtualMemory = GetSSDTAddr(service, GetSSDTFunIndex("NtReadVirtualMemory"));

	//Head_ReadVirtualMemory = GetPatchSize(SSDT_ReadVirtualMemory);

	//KdPrint(("NtReadVirtualMemory head--<%d>", Head_ReadVirtualMemory));

	//StartHOOK((UINT64)SSDT_ReadVirtualMemory, (UINT64)&MyReadVirtualMemory, /*(15)*/(USHORT)Head_ReadVirtualMemory, &S_ReadVirtualMemory);




	//SSDT_WriteVirtualMemory = GetSSDTAddr(service, GetSSDTFunIndex("NtWriteVirtualMemory"));

	//Head_WriteVirtualMemory = GetPatchSize(SSDT_WriteVirtualMemory);

	//KdPrint(("NtWriteVirtualMemory head--<%d>", Head_WriteVirtualMemory));

	//StartHOOK((UINT64)SSDT_WriteVirtualMemory, (UINT64)&MyWriteVirtualMemory,(USHORT)Head_WriteVirtualMemory, &S_WriteVirtualMemory);


	//KdPrint(("HOOK完成"));

	return status;
}

VOID Unload(PDRIVER_OBJECT DriverObject) {
	if (isProtecting) {
		ObUnRegisterCallbacks(g_pRegiHandle);
	}
	

	//RecoveryHOOK(SSDT_ReadVirtualMemory, /*15*/Head_ReadVirtualMemory, S_ReadVirtualMemory);

	//RecoveryHOOK(SSDT_OpenProcess,/* 20*/Head_OpenProcess, S_OpenProcess);

	//RecoveryHOOK(SSDT_WriteVirtualMemory, Head_WriteVirtualMemory, S_WriteVirtualMemory);

	/*LDE_End();*/

	IoDeleteSymbolicLink(&symLinkName);
	IoDeleteDevice(DeviceObject);

	
	KdPrint(("[BobHWin7]成功卸载驱动 \r\n"));
}





