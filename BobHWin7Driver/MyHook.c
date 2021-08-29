#include "MyHook.h"


//nt!NtOpenProcess:
//fffff800`041582ec 4883ec38        sub     rsp, 38h
//fffff800`041582f0 65488b042588010000 mov   rax, qword ptr gs : [188h]
//fffff800`041582f9 448a90f6010000  mov     r10b, byte ptr[rax + 1F6h]
//fffff800`04158300 4488542428      mov     byte ptr[rsp + 28h], r10b
//fffff800`04158305 4488542420      mov     byte ptr[rsp + 20h], r10b
//fffff800`0415830a e851fcffff      call    nt!PsOpenProcess(fffff800`04157f60)

VOID StartHOOK(UINT64 HOOK函数地址, UINT64 代理函数地址, USHORT 改写的长度, PVOID *原函数)
{
	///*48 B8 88 77 66 55 44 33 22 11 FF E0*/
	/*下面的数值12是跳转指令的总长度
	{



		//BYTE Hookcode[] = { 0x48 ,0xBA ,0x88, 0x77 ,0x66, 0x55, 0x44, 0x33, 0x22 ,0x11, 0xFF, 0xE2 };


		//*原函数 = ExAllocatePool(NonPagedPool, 改写的长度 + (USHORT)100);


		////拷贝头
		//RtlZeroMemory(*原函数, 0x1000);

		//RtlCopyMemory(*原函数, (PVOID)HOOK函数地址, 改写的长度);


		////写jmp

		//ULONG_PTR tempjmp = ((ULONG_PTR)HOOK函数地址 + 改写的长度);

		//*((PULONG_PTR)(Hookcode + 2)) = tempjmp;

		//RtlCopyMemory(((PUCHAR)(*原函数) + 改写的长度), Hookcode, sizeof(Hookcode));




		////取我们地址
		//*((PULONG_PTR)(Hookcode + 2)) = (ULONG_PTR)代理函数地址;


		////写jmp
		//KIRQL irql = WPOFFx64();

		//RtlCopyMemory(HOOK函数地址, Hookcode, sizeof(Hookcode));

		//WPONx64(irql);


	}
	*/
	{

		UCHAR 跳到代理函数[] = "\xFF\x25\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
		UCHAR 跳回原函数[] = "\xFF\x25\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
		memcpy(跳到代理函数 + 6, &代理函数地址, 8);

		/*
		下面的数值14是跳转指令的总长度 假设该指令地址为0x410000
		0x410000 jmp qword ptr [0x410006]
		0x410006 xxxxxxxx
		其中0x410006中储存代理函数的地址
		*/
		UINT64 跳回原函数的地址 = HOOK函数地址 + 改写的长度;
		memcpy(跳回原函数 + 6, &跳回原函数的地址, 8);
		*原函数 = ExAllocatePool(NonPagedPool, 改写的长度 + 14);
		RtlFillMemory(*原函数, 改写的长度 + 14, 0x90);

		KIRQL irql = WPOFFx64();
		memcpy(*原函数, (PVOID)HOOK函数地址, 改写的长度);
		memcpy((PCHAR)(*原函数) + 改写的长度, 跳回原函数, 14);

		KIRQL dpc_irql = KeRaiseIrqlToDpcLevel();
		RtlFillMemory((void*)HOOK函数地址, 改写的长度, 0x90);
		memcpy((PVOID)HOOK函数地址, &跳到代理函数, 14);
		KeLowerIrql(dpc_irql);
		WPONx64(irql);
	}

	
}



VOID RecoveryHOOK(UINT64 HOOK函数地址, USHORT 改写的长度, PVOID 原函数)
{
	KIRQL irql = WPOFFx64();
	memcpy((PVOID)HOOK函数地址, 原函数, 改写的长度);
	WPONx64(irql);
	ExFreePool(原函数);
}