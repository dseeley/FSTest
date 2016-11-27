/*
*  (c) Copyright 2016 Dougal Seeley.  All rights reserved.
*/

// fstest.cpp : Defines the entry point for the application.
//
#define STRICT
#define FORMATION_NAME_LEN 30

#include "stdafx.h"
#include "resource.h"
#include "fstest.h"



// Foward declarations of functions included in this code module:
BOOL CALLBACK APP_DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AboutFunc(HWND, UINT, WPARAM, LPARAM);

VOID WINAPI OnTabbedDialogInit(HWND hwndDlg);
DLGTEMPLATE * WINAPI DoLockDlgRes(LPCSTR lpszResName);
VOID WINAPI OnSelChanged(HWND hwndDlg);

// Global Variables:
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // The title bar text

HINSTANCE g_hinst;                              // current instance
bool SyncLetterWithName = FALSE;
bool CheatMode = FALSE;
bool isColour = FALSE;

/*---------------------------------------------------------------*/
// Procedure....: WinMain()
// Description..: Entry point in any Windows program
/*---------------------------------------------------------------*/
int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR      lpCmdLine,
    int         nCmdShow)
{
    g_hinst = hInstance;

    /*Initialise the random seed*/
    srand((int)time(0));

    // Enable common controls
    INITCOMMONCONTROLSEX iccex;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&iccex);

    // Run main dialog
    DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN), GetDesktopWindow(), APP_DlgProc);
    return 0;
}


/*---------------------------------------------------------------*/
// Procedure....: APP_DlgProc()
// Description..: Responds to all messages sent to the dialog
/*---------------------------------------------------------------*/
BOOL CALLBACK APP_DlgProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uiMsg)
    {
        case WM_INITDIALOG:
        {
            OnTabbedDialogInit(hDlg);
            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_OPTIONS_SELECTFORMATIONS:
                {
                    DLGHDR *pHdr = (DLGHDR *)GetWindowLong(hDlg, GWL_USERDATA);
                    //int iSel = TabCtrl_GetCurSel(pHdr->hwndTab); 
                    SendMessage(pHdr->hwndDisplay, WM_COMMAND, LOWORD(ID_OPTIONS_SELECTFORMATIONS), 0);

                    break;
                }

                case ID_OPTIONS_SYNCLETTER:
                {
                    if (SyncLetterWithName == TRUE)
                    {
                        SyncLetterWithName = FALSE;
                        CheckMenuItem(GetMenu(hDlg), ID_OPTIONS_SYNCLETTER, MF_BYCOMMAND | MF_UNCHECKED);
                    }
                    else
                    {
                        SyncLetterWithName = TRUE;
                        CheckMenuItem(GetMenu(hDlg), ID_OPTIONS_SYNCLETTER, MF_BYCOMMAND | MF_CHECKED);
                    }
                    break;
                }

                case ID_OPTIONS_CHEATMODE:
                {
                    /*Don't have SyncLetter active as well*/
                    if (SyncLetterWithName == TRUE)
                    {
                        SyncLetterWithName = FALSE;
                        CheckMenuItem(GetMenu(hDlg), ID_OPTIONS_SYNCLETTER, MF_BYCOMMAND | MF_UNCHECKED);
                    }


                    if (CheatMode == TRUE)
                    {
                        CheatMode = FALSE;
                        CheckMenuItem(GetMenu(hDlg), ID_OPTIONS_CHEATMODE, MF_BYCOMMAND | MF_UNCHECKED);
                        EnableMenuItem(GetMenu(hDlg), ID_OPTIONS_SYNCLETTER, MF_BYCOMMAND | MF_ENABLED);
                    }
                    else
                    {
                        CheatMode = TRUE;
                        CheckMenuItem(GetMenu(hDlg), ID_OPTIONS_CHEATMODE, MF_BYCOMMAND | MF_CHECKED);
                        EnableMenuItem(GetMenu(hDlg), ID_OPTIONS_SYNCLETTER, MF_BYCOMMAND | MF_GRAYED);
                    }

                    break;
                }
                
                case ID_HELP_ABOUT:
                {
                    //display about box
                    DialogBox(g_hinst, MAKEINTRESOURCE(IDD_ABOUT), hDlg, (DLGPROC)AboutFunc);
                    break;
                }

                case IDCANCEL:
                case ID_FILE_EXIT:
                    EndDialog(hDlg, FALSE);
                    return FALSE;
            }
            break;

        case WM_NOTIFY:
        {
            LPNMHDR mWord = (LPNMHDR)(lParam);
            switch (mWord->code)
            {
                case TCN_SELCHANGE:
                {
                    int iPage = TabCtrl_GetCurSel(GetParent(hDlg));
                    OnSelChanged(hDlg);
                }
                break;
            }
            break;
        }
    }

    return FALSE;
}




VOID WINAPI OnTabbedDialogInit(HWND hwndDlg)
{
    DLGHDR *pHdr = (DLGHDR *)LocalAlloc(LPTR, sizeof(DLGHDR));
    DWORD dwDlgBase = GetDialogBaseUnits();
    int cxMargin = LOWORD(dwDlgBase) / 4;
    int cyMargin = HIWORD(dwDlgBase) / 8;
    int EdgeOffset = 8;
    TCITEM tie;

    // Save a pointer to the DLGHDR structure. 
    SetWindowLong(hwndDlg, GWL_USERDATA, (LONG)pHdr);

    // Create the tab control. 
    pHdr->hwndTab = GetDlgItem(hwndDlg, IDC_TABCTRL);

    // Add a tab for each of the three child dialog boxes. 
    tie.mask = TCIF_TEXT;
    tie.pszText = "Randoms";
    TabCtrl_InsertItem(pHdr->hwndTab, 0, &tie);
    tie.pszText = "Blocks";
    TabCtrl_InsertItem(pHdr->hwndTab, 1, &tie);

    // Lock the resources for the three child dialog boxes. 
    pHdr->apRes[0] = DoLockDlgRes(MAKEINTRESOURCE(DLG_TABRANDOMS));
    pHdr->apRes[1] = DoLockDlgRes(MAKEINTRESOURCE(DLG_TABBLOCKS));

    // Calculate the display rectangle. 
    TabCtrl_AdjustRect(pHdr->hwndTab, FALSE, &pHdr->rcDisplay);
    pHdr->rcDisplay.left += 15;
    pHdr->rcDisplay.top += 15;

    /*Create the dialog tabs*/
    pHdr->hwndDisplayArr[0] = CreateDialogIndirect(g_hinst, pHdr->apRes[0], hwndDlg, DlgRandomProc);
    pHdr->hwndDisplayArr[1] = CreateDialogIndirect(g_hinst, pHdr->apRes[1], hwndDlg, DlgBlockProc);

    // Simulate selection of the first item. 
    OnSelChanged(hwndDlg);
}



// DoLockDlgRes - loads and locks a dialog box template resource. 
// Returns the address of the locked resource. 
// lpszResName - name of the resource 
DLGTEMPLATE * WINAPI DoLockDlgRes(LPCSTR lpszResName)
{
    HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG);
    HGLOBAL hglb = LoadResource(g_hinst, hrsrc);
    return (DLGTEMPLATE *)LockResource(hglb);
}


// OnSelChanged - processes the TCN_SELCHANGE notification. 
// hwndDlg - handle to the parent dialog box. 
VOID WINAPI OnSelChanged(HWND hwndDlg)
{
    DLGHDR *pHdr = (DLGHDR *)GetWindowLong(hwndDlg, GWL_USERDATA);
    RECT rcTabCtrl, rcCurDlgWind, rcCurDlgClient, rcMainWin;
    int iSel = TabCtrl_GetCurSel(pHdr->hwndTab);

    // Hide the current child dialog box, if any. 
    if (pHdr->hwndDisplay != NULL)
        ShowWindow(pHdr->hwndDisplay, SW_HIDE);

    // Assign the new current child dialog box. 
    pHdr->hwndDisplay = pHdr->hwndDisplayArr[iSel];

    /*TODO: Resize the tab and the main window to fit (vertical only).*/
    GetWindowRect(hwndDlg, &rcMainWin);
    GetWindowRect(pHdr->hwndDisplay, &rcCurDlgWind);
    GetClientRect(pHdr->hwndDisplay, &rcCurDlgClient);

    /*Get the size of the rect for the TabCtrl that will fit the current dialog*/
    CopyRect(&rcTabCtrl, &rcCurDlgClient);
    TabCtrl_AdjustRect(pHdr->hwndTab, TRUE, &rcTabCtrl);

    SetWindowPos(pHdr->hwndTab, HWND_TOP, 0, 0, (rcTabCtrl.right - rcTabCtrl.left) + 10, (rcTabCtrl.bottom - rcTabCtrl.top) + 10, SWP_NOMOVE);

    /*Size the main window*/
    SetWindowPos(hwndDlg, HWND_TOP, 0, 0, (rcMainWin.right - rcMainWin.left), (rcCurDlgWind.top - rcMainWin.top) + (rcCurDlgWind.bottom - rcCurDlgWind.top) + 25, SWP_NOMOVE);

    /*Redraw the image in this tab, so that the 'colour' option is honoured.*/
    SendMessage(pHdr->hwndDisplay, WM_COMMAND, LOWORD(ID_OPTIONS_COLOURPICS), 0);

    /*Show the new window*/
    ShowWindow(pHdr->hwndDisplayArr[iSel], SW_SHOW);
}



// Mesage handler for about box.
LRESULT CALLBACK AboutFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            char VerString[256], CurrFileString[256], DlgString[256];
            DWORD dwDummy;
            unsigned int uiSize;
            VS_FIXEDFILEINFO *lpffi;

            GetModuleFileName(NULL, CurrFileString, sizeof(CurrFileString));
            DWORD dwSize = GetFileVersionInfoSize(CurrFileString, &dwDummy);
            char *lpData = new char[dwSize];
            GetFileVersionInfo(CurrFileString, 0, dwSize, lpData);
            VerQueryValue(lpData, "\\", (LPVOID *)&lpffi, &uiSize);

            wsprintf(VerString, "%d.%d", HIWORD(lpffi->dwFileVersionMS), LOWORD(lpffi->dwFileVersionMS));
            delete[] lpData;


            wsprintf(DlgString, "Version %s", VerString);
            SetDlgItemText(hDlg, IDC_VER, DlgString);

            return TRUE;
            break;
        }

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
