/*
*  (c) Copyright 2016 Dougal Seeley.  All rights reserved.
*/

#if !defined(__FSTEST_H__)
#define __FSTEST_H__

#define MAX_LOADSTRING 100

BOOL CALLBACK DlgRandomProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgBlockProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

HBITMAP ScaleBitmap(HBITMAP hBmp, WORD wNewWidth, WORD wNewHeight);

#define C_PAGES 2 
 
typedef struct tag_dlghdr { 
    HWND hwndTab;       // tab control 
    HWND hwndDisplay;   // current child dialog box 
    HWND hwndDisplayArr[C_PAGES];   // child dialog box handles
    RECT rcDisplay;     // display rectangle for the tab control 
    DLGTEMPLATE *apRes[C_PAGES]; 
} DLGHDR; 


#endif // !defined(__FSTEST_H__)
