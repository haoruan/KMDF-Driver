#ifndef _BASIC_H
#define _BASIC_H

#include <ntddk.h>

#define DOT_DEVICE_NAME L"\\DosDevices\\MyDevice"
#define DEVICE_NAME L"\\Device\\MyDevice"

DRIVER_INITIALIZE DriverEntry;
_Dispatch_type_(IRP_MJ_CREATE)
DRIVER_DISPATCH MyOpen;
_Dispatch_type_(IRP_MJ_CLOSE)
DRIVER_DISPATCH MyClose;
_Dispatch_type_(IRP_MJ_READ)
DRIVER_DISPATCH MyRead;
_Dispatch_type_(IRP_MJ_WRITE)
DRIVER_DISPATCH MyWrite;
_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH MyIoControl;

DRIVER_UNLOAD OnUnload;

PDEVICE_OBJECT g_rootkitDevice;

#endif