#include "Driverdef.h"
#include "DeiverDefFun.h"
#include "SSDT.h"
#include "HookElxp.h"
#include "LDE64x64.h"
#include "ForceDelete.h"
#include "HttpRequest.h"

//�õ�ϵͳ�汾��
VOID GetVersion()
{
	/*ULONG NtBuildNumber;*/
	RTL_OSVERSIONINFOW osi;
	osi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	RtlFillMemory(&osi, sizeof(RTL_OSVERSIONINFOW), 0);
	RtlGetVersion(&osi);
	KdPrint(("��ǰϵͳ�汾:%ld.%ld.%ld\n", osi.dwMajorVersion,osi.dwMinorVersion,osi.dwBuildNumber));
	//return NtBuildNumber;
}
NTSTATUS DispatchDevCTL(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);//ȡ��IRP ����
	PVOID buffer = Irp->AssociatedIrp.SystemBuffer;//����Ļ�����
	ULONG CTLcode = irpsp->Parameters.DeviceIoControl.IoControlCode;//�õ��Զ���Ŀ�����
	ULONG uInSize = irpsp->Parameters.DeviceIoControl.InputBufferLength;//����ĳ���
	ULONG uOutSize = irpsp->Parameters.DeviceIoControl.OutputBufferLength;//����ĳ���
	PVOID tmpbuffer = NULL;
	switch (CTLcode)
	{
	case BOBH_READ:
		RtlCopyMemory(&appBuffer, buffer, uInSize);
		KdPrint(("���յ��ĵ�ַ��:%x \r\n", appBuffer.Address));
		tmpbuffer = ExAllocatePool(NonPagedPool, appBuffer.size + 1);
		RtlFillMemory(tmpbuffer, appBuffer.size + 1, 0);
		KdPrint(("tmpbuffer��ַ��%x ����Ϊ%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));
		KeReadProcessMemory(appBuffer.Address, tmpbuffer, appBuffer.size);
		KdPrint(("tmpbuffer��ַ��%x ����Ϊ%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));
		RtlCopyMemory(&appBuffer.Buffer, tmpbuffer, sizeof(tmpbuffer));
		RtlCopyMemory(buffer, &appBuffer, uInSize);
		ExFreePool(tmpbuffer);
		status = STATUS_SUCCESS;
		break;
	case BOBH_WRITE:
		RtlCopyMemory(&appBuffer, buffer, uInSize);
		KdPrint(("д�յ��ĵ�ַ��:%x \r\n", appBuffer.Address));

		tmpbuffer = ExAllocatePool(NonPagedPool, appBuffer.size + 1);
		RtlFillMemory(tmpbuffer, appBuffer.size + 1, 0);
		KdPrint(("tmpbuffer��ַ��%x ����Ϊ%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));

		RtlCopyMemory(tmpbuffer, &appBuffer.Buffer, appBuffer.size);
		KdPrint(("tmpbuffer��ַ��%x ����Ϊ%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));

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
		KdPrint(("ҪѰ�ҵ�moudlenameΪ%wZ \r\n", moudlename));

		outdllbase = KeGetMoudleAddress(TempBase->Pid, &moudlename);

		*(PULONGLONG)buffer = outdllbase;

		status = STATUS_SUCCESS;
		break;
	}
	case BOBH_GETPROCESSID:
	{
		PUCHAR a = NULL;
		if (uInSize>=14)
		{
			a = ExAllocatePool(NonPagedPool, 14);

			RtlCopyMemory(a, buffer, 14);
		}
		else
		{
			a = ExAllocatePool(NonPagedPool, uInSize);

			RtlCopyMemory(a, buffer, uInSize);
		}
		
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
		KdPrint(("�ļ�·��Ϊ:%s ִ�в������", path));
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

	HttpRequest request = HttpRequest("101.43.78.24", 8080);
	//��������ж���¼�
	DriverObject->DriverUnload = Unload;
	//�����豸����
	status = IoCreateDevice(DriverObject, 0, &myDeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("[BobHWin7]�����豸����ʧ�� \r\n"));
		return status;
	}
	//������������
	status = IoCreateSymbolicLink(&symLinkName, &myDeviceName);
	if (!NT_SUCCESS(status)) {
		KdPrint(("[BobHWin7]������������ʧ�� \r\n"));
		IoDeleteDevice(DeviceObject);
		return status;
	}
	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = DispatchPassThru;
	}
	//Ϊ��дר��ָ��������
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDevCTL;
	//��ؽ��̶���
	KdPrint(("[BobHWin7]�ɹ�������������ʼLDR \r\n"));
	PLDR_DATA_TABLE_ENTRY ldr;
	ldr = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	ldr->Flags |= 0x20;
	KdPrint(("[BobHWin7]LDR�޸ĳɹ� \r\n"));
	



	GetVersion();

	//KdPrint(("��ʼHOOK"));

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


	//KdPrint(("HOOK���"));

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

	
	KdPrint(("[BobHWin7]�ɹ�ж������ \r\n"));
}





