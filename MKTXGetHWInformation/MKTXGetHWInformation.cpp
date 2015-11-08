// MKTXGetHWInformation.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MKTXGetHWInformation.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	if (!InitInstance(hInstance, nCmdShow)) return FALSE;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_STEALTHDIALOG);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg) ||
			!IsDialogMessage(msg.hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	hInst = hInstance;
	HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DLG_DIALOG), NULL, (DLGPROC)DlgProc);
	if(!hWnd) {
		return FALSE;
	}

	ZeroMemory(&niData, sizeof(NOTIFYICONDATA));

	ULONGLONG ullVersion = utils::GetDllVersion(_T("Shell32.dll"));
	if(ullVersion >= MAKEDLLVERULL(5, 0, 0, 0))
	{
		niData.cbSize = sizeof(NOTIFYICONDATA);
	}
	else
	{
		niData.cbSize = NOTIFYICONDATA_V2_SIZE;
	}

	niData.uID = TRAYICONID;
	niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	niData.hWnd = hWnd;
	niData.uCallbackMessage = SWM_TRAYMSG;

	niData.hIcon = (HICON)LoadImage(
		hInstance,
		MAKEINTRESOURCE(IDI_STEALTHDLG),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR
	);

	SIZE_T strlen = sizeof(niData.szTip) / sizeof(TCHAR);
	StringCchCopy(niData.szTip, strlen, utils::GetStringResource(IDI_TOOL_TIP_DEFAULT, strlen));

	Shell_NotifyIcon(NIM_ADD, &niData);

	if(niData.hIcon && DestroyIcon(niData.hIcon))
	{
		niData.hIcon = NULL;
	}

	LPCTSTR lpszDriverFullPathName = NULL;
	utils::GetDriverFullPathName(lpszDriverFullPathName);
	
	TCHAR pszDriverFullPathName[MAX_PATH];
	StringCchCopy(pszDriverFullPathName, MAX_PATH, lpszDriverFullPathName);

	pDriver = new CDriverControl(
		pszDriverFullPathName,
		utils::GetStringResource(IDS_SERVICE_NAME, MAX_PATH),
		utils::GetStringResource(IDS_SERVICE_DISPLAY_NAME, MAX_PATH),
		SERVICE_DEMAND_START
	);

	DWORD dwResult = pDriver->CreateDrv();
	if(dwResult != DRV_OK)
	{
		utils::PrintError(_T("ERROR: The driver has failed to get created."));
		pDriver->UnloadDrv();
		return FALSE;
	}
	
	dwResult = pDriver->StartDrv();
	if(dwResult != DRV_OK)
	{
		utils::PrintError(_T("ERROR: The driver failed to start."));
		pDriver->UnloadDrv();
		return FALSE;
	}

	return TRUE;
}

BOOL OnInitDialog(HWND hWnd)
{
	HMENU hMenu = GetSystemMenu(hWnd, FALSE);

	if(hMenu)
	{
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, IDM_ABOUT, _T("About"));
	}

	HICON hIcon = (HICON)LoadImage(
					hInst,
					MAKEINTRESOURCE(IDI_STEALTHDLG),
					IMAGE_ICON,
					0,
					0,
					LR_SHARED | LR_DEFAULTSIZE
				);

	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	return TRUE;
}

void ShowContextMenu(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	
	if(hMenu)
	{
		if(IsWindowVisible(hWnd))
		{
			InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_HIDE, _T("Hide"));
		}
		else
		{
			InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_SHOW, _T("Show"));
		}

		InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_EXIT, _T("Exit"));

		SetForegroundWindow(hWnd);

		TrackPopupMenu(hMenu, TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);
		DestroyMenu(hMenu);
	}
}

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch(message)
	{
	case SWM_TRAYMSG:
		switch (lParam)
		{
		case WM_LBUTTONDBLCLK:
			ShowWindow(hWnd, SW_RESTORE);
			break;

		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
			ShowContextMenu(hWnd);
			break;
		}
		break;

	case WM_SYSCOMMAND:
		if((wParam & 0xFFF0) == SC_MINIMIZE)
		{
			ShowWindow(hWnd, SW_HIDE);
			return 1;
		}
		else if(wParam == IDM_ABOUT)
		{
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
		}
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case SWM_SHOW:
			ShowWindow(hWnd, SW_RESTORE);
			break;

		case SWM_HIDE:
		case IDOK:
			ShowWindow(hWnd, SW_HIDE);
			break;

		case SWM_EXIT:
			DestroyWindow(hWnd);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		}

		return TRUE;

	case WM_INITDIALOG:
		return OnInitDialog(hWnd);

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		niData.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, &niData);
		pDriver->UnloadDrv();
		PostQuitMessage(0);
		break;
	}

	return FALSE;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}

	return FALSE;
}
