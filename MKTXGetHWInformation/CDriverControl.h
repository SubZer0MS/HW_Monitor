#pragma once

#define MKTX_TYPE 40000

#define NT_DEVICE_NAME	L"\\Device\\MKTXGetHWInfo"
#define DOS_DEVICE_NAME	L"\\DosDevices\\MKTXGetHWInfo"

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

#define MKTX_ERROR_PCI_BUS_NOT_EXIST	(0xE0000001L)
#define MKTX_ERROR_PCI_NO_DEVICE		(0xE0000002L)
#define MKTX_ERROR_PCI_WRITE_CONFIG		(0xE0000003L)
#define MKTX_ERROR_PCI_READ_CONFIG		(0xE0000004L)

#define DRV_OK (DWORD)0
#define DRV_NOT_CREATE (DWORD)1
#define DRV_NOT_START (DWORD)2
#define DRV_NOT_INIT (DWORD)3
#define DRV_ERROR_SCMANAGER (DWORD)4
#define DRV_ERROR_CREATE (DWORD)5
#define DRV_ERROR_START (DWORD)6
#define DRV_ERROR_OPEN (DWORD) 7
#define DRV_ERROR_STOP (DWORD)8
#define DRV_ERROR_UNLOAD (DWORD) 9


class CDriverControl
{
public:

	CDriverControl(
			LPCTSTR lpFilePath,
			LPCTSTR lpServiceName,
			LPCTSTR lpDisplayName,
			DWORD dwStartType
		) :
		m_lpFilePath(lpFilePath),
		m_lpServiceName(lpServiceName),
		m_lpDisplayName(lpDisplayName),
		m_dwStartType(dwStartType),
		m_hService(NULL),
		m_hDriver(INVALID_HANDLE_VALUE),
		m_loaded(FALSE),
		m_started(FALSE)
	{ }

	~CDriverControl();

	DWORD CreateDrv();
	DWORD StartDrv();
	DWORD StopDrv();
	DWORD UnloadDrv();

	static UINT GetPciAddress(byte, byte, byte);

	LPDWORD ReadMsr(OUT UINT, OUT UINT);
	LPDWORD ReadMsrThreadX(OUT UINT, OUT UINT, UINT);
	HRESULT WriteMsr(ULONG);
	HRESULT WriteIoPort(UINT, BYTE);
	LPDWORD ReadPciConfig(UINT, UINT, OUT UINT);
	HRESULT WritePciConfig(UINT, UINT, UINT);
	LPDWORD ReadMemory(ULONG, OUT ULONG);


private:

	CONST UINT m_InvalidPciAddress = 0xFFFFFFFF;

	LPCTSTR m_lpFilePath;
	LPCTSTR m_lpServiceName;
	LPCTSTR m_lpDisplayName;
	DWORD m_dwStartType;
	SC_HANDLE m_hService;
	BOOL m_loaded;
	BOOL m_started;

	HANDLE m_hDriver;

	typedef struct _WRITE_IO_PORT
	{
		UINT portNumber;
		BYTE value;
	} WRITE_IO_PORT, *PWRITE_IO_PORT;

	typedef struct _READ_PCI_CONFIG
	{
		UINT pciAddress;
		UINT regAddress;
	} READ_PCI_CONFIG, *PREAD_PCI_CONFIG;

	typedef struct _WRITE_PCI_CONFIG
	{
		UINT pciAddress;
		UINT regAddress;
		UINT value;
	} WRITE_PCI_CONFIG, *PWRITE_PCI_CONFIG;

	typedef struct _READ_MEMORY_INPUT
	{
		ULONG address;
		UINT unitSize;
		UINT count;
	} READ_MEMORY_INPUT, *PREAD_MEMORY_INPUT;

	inline BOOL IsLoaded();
	inline BOOL IsStarted();

};