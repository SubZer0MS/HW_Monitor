#pragma once

#include "resource.h"

#define TRAYICONID	1	// ID number for the Notify Icon
#define SWM_TRAYMSG	WM_APP	// the message ID sent to our window

#define SWM_SHOW	WM_APP + 1	// show the window
#define SWM_HIDE	WM_APP + 2	// hide the window
#define SWM_EXIT	WM_APP + 3	// close the window

// Global Variables:
HINSTANCE hInst;	// current instance
NOTIFYICONDATA niData;	// notify icon data
CDriverControl* pDriver;  // the driver used

BOOL InitInstance(HINSTANCE, INT);
BOOL OnInitDialog(HWND);
VOID ShowContextMenu(HWND);
INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
