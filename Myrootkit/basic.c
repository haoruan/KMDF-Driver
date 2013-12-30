#include "basic.h"

NTSTATUS DriverEntry( IN PDRIVER_OBJECT theDriverObject, IN PUNICODE_STRING theRegistryPath )
{
	NTSTATUS ntStatus;
	UNICODE_STRING deviceNameUnicodeString;
	UNICODE_STRING deviceLinkUnicodeString;
	UNREFERENCED_PARAMETER(theRegistryPath);

	DbgPrint("I loaded!");
	
	theDriverObject->DriverUnload  = OnUnload;

	theDriverObject->MajorFunction[IRP_MJ_CREATE] = MyOpen;
	theDriverObject->MajorFunction[IRP_MJ_CLOSE] = MyClose;
	theDriverObject->MajorFunction[IRP_MJ_READ] = MyRead;
	theDriverObject->MajorFunction[IRP_MJ_WRITE] = MyWrite;
	theDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyIoControl;

	RtlInitUnicodeString(&deviceNameUnicodeString, DEVICE_NAME);
	RtlInitUnicodeString(&deviceLinkUnicodeString, DOT_DEVICE_NAME);
	ntStatus = IoCreateDevice(theDriverObject, 
							0, 
							&deviceNameUnicodeString, 
							0x00001234, 
							0, 
							TRUE, 
							&g_rootkitDevice);

	if (NT_SUCCESS(ntStatus)) {
		ntStatus = IoCreateSymbolicLink(&deviceLinkUnicodeString, &deviceNameUnicodeString);
	}

	return ntStatus;
}

NTSTATUS MyOpen(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	DbgPrint("Kernal Open Operation!");
	
	return STATUS_SUCCESS;
}

NTSTATUS MyClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	DbgPrint("Kernal Close Operation!");
	
	return STATUS_SUCCESS;
}

NTSTATUS MyRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	DbgPrint("Kernal Read Operation!");
	
	return STATUS_SUCCESS;
}

NTSTATUS MyWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	DbgPrint("Kernal Write Operation!");
	
	return STATUS_SUCCESS;
}

NTSTATUS MyIoControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	DbgPrint("Kernal IoControl Operation!");
	
	return STATUS_SUCCESS;
}

VOID OnUnload( IN PDRIVER_OBJECT DriverObject )
{
	PDEVICE_OBJECT      deviceObject = DriverObject->DeviceObject;
    UNICODE_STRING      uniWin32NameString;

	RtlInitUnicodeString( &uniWin32NameString, DOT_DEVICE_NAME );

    IoDeleteSymbolicLink( &uniWin32NameString );

    IoDeleteDevice( deviceObject );

	DbgPrint("OnUnload called\n");
}

