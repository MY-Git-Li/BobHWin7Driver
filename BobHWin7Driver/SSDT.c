#include "SSDT.h"
#include "ntimage.h"
#include "DeiverDefFun.h"
#define SEC_IMAGE_ASD 0x1000000

//通用获得PSYSTEM_SERVICE_TABLE方法
PSYSTEM_SERVICE_TABLE GetSystemServiceTable_Generalmethod(PDRIVER_OBJECT pdriver);
//获得PSYSTEM_SERVICE_TABLE方法 win7-win10 1895
PSYSTEM_SERVICE_TABLE GetSystemServiceTable();
//得到在SSDT中的地址
ULONG64 GetSSDTAddress(PSYSTEM_SERVICE_TABLE servicestable, DWORD index);
//得到在ShadowSSDT中的地址
ULONG64 GetShadowSSDTAddress(PSYSTEM_SERVICE_TABLE servicestable, DWORD index);




//得到内核模块地址
ULONG_PTR GetKernelModuleBase(PDRIVER_OBJECT pdiver, PULONG pimagesize, PWCHAR modulename)
{
	//
	//采用遍历链表的方法
	//

	UNICODE_STRING kernelname = { 0 };
	ULONG_PTR uret = 0;

	PLDR_DATA_TABLE_ENTRY64 pentry = (PLDR_DATA_TABLE_ENTRY64)pdiver->DriverSection;

	PLDR_DATA_TABLE_ENTRY64 first = NULL;

	RtlInitUnicodeString(&kernelname, modulename);

	__try 
	{
		do 
		{
			if (pentry->BaseDllName.Buffer != NULL)
			{
				if (RtlCompareUnicodeString(&pentry->BaseDllName,&kernelname,TRUE) == 0)
				{
					uret = (ULONG_PTR)pentry->DllBase;
					if (pimagesize)
					{
						*pimagesize = pentry->SizeOfImage;
					}
					break;
				}
				pentry =(PLDR_DATA_TABLE_ENTRY64)pentry->InLoadOrderLinks.Blink;
			}

		} while (pentry->InLoadOrderLinks.Blink != (ULONGLONG)first);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return uret;
	}

	return uret;
}
//通用获得PSYSTEM_SERVICE_TABLE方法
PSYSTEM_SERVICE_TABLE GetSystemServiceTable_Generalmethod(PDRIVER_OBJECT pdriver)
{
	ULONG_PTR uret = 0;

	PUCHAR kernelbase = NULL;

	ULONG kernelSize = 0;

	kernelbase = GetKernelModuleBase(pdriver, &kernelSize, L"ntoskrnl.exe");

	if (!kernelbase)
	{
		return (PSYSTEM_SERVICE_TABLE)uret;
	}

	//特征码
	BYTE KiSystemServiceStartPattern[] = { 0x8b,0xf8,0xc1,0xef,0x07,0x83,0xe7,0x20,0x25,0xff,0x0f,0x00,0x00 };

	ULONG signatureSize = sizeof(KiSystemServiceStartPattern);

	ULONG KiSSSOffset = 0;

	BOOLEAN bfound = FALSE;

	//特征码搜索
	for (KiSSSOffset = 0;KiSSSOffset < kernelSize - signatureSize;KiSSSOffset++)
	{
		if (RtlCompareMemory((kernelbase + KiSSSOffset),KiSystemServiceStartPattern,signatureSize) == signatureSize)
		{
			bfound = TRUE;
			break;
		}
	}

	if (!bfound)
	{
		return (PSYSTEM_SERVICE_TABLE)uret;
	}

	//这里就是 lea r10 ，KeServiceDescriptorTable
	PUCHAR address = kernelbase + KiSSSOffset + signatureSize;//==KiSystemServiceStart

	address = address + 7;

	LONG jmpoffset = 0;

	if ((*address == 0x4c)  && (*(address + 1) == 0x8d) && (*(address + 2) == 0x1d))
	{
		jmpoffset = *(PLONG)(address + 3);  //lea r10 ，KeServiceDescriptorTable
	}

	if (!jmpoffset)
	{
		return (PSYSTEM_SERVICE_TABLE)uret;
	}

	uret = (ULONG_PTR)(address + jmpoffset + 7);

	return (PSYSTEM_SERVICE_TABLE)uret;
}
//获得PSYSTEM_SERVICE_TABLE方法 win7-win10 1895
PSYSTEM_SERVICE_TABLE GetSystemServiceTable()
{
	DWORD64 dmsr = __readmsr(0xC0000082);

	PUCHAR tempptr = (PUCHAR)dmsr;

	LONG offset = 0;

	PSYSTEM_SERVICE_TABLE servicetable = NULL;

	for (int i=0;i<= 0x1000;i++	)
	{
		if (*(tempptr+i) == 0x4c && *(tempptr + i + 1) == 0x8d && *(tempptr + i + 2) == 0x15)
		{
			offset = *((PLONG)(tempptr + i + 3));

			servicetable = (PSYSTEM_SERVICE_TABLE)(tempptr + i + 7 + offset);

			break;
		}
	}

	return servicetable;
}

//得到在SSDT中的地址
ULONG64 GetSSDTAddress(PSYSTEM_SERVICE_TABLE servicestable,DWORD index)
{

	ULONG64 reallyaddr = 0;

	PLONG tablestart = servicestable->tablebase;

	ULONG tempoffset = tablestart[index];

	tempoffset = tempoffset >> 4;

	reallyaddr = (ULONG64)tempoffset + (ULONG64)servicestable->tablebase;

	/*DbgPrint("SSDT %d : <%p> \r\n", index, reallyaddr);*/
	return reallyaddr;
}

//获得文件的导出
PVOID AsdGetModuleExport(IN PVOID pBase, IN PCCHAR name_ord)
{
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)pBase;
	PIMAGE_NT_HEADERS32 pNtHdr32 = NULL;
	PIMAGE_NT_HEADERS64 pNtHdr64 = NULL;
	PIMAGE_EXPORT_DIRECTORY pExport = NULL;
	ULONG expSize = 0;
	ULONG_PTR pAddress = 0;

	ASSERT(pBase != NULL);
	if (pBase == NULL)
		return NULL;

	/// Not a PE file
	if (pDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	pNtHdr32 = (PIMAGE_NT_HEADERS32)((PUCHAR)pBase + pDosHdr->e_lfanew);
	pNtHdr64 = (PIMAGE_NT_HEADERS64)((PUCHAR)pBase + pDosHdr->e_lfanew);

	// Not a PE file
	if (pNtHdr32->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	// 64 bit image
	if (pNtHdr32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		pExport = (PIMAGE_EXPORT_DIRECTORY)(pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)pBase);
		expSize = pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	}
	// 32 bit image
	else
	{
		pExport = (PIMAGE_EXPORT_DIRECTORY)(pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)pBase);
		expSize = pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	}

	PUSHORT pAddressOfOrds = (PUSHORT)(pExport->AddressOfNameOrdinals + (ULONG_PTR)pBase);
	PULONG  pAddressOfNames = (PULONG)(pExport->AddressOfNames + (ULONG_PTR)pBase);
	PULONG  pAddressOfFuncs = (PULONG)(pExport->AddressOfFunctions + (ULONG_PTR)pBase);

	for (ULONG i = 0; i < pExport->NumberOfFunctions; ++i)
	{
		USHORT OrdIndex = 0xFFFF;
		PCHAR  pName = NULL;

		// Find by index
		if ((ULONG_PTR)name_ord <= 0xFFFF)
		{
			OrdIndex = (USHORT)i;
		}
		// Find by name
		else if ((ULONG_PTR)name_ord > 0xFFFF && i < pExport->NumberOfNames)
		{
			pName = (PCHAR)(pAddressOfNames[i] + (ULONG_PTR)pBase);
			OrdIndex = pAddressOfOrds[i];
		}
		// Weird params
		else
			return NULL;

		if (((ULONG_PTR)name_ord <= 0xFFFF && (USHORT)((ULONG_PTR)name_ord) == OrdIndex + pExport->Base) ||
			((ULONG_PTR)name_ord > 0xFFFF && strcmp(pName, name_ord) == 0))
		{
			pAddress = pAddressOfFuncs[OrdIndex] + (ULONG_PTR)pBase;

			// Check forwarded export
			if (pAddress >= (ULONG_PTR)pExport && pAddress <= (ULONG_PTR)pExport + expSize)
			{

				break;
			}
			break;
		}
	}
	return (PVOID)pAddress;
}

//将文件映射到内核地址
PVOID AsdkLoadLibrary(const wchar_t *fun_dll_path)
{
	HANDLE hSection = NULL, hFile = NULL;

	UNICODE_STRING dllName = { 0 };

	PVOID BaseAddress = NULL;

	SIZE_T size = 0;

	NTSTATUS stat = 0;

	OBJECT_ATTRIBUTES oa = { sizeof(oa),0,&dllName,OBJ_CASE_INSENSITIVE };
	
	IO_STATUS_BLOCK iosb = { 0 };

	RtlInitUnicodeString(&dllName, fun_dll_path);

	//asm int 3
	stat = ZwOpenFile(&hFile, FILE_EXECUTE | SYNCHRONIZE, &oa, &iosb, FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);

	if (!NT_SUCCESS(stat))
	{
		return 0;
	}

	oa.ObjectName = 0;

	stat = ZwCreateSection(&hSection, SECTION_ALL_ACCESS, &oa, 0, PAGE_EXECUTE, SEC_IMAGE_ASD, hFile);

	if (!NT_SUCCESS(stat))
	{
		DbgPrint("1<%x>", stat);
		return 0;
	}


	stat = ZwMapViewOfSection(hSection, NtCurrentProcess(), &BaseAddress, 0, 1000, 0, &size, (SECTION_INHERIT)1, MEM_TOP_DOWN, PAGE_READWRITE);

	if (!NT_SUCCESS(stat))
	{
		DbgPrint("2<%x>", stat);
		return 0;
	}

	ZwClose(hSection);
	ZwClose(hFile);

	return BaseAddress;
}
//得到函数在SSDT中的序列号
ULONG GetSSDTFunIndex(PUCHAR funname)
{
	ULONG_PTR tempfunaddr = 0;

	ULONG funaddrid = 0;

	PVOID ntdll = NULL;

	ntdll = AsdkLoadLibrary(L"\\SystemRoot\\System32\\ntdll.dll");

	if (!ntdll)
	{
		return 0;
	}

	tempfunaddr = (ULONG_PTR)AsdGetModuleExport(ntdll, funname);

	if (!tempfunaddr)
	{
		return 0;
	}

	funaddrid = *(PULONG)((PUCHAR)tempfunaddr + 4);


}

//得到在SSDT中的地址
ULONG64 GetShadowSSDTAddress(PSYSTEM_SERVICE_TABLE servicestable, DWORD index)
{
	return GetSSDTAddress(servicestable, index);
}

//得到在SSDT中序列号为index的地址
ULONG64 GetSSDTAddr(PDRIVER_OBJECT pdriver, DWORD index)
{

	ULONG64 ret = 0;
	PSYSTEM_SERVICE_TABLE SystemServiceTable = GetSystemServiceTable_Generalmethod(pdriver);

	PSYSTEM_SERVICE_TABLE ssdt = SystemServiceTable;

	ret = GetSSDTAddress(ssdt, index);

	return ret;
}
//得到在ShadowSSDT中序列号为index的地址
ULONG64 GetShadowSSDTAddr(PDRIVER_OBJECT pdriver, DWORD index)
{
	ULONG64 ret = 0;
	PSYSTEM_SERVICE_TABLE SystemServiceTable = GetSystemServiceTable_Generalmethod(pdriver);

	PSYSTEM_SERVICE_TABLE shoadowssdt = (PSYSTEM_SERVICE_TABLE)((DWORD64)SystemServiceTable + sizeof(SYSTEM_SERVICE_TABLE));

	STRING processname = { 0 };
	RtlInitString(&processname, "csrss.exe");
	DWORD pid = GetPidByEnumProcess(processname);
	PEPROCESS tempep = NULL;
	NTSTATUS status = PsLookupProcessByProcessId(pid, &tempep);

	if (NT_SUCCESS(status))
	{
		KAPC_STATE kapc = { 0 };
		KeStackAttachProcess(tempep, &kapc);

		ret = GetShadowSSDTAddress(shoadowssdt, index);

		KeUnstackDetachProcess(&kapc);
		ObDereferenceObject(tempep);
	}

	return ret;

}
