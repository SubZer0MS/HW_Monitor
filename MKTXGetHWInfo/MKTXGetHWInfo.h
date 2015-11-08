#pragma once

#include <ntddk.h>
#include <devioctl.h>
#include <stddef.h>

#define MKTX_TYPE 40000

//-----------------------------------------------------------------------------
//
// Version Information
//
//-----------------------------------------------------------------------------

#define MKTX_DRIVER_ID							_T("MKTXGetHWInfo")

#define MKTX_DRIVER_MAJOR_VERSION				1
#define MKTX_DRIVER_MINOR_VERSION				0
#define MKTX_DRIVER_REVISION					0
#define MKTX_DRIVER_RELESE						0

#define MKTX_DRIVER_VERSION \
	((MKTX_DRIVER_MAJOR_VERSION << 24) | (MKTX_DRIVER_MINOR_VERSION << 16) \
	| (MKTX_DRIVER_REVISION << 8) | MKTX_DRIVER_RELESE)

//-----------------------------------------------------------------------------
//
// Device Name
//
//-----------------------------------------------------------------------------

#define NT_DEVICE_NAME	L"\\Device\\MKTXGetHWInfo"
#define DOS_DEVICE_NAME	L"\\DosDevices\\MKTXGetHWInfo"

//-----------------------------------------------------------------------------
//
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
//
//-----------------------------------------------------------------------------
#define IOCTL_MKTX_GET_DRIVER_VERSION \
	CTL_CODE(MKTX_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MKTX_GET_REFCOUNT \
	CTL_CODE(MKTX_TYPE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MKTX_READ_MSR \
	CTL_CODE(MKTX_TYPE, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MKTX_WRITE_MSR \
	CTL_CODE(MKTX_TYPE, 0x822, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MKTX_READ_PMC \
	CTL_CODE(MKTX_TYPE, 0x823, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MKTX_HALT \
	CTL_CODE(MKTX_TYPE, 0x824, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MKTX_READ_IO_PORT \
	CTL_CODE(MKTX_TYPE, 0x831, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_MKTX_WRITE_IO_PORT \
	CTL_CODE(MKTX_TYPE, 0x832, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_MKTX_READ_IO_PORT_BYTE \
	CTL_CODE(MKTX_TYPE, 0x833, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_MKTX_READ_IO_PORT_WORD \
	CTL_CODE(MKTX_TYPE, 0x834, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_MKTX_READ_IO_PORT_DWORD \
	CTL_CODE(MKTX_TYPE, 0x835, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_MKTX_WRITE_IO_PORT_BYTE \
	CTL_CODE(MKTX_TYPE, 0x836, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_MKTX_WRITE_IO_PORT_WORD \
	CTL_CODE(MKTX_TYPE, 0x837, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_MKTX_WRITE_IO_PORT_DWORD \
	CTL_CODE(MKTX_TYPE, 0x838, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_MKTX_READ_MEMORY \
	CTL_CODE(MKTX_TYPE, 0x841, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_MKTX_WRITE_MEMORY \
	CTL_CODE(MKTX_TYPE, 0x842, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_MKTX_READ_PCI_CONFIG \
	CTL_CODE(MKTX_TYPE, 0x851, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_MKTX_WRITE_PCI_CONFIG \
	CTL_CODE(MKTX_TYPE, 0x852, METHOD_BUFFERED, FILE_WRITE_ACCESS)

//-----------------------------------------------------------------------------
//
// PCI Error Codes
//
//-----------------------------------------------------------------------------

#define MKTX_ERROR_PCI_BUS_NOT_EXIST	(0xE0000001L)
#define MKTX_ERROR_PCI_NO_DEVICE		(0xE0000002L)
#define MKTX_ERROR_PCI_WRITE_CONFIG		(0xE0000003L)
#define MKTX_ERROR_PCI_READ_CONFIG		(0xE0000004L)

//-----------------------------------------------------------------------------
//
// Support Macros
//
//-----------------------------------------------------------------------------

// Bus Number, Device Number and Function Number to PCI Device Address
#define PciBusDevFunc(Bus, Dev, Func)	((Bus&0xFF)<<8) | ((Dev&0x1F)<<3) | (Func&7)
// PCI Device Address to Bus Number
#define PciGetBus(address)				((address>>8) & 0xFF)
// PCI Device Address to Device Number
#define PciGetDev(address)				((address>>3) & 0x1F)
// PCI Device Address to Function Number
#define PciGetFunc(address)				(address&7)

//-----------------------------------------------------------------------------
//
// Typedef Struct
//
//-----------------------------------------------------------------------------

#pragma pack(push, 4)

typedef struct _MKTX_WRITE_MSR_INPUT {
	ULONG Register;
	ULARGE_INTEGER Value;
}   MKTX_WRITE_MSR_INPUT;

typedef struct _MKTX_WRITE_IO_PORT_INPUT {
	ULONG PortNumber;
	union _DATA {
		ULONG   LongData;
		USHORT  ShortData;
		UCHAR   CharData;
	} DATA;
}   MKTX_WRITE_IO_PORT_INPUT;

typedef struct _MKTX_READ_PCI_CONFIG_INPUT {
	ULONG PciAddress;
	ULONG PciOffset;
}   MKTX_READ_PCI_CONFIG_INPUT;

typedef struct _MKTX_WRITE_PCI_CONFIG_INPUT {
	ULONG PciAddress;
	ULONG PciOffset;
	UCHAR Data[1];
}   MKTX_WRITE_PCI_CONFIG_INPUT;

typedef struct _MKTX_READ_MEMORY_INPUT {
	LARGE_INTEGER Address;
	ULONG UnitSize;
	ULONG Count;
}   MKTX_READ_MEMORY_INPUT;

typedef struct _MKTX_WRITE_MEMORY_INPUT {
	LARGE_INTEGER Address;
	ULONG UnitSize;
	ULONG Count;
	UCHAR Data[1];
}   MKTX_WRITE_MEMORY_INPUT;

#pragma pack(pop)

//-----------------------------------------------------------------------------
//
// Function Prototypes
//
//-----------------------------------------------------------------------------

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
);

NTSTATUS Dispatch(
	IN PDEVICE_OBJECT pDO,
	IN PIRP pIrp
);

VOID Unload(
	IN PDRIVER_OBJECT DriverObject
);

//-----------------------------------------------------------------------------
//
// Function Prototypes for Control Code
//
//-----------------------------------------------------------------------------

NTSTATUS ReadMsr(
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);

NTSTATUS WriteMsr(
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);

NTSTATUS ReadPmc(
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);

NTSTATUS ReadIoPort(
	ULONG ioControlCode,
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);

NTSTATUS WriteIoPort(
	ULONG ioControlCode,
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);

NTSTATUS ReadPciConfig(
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);

NTSTATUS WritePciConfig(
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);

NTSTATUS ReadMemory(
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);

NTSTATUS WriteMemory(
	void *lpInBuffer,
	ULONG nInBufferSize,
	void *lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG *lpBytesReturned
);


//-----------------------------------------------------------------------------
//
// Support Function Prototypes
//
//-----------------------------------------------------------------------------

NTSTATUS pciConfigRead(
	ULONG pciAddress,
	ULONG offset,
	void *data,
	int length
);

NTSTATUS pciConfigWrite(
	ULONG pciAddress,
	ULONG offset, 
	void *data, 
	int length
);