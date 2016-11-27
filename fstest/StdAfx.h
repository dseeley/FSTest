// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// Windows Header Files:
#include <windows.h>

// Force the linker to add the following libraries.
#ifdef _MSC_VER
//#pragma comment(lib, "kernel32.lib")
//#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "gdi32.lib")
//#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Version.lib")
#endif

// C RunTime Header Files
//#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <winver.h>

// Local Header Files
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commdlg.h>


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
