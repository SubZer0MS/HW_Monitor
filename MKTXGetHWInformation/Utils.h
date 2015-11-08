#pragma once

namespace utils
{

	BOOL GetDriverFullPathName(OUT LPCTSTR&);
	CONST ULONGLONG GetDllVersion(CONST LPCTSTR&);
	CONST PTCHAR GetStringResource(UINT, SIZE_T);
	VOID PrintError(CONST LPCTSTR&);
	VOID PrintInfo(CONST LPCTSTR, ...);

}