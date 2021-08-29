#pragma once

#include "ntifs.h"
#include "ntddk.h"
#include <windef.h>
#include "ntdef.h"
#include "DeiverDefFun.h"

VOID StartHOOK(UINT64 HOOK函数地址, UINT64 代理函数地址, USHORT 改写的长度, PVOID *jmpBrigePtr);
VOID RecoveryHOOK(UINT64 HOOK函数地址, USHORT 改写的长度, PVOID  原函数);