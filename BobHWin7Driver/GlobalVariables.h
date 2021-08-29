#include "DeiverDefFun.h"
#include "WindowsStructure.h"

PEPROCESS Process;

DWORD protectPID;
PVOID g_pRegiHandle;
BOOLEAN isProtecting;

UNICODE_STRING myDeviceName;//设备名称
UNICODE_STRING symLinkName;//设备符号链接
PDEVICE_OBJECT DeviceObject;

