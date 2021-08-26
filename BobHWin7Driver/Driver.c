#include "Driverdef.h"
#include "DeiverDefFun.h"
#include "Hide.c"
#define DELAY_ONE_MICROSECOND 	(-10)
#define DELAY_ONE_MILLISECOND	(DELAY_ONE_MICROSECOND*1000)

VOID KernelSleep(LONG msec)
{
	LARGE_INTEGER my_interval;
	my_interval.QuadPart = DELAY_ONE_MILLISECOND;
	my_interval.QuadPart *= msec;
	KeDelayExecutionThread(KernelMode, 0, &my_interval);
}

VOID
DelObject(
	_In_ PVOID StartContext
)
{
	PULONG_PTR pZero = NULL;
	KernelSleep(5000);
	ObMakeTemporaryObject(DeviceObject);
	DPRINT("test seh.\n");
	__try {
		*pZero = 0x100;
	}
	__except (1)
	{
		DPRINT("seh success.\n");
	}
}

VOID Reinitialize(
	_In_     PDRIVER_OBJECT        pDriverObject,
	_In_opt_ PVOID                 Context,
	_In_     ULONG                 Count
)
{
	HANDLE hThread = NULL;
	PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, DelObject, NULL);
	if (*NtBuildNumber < 8000)
		HideDriverWin7(pDriverObject);
	else
		HideDriverWin10(pDriverObject);
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
		memcpy(&appBuffer, buffer, uInSize);
		KdPrint(("读收到的地址是:%x \r\n", appBuffer.Address));
		tmpbuffer = ExAllocatePool(NonPagedPool, appBuffer.size + 1);
		RtlFillMemory(tmpbuffer, appBuffer.size + 1, 0);
		/*KdPrint(("tmpbuffer地址是%x 内容为%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));*/
		KeReadProcessMemory(appBuffer.Address, tmpbuffer, appBuffer.size);
		/*KdPrint(("tmpbuffer地址是%x 内容为%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));*/
		memcpy(&appBuffer.Buffer, tmpbuffer, sizeof(tmpbuffer));
		memcpy(buffer, &appBuffer, uInSize);
		ExFreePool(tmpbuffer);
		status = STATUS_SUCCESS;
		break;
	case BOBH_WRITE:
		memcpy(&appBuffer, buffer, uInSize);
		KdPrint(("写收到的地址是:%x \r\n", appBuffer.Address));

		tmpbuffer = ExAllocatePool(NonPagedPool, appBuffer.size + 1);
		RtlFillMemory(tmpbuffer, appBuffer.size + 1, 0);
		KdPrint(("tmpbuffer地址是%x 内容为%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));

		memcpy(tmpbuffer, &appBuffer.Buffer, appBuffer.size);
		KdPrint(("tmpbuffer地址是%x 内容为%d \r\n", tmpbuffer, *(DWORD*)tmpbuffer));

		KeWriteProcessMemory(appBuffer.Address, tmpbuffer, appBuffer.size);

		ExFreePool(tmpbuffer);
		status = STATUS_SUCCESS;

		break;
	case BOBH_SET:
	{
		DWORD PID;
		memcpy(&PID, buffer, uInSize);
		SetPID(PID);
		status = STATUS_SUCCESS;
		break;
	}
	case BOBH_PROTECT:
	{
		DWORD PID;
		memcpy(&PID, buffer, uInSize);
		ProtectProcessStart(PID);
		status = STATUS_SUCCESS;
		break;
	}
	case BOBH_UNPROTECT:
	{
		ProtectProcessStop();
		status = STATUS_SUCCESS;
		break;
	}
	case BOBH_KILLPROCESS_DIRECT:
	{
		DWORD PID;
		memcpy(&PID, buffer, uInSize);
		KeKillProcessSimple(PID);
		status = STATUS_SUCCESS;
		break;
	}
	case BOBH_KILLPROCESS_MEMORY:
	{
		DWORD PID;
		memcpy(&PID, buffer, uInSize);
		KeKillProcessZeroMemory(PID);
		status = STATUS_SUCCESS;
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

		UNICODE_STRING moudlename = { 0 };
		ULONG64 outdllbase = 0;
		RtlInitUnicodeString(&moudlename, TempBase->ModuleName);
		KdPrint(("要寻找的moudlename为%wZ \r\n", moudlename));

		outdllbase = KeGetMoudleAddress(TempBase->Pid, &moudlename);

		*(PULONG64)buffer = outdllbase;

		status = STATUS_SUCCESS;
		break;
	}
	case BOBH_GETPROCESSID:
	{


		PMYCHAR a = (PMYCHAR)buffer;

		STRING process = { 0 };


		RtlInitString(&process, a->_char);
		KdPrint(("process %s \r\n", process));


		DWORD pid = 0;
		pid = EnumProcess(process);
		*(PDWORD)buffer = pid;

		if (pid > 0)
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
	//ProtectProcessStart(1234);
	//ProtectProcessStart(3100);
	// 
	//隐藏驱动卸载蓝屏,偶尔蓝屏
		HideDriver(DriverObject);
	////隐藏驱动 卸载蓝屏,待解决.
	/*IoRegisterDriverReinitialization(DriverObject, Reinitialize, NULL);*/
	
	return status;
}

VOID Unload(PDRIVER_OBJECT DriverObject) {
	if (isProtecting) {
		ObUnRegisterCallbacks(g_pRegiHandle);
	}
	IoDeleteSymbolicLink(&symLinkName);
	IoDeleteDevice(DeviceObject);
	KdPrint(("[BobHWin7]成功卸载驱动 \r\n"));
}



// 根据进程ID返回进程EPROCESS结构体,失败返回NULL
PEPROCESS LookupProcess(HANDLE Pid)
{
	PEPROCESS eprocess = NULL;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	Status = PsLookupProcessByProcessId(Pid, &eprocess);
	if (NT_SUCCESS(Status))
		return eprocess;
	return NULL;
}

DWORD EnumProcess(STRING processName)
{
	PEPROCESS eproc = NULL;

	DWORD ret = 0;

	for (int temp = 0; temp < 100000; temp += 4)
	{
		eproc = LookupProcess((HANDLE)temp);
		if (eproc != NULL)
		{
			STRING nowProcessnameString = { 0 };
			RtlInitString(&nowProcessnameString, PsGetProcessImageFileName(eproc));
			
			if (RtlCompareString(&nowProcessnameString, &processName,FALSE)==0)
			{
				ret = PsGetProcessId(eproc);
				break;
			}

			DbgPrint("进程名: %s --> 进程PID = %d --> 父进程PPID = %d\r\n", PsGetProcessImageFileName(eproc), PsGetProcessId(eproc),
				PsGetProcessInheritedFromUniqueProcessId(eproc));
			ObDereferenceObject(eproc);
		}
	}
	return ret;
}


VOID KeReadProcessMemory(ULONG64 add, PVOID buffer, SIZE_T size){
	KAPC_STATE apc_state;
	KeStackAttachProcess(Process, &apc_state);
	__try
	{
		if (MmIsAddressValid(add))
		{
			memcpy(buffer, (PVOID)add, size);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("读取错误:地址:%llX", add);
	}
	KeUnstackDetachProcess(&apc_state);
}
VOID KeWriteProcessMemory(ULONG64 add, PVOID buffer, SIZE_T size) {
	KAPC_STATE apc_state;
	KeStackAttachProcess(Process, &apc_state);
	__try
	{
		if (MmIsAddressValid(add))
		{
			memcpy((PVOID)add, buffer, size);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("读取错误:地址:%llX", add);
	}
	KeUnstackDetachProcess(&apc_state);
}
VOID SetPID(DWORD pid) {
	NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &Process);
	if (!NT_SUCCESS(status)) {
		KdPrint(("[BobHWin7]设置PID失败 \r\n"));
		return;
	}
	KdPrint(("[BobHWin7]设置PID: %d 成功 \r\n",pid));
}
NTSTATUS DispatchPassThru(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	//得到irp堆栈地址
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	//完成IRP请求
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
VOID KeKillProcessSimple(DWORD pid) {
	__try {
		HANDLE hProcess = NULL;
		CLIENT_ID ClientId = { 0 };
		OBJECT_ATTRIBUTES oa = { 0 };
		ClientId.UniqueProcess = (HANDLE)pid;
		ClientId.UniqueThread = 0;
		oa.Length = sizeof(oa);
		oa.RootDirectory = 0;
		oa.ObjectName = 0;
		oa.Attributes = 0;
		oa.SecurityDescriptor = 0;
		oa.SecurityQualityOfService = 0;
		ZwOpenProcess(&hProcess, 1, &oa, &ClientId);
		if (hProcess)
		{
			ZwTerminateProcess(hProcess, 0);
			ZwClose(hProcess);
		}
		KdPrint(("[BobHWin7] 杀进程成功"));
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("[BobHWin7] 普通方法杀进程失败"));
	}
}
BOOLEAN KeKillProcessZeroMemory(DWORD pid) {
	NTSTATUS ntStatus = STATUS_SUCCESS;
	int i = 0;
	PVOID handle;
	PEPROCESS Eprocess;
	ntStatus = PsLookupProcessByProcessId(pid, &Eprocess);
	if (NT_SUCCESS(ntStatus))
	{
		PKAPC_STATE pKs = (PKAPC_STATE)ExAllocatePool(NonPagedPool, sizeof(PKAPC_STATE));
		KeStackAttachProcess(Eprocess, pKs);//Attach进程虚拟空间
		for (i = 0; i <= 0x7fffffff; i += 0x1000)
		{
			if (MmIsAddressValid((PVOID)i))
			{
				_try
				{
					ProbeForWrite((PVOID)i,0x1000,sizeof(ULONG));
					memset((PVOID)i,0xcc,0x1000);
				}
				_except(1) { continue; }
			}
			else {
				if (i>0x1000000)  //填这么多足够破坏进程数据了  
					break;
			}
		}
		KeUnstackDetachProcess(pKs);
		if (ObOpenObjectByPointer((PVOID)Eprocess, 0, NULL, 0, NULL, KernelMode, &handle) != STATUS_SUCCESS)
			return FALSE;
		ZwTerminateProcess((HANDLE)handle, STATUS_SUCCESS);
		ZwClose((HANDLE)handle);
		return TRUE;
	}
	return FALSE;

}
OB_PREOP_CALLBACK_STATUS MyObjectPreCallback
(
	__in PVOID  RegistrationContext,
	__in POB_PRE_OPERATION_INFORMATION  pOperationInformation
) 
{
	//KdPrint(("[BobHWin7]进来了！！！ \r\n"));
	if (pOperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;
	HANDLE pid = PsGetProcessId((PEPROCESS)pOperationInformation->Object);
	if (pid == protectPID) {
		//KdPrint(("[BobHWin7]有关PID执行操作"));
		if (pOperationInformation->Operation == OB_OPERATION_HANDLE_CREATE){
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_TERMINATE) == PROCESS_TERMINATE)
			{
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
				//pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			}
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)//openprocess
			{
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_OPERATION;
				//pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			}
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_READ) == PROCESS_VM_READ)//内存读
			{
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_READ;
				//pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			}
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)//内存写
			{
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_WRITE;
				//pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			}
		}
	}
	return OB_PREOP_SUCCESS;
}


VOID ProtectProcessStart(DWORD pid) {
	if (isProtecting) {
		return;
	}
	protectPID = pid;
	KdPrint(("[BobHWin7] 开始保护PID:%d",pid));
	OB_OPERATION_REGISTRATION oor;
	OB_CALLBACK_REGISTRATION ob;
	oor.ObjectType = PsProcessType;
	oor.Operations = OB_OPERATION_HANDLE_CREATE;//应用层打开句柄，通知回调,也可添加OB_OPERATION_HANDLE_DUPLICATE  表示赋值句柄
	oor.PreOperation = MyObjectPreCallback;
	oor.PostOperation = NULL;
	ob.Version = OB_FLT_REGISTRATION_VERSION;
	ob.OperationRegistrationCount = 1;
	ob.OperationRegistration = &oor;
	RtlInitUnicodeString(&ob.Altitude, L"321000");
	ob.RegistrationContext = NULL;

	NTSTATUS status = ObRegisterCallbacks(&ob, &g_pRegiHandle);
	if (NT_SUCCESS(status)) {
		KdPrint(("[BobHWin7]注册obj回调成功 \r\n"));
		isProtecting = TRUE;
	}
	else {
		KdPrint(("[BobHWin7]注册obj回调失败 %x\r\n",status));
		isProtecting = FALSE;
	}
}
VOID ProtectProcessStop() {
	if (isProtecting) {
		ObUnRegisterCallbacks(g_pRegiHandle);
		isProtecting = FALSE;
	}
}

ULONGLONG KeGetMoudleAddress(_In_ ULONG pid, _In_ PUNICODE_STRING name)
{
	PEPROCESS p = NULL;
	ULONGLONG ModuleBase = 0;
	NTSTATUS status = STATUS_SUCCESS;
	KAPC_STATE kapc_state = { 0 };
	ULONG64  dllbaseaddr = 0;
	status = PsLookupProcessByProcessId((HANDLE)pid, &p);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("寻找失败"));
		return 0;
	}
	KeStackAttachProcess(p, &kapc_state);

	
	PPEB32 peb32 = NULL;

	peb32 = (PPEB32)PsGetProcessWow64Process(p);

	if (peb32 != NULL)
	{
		
		KdPrint(("wow64进程"));
		
		PPEB_LDR_DATA32 pPebLdrData32 = NULL;
		PLDR_DATA_TABLE_ENTRY32 pLdrDataEntry32 = NULL; //LDR链表入口;
		PLIST_ENTRY32 pListEntryStart32 = NULL; //链表头节点、尾节点;
		PLIST_ENTRY32 pListEntryEnd32 = NULL;
		__try
		{
			
			pPebLdrData32 = (PPEB_LDR_DATA32)peb32->Ldr;
		/*	KdPrint(("pPebLdrData32 %x", pPebLdrData32));*/

			pListEntryStart32 = pListEntryEnd32 = pPebLdrData32->InLoadOrderModuleList.Flink;
			/*KdPrint(("pListEntryStart32 %x", pListEntryStart32));*/
			do {//输出DLL全路径;

				
				pLdrDataEntry32 = (PLDR_DATA_TABLE_ENTRY32)CONTAINING_RECORD(pListEntryStart32, LDR_DATA_TABLE_ENTRY32, InMemoryOrderLinks);
				
				/*KdPrint(("pLdrDataEntry32 %x", pLdrDataEntry32));*/

				WCHAR* a = (DWORD)pLdrDataEntry32->BaseDllName;
				
				/*KdPrint(("BaseDllName %x", pLdrDataEntry32->BaseDllName));*/

				UNICODE_STRING nowMoudlename = { 0 };
				RtlInitUnicodeString(&nowMoudlename, a);

				if (RtlCompareUnicodeString(&nowMoudlename, name, TRUE) == 0 ) {

					dllbaseaddr = pLdrDataEntry32->DllBase;

					KdPrint(("找到了！"));
					KdPrint(("DllBase %x", dllbaseaddr));
					break;
				}

				pListEntryStart32 = (PLIST_ENTRY32)pListEntryStart32->Flink;
			} while (pListEntryStart32 != pListEntryEnd32);

		}
		__except (1)
		{
			KdPrint(("内存访问异常"));
			
			dllbaseaddr = 0;
		}
	}
	else
	{
		PPEB peb = NULL;
		peb = (PPEB)PsGetProcessPeb(p);
		if (peb == NULL)
		{
			ObDereferenceObject(p);
			KdPrint(("寻找失败"));
			KeUnstackDetachProcess(&kapc_state);
			return 0;
		}
		else
		{
			KdPrint(("非wow64进程"));

			PPEB_LDR_DATA pPebLdrData = (PPEB_LDR_DATA)peb->Ldr;

			PLIST_ENTRY plistEntryStart = NULL, plistEntryEnd = NULL;

			PLDR_DATA_TABLE_ENTRY pLdrDataEntry = NULL;

			plistEntryStart = plistEntryEnd = pPebLdrData->InMemoryOrderModuleList.Blink;
		
			__try
			{

				do
				{
					pLdrDataEntry = (PLDR_DATA_TABLE_ENTRY)CONTAINING_RECORD(plistEntryStart, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

					if (RtlCompareUnicodeString(&pLdrDataEntry->BaseDllName, name, TRUE) == 0)
					{
						dllbaseaddr = (ULONG64)pLdrDataEntry->DllBase;
						KdPrint(("找到了！"));
						KdPrint(("DllBase %x", dllbaseaddr));
						break;
					}
					/*KdPrint(("pLdrDataEntry  0x%x", pLdrDataEntry));*/

					plistEntryStart = plistEntryStart->Blink;

				} while (plistEntryStart != plistEntryEnd);
			}
			__except (1)
			{
				KdPrint(("内存访问异常"));
				dllbaseaddr = 0;
			}
		}
	}

	ObDereferenceObject(p);
	KeUnstackDetachProcess(&kapc_state);
	return dllbaseaddr;
}

//
//关闭内存写保护的代码
//
KIRQL WPOFFx64()
{
	KIRQL irql = KeRaiseIrqlToDpcLevel();
	UINT64 cr0 = __readcr0();
	cr0 &= 0xfffffffffffeffff;
	__writecr0(cr0);
	_disable();
	return irql;
}

//
//打开内存写保护的代码
//
void WPONx64(
	KIRQL irql)
{
	UINT64 cr0 = __readcr0();
	cr0 |= 0x10000;
	_enable();
	__writecr0(cr0);
	KeLowerIrql(irql);
}
//
//test hide driver
//
BOOLEAN HideDriver(
	_In_ PDRIVER_OBJECT pDrvObj)
{
	if (pDrvObj->DriverSection != NULL)
	{
		PLIST_ENTRY nextSection = ((PLIST_ENTRY)pDrvObj->DriverSection)->Blink;
		RemoveEntryList((PLIST_ENTRY)pDrvObj->DriverSection);
		pDrvObj->DriverSection = nextSection;
		DbgPrint("隐藏驱动成功");
		return TRUE;
	}
	DbgPrint("隐藏驱动失败");
	return FALSE;
}