#include "GlobalVariables.h"

PEPROCESS Process = NULL;

DWORD protectPID = -1;
PVOID g_pRegiHandle = NULL;
BOOLEAN isProtecting = FALSE;

UNICODE_STRING myDeviceName = RTL_CONSTANT_STRING(L"\\Device\\BobHWin7Read");//设备名称
UNICODE_STRING symLinkName = RTL_CONSTANT_STRING(L"\\??\\BobHWin7ReadLink");//设备符号链接
PDEVICE_OBJECT DeviceObject = NULL;