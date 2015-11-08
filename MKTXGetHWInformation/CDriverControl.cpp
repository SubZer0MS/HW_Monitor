#include "stdafx.h"
#include "CDriverControl.h"

CDriverControl::~CDriverControl()
{
	UnloadDrv();

	m_lpFilePath = NULL;
	m_lpServiceName = NULL;
	m_lpDisplayName = NULL;
	m_dwStartType = NULL;
	m_hService = NULL;
	m_loaded = FALSE;
	m_started = FALSE;

}

DWORD CDriverControl::CreateDrv()
{

	if(IsLoaded()) return DRV_OK;

	HRESULT status = DRV_OK;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if(hSCManager == NULL) {
		utils::PrintError(_T(__FUNCTION__));
		status =  DRV_ERROR_SCMANAGER;
		goto CleanUp;
	}

	m_hService = CreateService(
		hSCManager,
		m_lpServiceName,
		m_lpDisplayName,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		m_dwStartType,
		SERVICE_ERROR_IGNORE,
		_T("C:\\Users\\mihsar\\Documents\\Visual Studio 2015\\Projects\\MKTXGetHWInfo\\MKTXGetHWInfo_.sys"), //m_lpFilePath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);

	if(m_hService == NULL) {
		m_hService = OpenService(hSCManager, m_lpServiceName, SERVICE_ALL_ACCESS);

		if(m_hService == NULL) {
			utils::PrintError(_T(__FUNCTION__));
			status =  DRV_ERROR_CREATE;
			goto CleanUp;
		}
	}
	m_loaded = TRUE;


CleanUp:

	CloseServiceHandle(hSCManager);

	return status;
}

DWORD CDriverControl::StartDrv()
{

	if(!IsLoaded()) return DRV_NOT_CREATE;
	if(IsStarted()) return DRV_OK;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if(hSCManager == NULL) return DRV_ERROR_SCMANAGER;

	m_hService = OpenService(hSCManager, m_lpServiceName, SERVICE_ALL_ACCESS);

	if(m_hService == NULL) {
		CloseServiceHandle(hSCManager);
		return DRV_ERROR_OPEN;
	}

	if(StartService(m_hService, 0, NULL) == NULL) {
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(m_hService);
		return DRV_ERROR_START;
	}

	CloseServiceHandle(hSCManager);
	
	LPCTSTR pszFormat = _T("\\\\.\\%s");
	UINT strLen = lstrlen(m_lpServiceName) + lstrlen(pszFormat);
	TCHAR* ppszDest = new TCHAR[strLen];
	StringCchPrintf(ppszDest, strLen, pszFormat, m_lpServiceName);

	m_hDriver = CreateFile(
		ppszDest,
		GENERIC_READ | GENERIC_WRITE,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL
	);
	if(m_hDriver == NULL)
	{
		return DRV_ERROR_START;
	}
	
	m_started = TRUE;

	return DRV_OK;
}

DWORD CDriverControl::StopDrv()
{
	SERVICE_STATUS ss;
	HRESULT status = DRV_OK;

	if(!IsStarted()) return DRV_OK;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(hSCManager == NULL) return DRV_ERROR_SCMANAGER;

	m_hService = OpenService(hSCManager, m_lpServiceName, SERVICE_ALL_ACCESS);

	if(m_hService == NULL) {
		CloseServiceHandle(hSCManager);
		return DRV_ERROR_OPEN;
	}

	status = ControlService(m_hService, SERVICE_CONTROL_STOP, &ss);
	if(status == NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
		status = DRV_ERROR_STOP;

	}

	m_started = FALSE;

	CloseServiceHandle(hSCManager);
	CloseServiceHandle(m_hService);

	return status;
}

DWORD CDriverControl::UnloadDrv()
{
	if(!IsLoaded()) return DRV_OK;
	if(IsStarted() && StopDrv() != DRV_OK) return DRV_ERROR_UNLOAD;

	HRESULT status = ERROR_SUCCESS;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if(hSCManager == NULL) return DRV_ERROR_SCMANAGER;

	m_hService = OpenService(hSCManager, m_lpServiceName, SERVICE_ALL_ACCESS);

	if(m_hService == NULL) {
		CloseServiceHandle(hSCManager);
		return DRV_ERROR_OPEN;
	}

	status = DeleteService(m_hService);
	if(status == NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
	}

	CloseServiceHandle(hSCManager);
	DeleteFile(m_lpFilePath);

	m_loaded = FALSE;

	return DRV_OK;
}

BOOL CDriverControl::IsLoaded()
{
	return m_loaded;
}

BOOL CDriverControl::IsStarted()
{
	return m_started;
}

LPDWORD CDriverControl::ReadMsr(UINT eax, UINT edx)
{
	if(!IsLoaded() || !IsStarted())
	{
		eax = edx = NULL;
		return (LPDWORD)DRV_ERROR_OPEN;
	}

	ULONG outBuffer;
	LPDWORD bytesRead = NULL;

	HRESULT status = DeviceIoControl(
		m_hDriver,
		IOCTL_MKTX_READ_MSR,
		NULL,
		NULL,
		&outBuffer,
		sizeof(outBuffer),
		bytesRead,
		NULL
	);
	if(status == NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
	}
	else
	{
		edx = (UINT)((outBuffer >> 32) & 0xFFFFFFFF);
		eax = (UINT)(outBuffer & 0xFFFFFFFF);
	}
	
	return bytesRead;
}

LPDWORD CDriverControl::ReadMsrThreadX(UINT eax, UINT edx, UINT thread)
{
	LPDWORD bytesRead = NULL;
	DWORD_PTR pThreadMask = NULL;

	pThreadMask = SetThreadAffinityMask(GetCurrentThread(), 1UL << thread);
	if(pThreadMask != NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
	}
	else
	{
		bytesRead = ReadMsr(eax, edx);
		SetThreadAffinityMask(GetCurrentThread(), pThreadMask);
	}
	
	return bytesRead;
}

HRESULT CDriverControl::WriteMsr(ULONG inBuffer)
{
	if(!IsLoaded() || !IsStarted())
	{
		inBuffer = 0;
		return DRV_ERROR_OPEN;
	}

	HRESULT status = DeviceIoControl(
		m_hDriver,
		IOCTL_MKTX_WRITE_MSR,
		&inBuffer,
		sizeof(inBuffer),
		NULL,
		NULL,
		NULL,
		NULL
	);
	if(status == NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
	}

	return status;
}

HRESULT CDriverControl::WriteIoPort(UINT port, BYTE value)
{
	WRITE_IO_PORT input = { port, value };

	HRESULT status = DeviceIoControl(
		m_hDriver,
		IOCTL_MKTX_WRITE_IO_PORT_BYTE,
		&input,
		sizeof(input),
		NULL,
		NULL,
		NULL,
		NULL
	);
	if(status == NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
	}

	return status;
}

UINT CDriverControl::GetPciAddress(byte bus, byte device, byte function)
{
	return (UINT)(((bus & 0xFF) << 8) | ((device & 0x1F) << 3) | (function & 7));
}

LPDWORD CDriverControl::ReadPciConfig(UINT pciAddress, UINT regAddress, OUT UINT value)
{
	value = NULL;
	LPDWORD bytesRead = NULL;
	READ_PCI_CONFIG input = { pciAddress, regAddress };

	HRESULT status = DeviceIoControl(
		m_hDriver,
		IOCTL_MKTX_READ_PCI_CONFIG,
		&input,
		sizeof(input),
		&value,
		sizeof(value),
		bytesRead,
		NULL
	);
	if(status == NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
	}

	return bytesRead;
}

HRESULT CDriverControl::WritePciConfig(UINT pciAddress, UINT regAddress, UINT value)
{
	WRITE_PCI_CONFIG input = { pciAddress, regAddress, value };

	HRESULT status = DeviceIoControl(
		m_hDriver,
		IOCTL_MKTX_WRITE_PCI_CONFIG,
		&input,
		sizeof(input),
		&value,
		sizeof(value),
		NULL,
		NULL
	);
	if(status == NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
	}

	return status;
}

LPDWORD CDriverControl::ReadMemory(ULONG address, OUT ULONG value)
{
	value = NULL;
	LPDWORD bytesRead = NULL;
	READ_MEMORY_INPUT input = { address, 1, (UINT)sizeof(value) };

	HRESULT status = DeviceIoControl(
		m_hDriver,
		IOCTL_MKTX_READ_MEMORY,
		&input,
		sizeof(input),
		&value,
		sizeof(value),
		bytesRead,
		NULL
	);
	if(status == NULL)
	{
		utils::PrintError(_T(__FUNCTION__));
	}

	return bytesRead;
}