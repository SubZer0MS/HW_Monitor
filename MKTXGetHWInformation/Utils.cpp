#include "stdafx.h"

namespace utils
{

	BOOL GetDriverFullPathName(OUT LPCTSTR& lpFilePathFull)
	{
		DWORD dwRetVal = NULL;
		TCHAR lpTempPathBuffer[MAX_PATH];
		TCHAR fileNameFullPath[MAX_PATH];
		HRSRC hRes = NULL;
		HGLOBAL hResourceLoaded = INVALID_HANDLE_VALUE;
		LPVOID lpResLock;
		DWORD fileSize = NULL;
		HANDLE hFile = INVALID_HANDLE_VALUE;

		LPCTSTR pszFormat = _T("%s\%s");

		dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
		if(dwRetVal > MAX_PATH || (dwRetVal == 0)) goto CleanUp;

		hRes = FindResource(
					NULL,
					MAKEINTRESOURCE(IDR_DRIVER_FILE),
					L"BINARY"
				);
		if(hRes == NULL)
		{
			goto CleanUp;
		}

		hResourceLoaded = LoadResource(NULL, hRes);
		if(hResourceLoaded == NULL) goto CleanUp;

		lpResLock = LockResource(hResourceLoaded);
		if(lpResLock == NULL) goto CleanUp;

		fileSize = SizeofResource(NULL, hRes);
		if(fileSize == NULL) goto CleanUp;

		StringCchPrintf(fileNameFullPath, MAX_PATH, pszFormat, lpTempPathBuffer, _T("MKTXGetHWInfo.sys"));

		hFile = CreateFile(
					fileNameFullPath,
					GENERIC_WRITE,
					NULL,
					NULL,
					CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL
				);
		if(hFile == INVALID_HANDLE_VALUE) goto CleanUp;

		dwRetVal = WriteFile(hFile, lpResLock, fileSize, NULL, NULL);
		if(dwRetVal == FALSE) goto CleanUp;

		CloseHandle(hFile);

		lpFilePathFull = fileNameFullPath;

		return ERROR_SUCCESS;


	CleanUp:

		PrintError(_T(__FUNCTION__));

		if(hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
		}

		return NULL;
	}

	VOID PrintError(CONST LPCTSTR& errDesc)
	{
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL
		);

		lpDisplayBuf = (LPVOID)LocalAlloc(
							LMEM_ZEROINIT,
							(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)errDesc) + lstrlen((LPCTSTR)_T(__FUNCTION__)) + 40) * sizeof(TCHAR)
						);

		StringCchPrintf(
			(LPTSTR)lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			_T("%s. %s failed with error %d: %s"),
			errDesc,
			_T(__FUNCTION__),
			dw,
			lpMsgBuf
		);

		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, _T("ERROR"), MB_OK);

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);
	}

	VOID PrintInfo(CONST LPCTSTR pMessage, ...)
	{
		LPWSTR pBuffer = NULL;

		va_list args = NULL;
		va_start(args, pMessage);

		FormatMessage(
			FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			pMessage,
			0,
			0,
			(LPWSTR)&pBuffer,
			0,
			&args);

		va_end(args);

		MessageBox(NULL, (LPCTSTR)pBuffer, _T("INFO"), MB_OK);

		LocalFree(pBuffer);
	}

	const ULONGLONG GetDllVersion(CONST LPCTSTR& lpszDllName)
	{
		ULONGLONG ullVersion = NULL;
		HINSTANCE hinstDll;

		hinstDll = LoadLibrary(lpszDllName);
		if(hinstDll)
		{
			DLLGETVERSIONPROC pDllGetVersion;
			pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");

			if(pDllGetVersion)
			{
				DLLVERSIONINFO dvi;
				HRESULT hr;
				ZeroMemory(&dvi, sizeof(dvi));
				dvi.cbSize = sizeof(dvi);
				hr = (*pDllGetVersion)(&dvi);

				if(SUCCEEDED(hr))
				{
					ullVersion = MAKEDLLVERULL(dvi.dwMajorVersion, dvi.dwMinorVersion, 0, 0);
				}
			}

			FreeLibrary(hinstDll);
		}

		return ullVersion;
	}

	CONST PTCHAR GetStringResource(UINT UId, SIZE_T maxSize)
	{
		TCHAR* pCharBuffer = new TCHAR[maxSize];
		LoadString(GetModuleHandle(NULL), IDI_TOOL_TIP_DEFAULT, pCharBuffer, (INT)maxSize);
		
		return pCharBuffer;
	}

}