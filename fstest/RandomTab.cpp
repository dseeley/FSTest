/*
*  (c) Copyright 2016 Dougal Seeley.  All rights reserved.
*/

// RandomTab.cpp : Defines the entry point for the application.
//
#define STRICT
#define FORMATION_NAME_LEN 30

#include "stdafx.h"
#include "resource.h"
#include "fstest.h"

static LRESULT CALLBACK DlgSelectFormationsProc(HWND, UINT, WPARAM, LPARAM);
static VOID WINAPI OnChildDialogInit(HWND hwndDlg);

extern HINSTANCE g_hinst;   // current instance
extern bool SyncLetterWithName;
extern bool CheatMode;

static int gRandomIndex;    //The current Random


typedef struct tag_RandomFormation_t
{
    char Letter[1];
    int ResourceBw;
    char Name[FORMATION_NAME_LEN];
} RandomFormation_t;

RandomFormation_t AllRandoms[] = {
    'A', IDB_BW_A, "Unipod",
    'B', IDB_BW_B, "Stairstep Diamond",
    'C', IDB_BW_C, "Murphy Flake",
    'D', IDB_BW_D, "Yuan",
    'E', IDB_BW_E, "Meeker",
    'F', IDB_BW_F, "Open Accordian",
    'G', IDB_BW_G, "Cataccord",
    'H', IDB_BW_H, "Bow",
    'J', IDB_BW_J, "Donut",
    'K', IDB_BW_K, "Hook",
    'L', IDB_BW_L, "Adder",
    'M', IDB_BW_M, "Star",
    'N', IDB_BW_N, "Crank",
    'O', IDB_BW_O, "Satellite",
    'P', IDB_BW_P, "Sidebody",
    'Q', IDB_BW_Q, "Phalanx"
};

RandomFormation_t *CurrRandoms = (RandomFormation_t *)malloc(sizeof(AllRandoms));
int CurrRandomsLen;
bool CurrRandomsModified = FALSE;


/*---------------------------------------------------------------*/
// Procedure....: APP_DlgProc()
// Description..: Responds to all messages sent to the dialog
/*---------------------------------------------------------------*/
BOOL CALLBACK DlgRandomProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hwndListLetter = GetDlgItem(hDlg, IDC_FS_ANS_LETTER);
    HWND hwndListName = GetDlgItem(hDlg, IDC_FS_ANS_NAME);
    HWND hwndParent = GetParent(hDlg);

    switch (uiMsg)
    {
        case WM_INITDIALOG:
        {
            int i;

            OnChildDialogInit(hDlg);

            if (CurrRandomsModified == FALSE)
            {
                memcpy(CurrRandoms, AllRandoms, sizeof(AllRandoms));
                CurrRandomsLen = sizeof(AllRandoms) / sizeof(AllRandoms[0]);
            }

            /*Set the icon*/
            SetClassLong(hDlg, GCL_HICON, (DWORD)LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_FSTEST)));

            /*Reset the lists*/
            SendMessage(hwndListLetter, LB_RESETCONTENT, 0, 0);
            SendMessage(hwndListName, LB_RESETCONTENT, 0, 0);

            /*Initialize the list box*/
            for (i = 0; i < CurrRandomsLen; i++)
            {
                SendMessage(hwndListLetter, LB_ADDSTRING, 0, (LPARAM)CurrRandoms[i].Letter);
                SendMessage(hwndListLetter, LB_SETITEMDATA, i, (LPARAM)i);

                /*ItemData gets set to 0 if the ListBox is sorted - dunno why.*/
                SendMessage(hwndListName, LB_ADDSTRING, 0, (LPARAM)CurrRandoms[i].Name);
                SendMessage(hwndListName, LB_SETITEMDATA, i, (LPARAM)i);
            }

            SendMessage(hDlg, WM_COMMAND, IDOK, 0);

            return FALSE;

            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                {
                    HBITMAP myBmp, myBmpOrig;
                    int myRnum = rand();
                    RECT rcPic;
                    HWND hwndPicture = GetDlgItem(hDlg, IDC_FS_PICTURE);
                    LPCTSTR ImgResName = 0;

                    GetWindowRect(hwndPicture, &rcPic);

                    //SendDlgItemMessage(hDlg, IDC_FS_PICTURE, WM_SHOWWINDOW, (WPARAM)SW_SHOW, (LPARAM)0);
                    gRandomIndex = (int)floor(((double)myRnum / RAND_MAX)*CurrRandomsLen);

                    ImgResName = MAKEINTRESOURCE(CurrRandoms[gRandomIndex].ResourceBw);

                    myBmpOrig = (HBITMAP)LoadImage(g_hinst, ImgResName, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
                    myBmp = ScaleBitmap(myBmpOrig, (WORD)(rcPic.right - rcPic.left), (WORD)(rcPic.bottom - rcPic.top));

                    SendDlgItemMessage(hDlg, IDC_FS_PICTURE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)myBmp);

                    /*Clear the result fields*/
                    SetDlgItemText(hDlg, IDC_FS_RESULT_NAME, "");
                    SetDlgItemText(hDlg, IDC_FS_RESULT_LETTER, "");

                    /*Deselect everything*/
                    SendMessage(hwndListName, LB_SETCURSEL, -1, 0L);
                    SendMessage(hwndListLetter, LB_SETCURSEL, -1, 0L);

                    if (CheatMode == TRUE)
                    {
                        SendMessage(hDlg, WM_COMMAND, LOWORD(IDC_GIVEUP), 0);
                    }
                    return FALSE;
                    break;
                }

                case IDC_GIVEUP:
                {
                    /*The letter is easy*/
                    SendMessage(hwndListLetter, LB_SETCURSEL, gRandomIndex, 0L);

                    int SelPos = SendMessage(hwndListName, LB_FINDSTRINGEXACT, 0, (LPARAM)CurrRandoms[gRandomIndex].Name);
                    SendMessage(hwndListName, LB_SETCURSEL, SelPos, 0L);

                    /*Clear the result fields*/
                    SetDlgItemText(hDlg, IDC_FS_RESULT_NAME, "");
                    SetDlgItemText(hDlg, IDC_FS_RESULT_LETTER, "");

                    break;
                }

                case ID_OPTIONS_SELECTFORMATIONS:
                {
                    DialogBox(g_hinst, MAKEINTRESOURCE(IDD_SELECTFORMATIONS), hDlg, (DLGPROC)DlgSelectFormationsProc);
                    break;
                }

                case IDC_FS_ANS_LETTER:
                {
                    switch (HIWORD(wParam))
                    {
                        case LBN_SELCHANGE:
                        {
                            int CurSel = SendMessage(hwndListLetter, LB_GETCURSEL, 0, 0);

                            if (CurSel == gRandomIndex)
                                SetDlgItemText(hDlg, IDC_FS_RESULT_LETTER, "Correct!");
                            else
                                SetDlgItemText(hDlg, IDC_FS_RESULT_LETTER, "Wrong!");
                            break;
                        }
                    }
                    break;
                }

                case IDC_FS_ANS_NAME:
                {
                    switch (HIWORD(wParam))
                    {
                        case LBN_SELCHANGE:
                        {
                            int i;
                            char Buf[FORMATION_NAME_LEN];
                            bool isCorrect = FALSE;
                            int CurSel = SendMessage(hwndListName, LB_GETCURSEL, 0, 0);
                            SendMessage(hwndListName, LB_GETTEXT, CurSel, (LPARAM)Buf);

                            for (i = 0; i < CurrRandomsLen; i++)
                            {
                                if (strcmp(Buf, CurrRandoms[gRandomIndex].Name) == 0)
                                {
                                    SetDlgItemText(hDlg, IDC_FS_RESULT_NAME, "Correct!");
                                    isCorrect = TRUE;
                                    if (SyncLetterWithName == TRUE)	/*Set the Letter*/
                                    {
                                        SendMessage(hwndListLetter, LB_SETCURSEL, gRandomIndex, 0L);
                                        /*Clear the result fields*/
                                        SetDlgItemText(hDlg, IDC_FS_RESULT_LETTER, "");
                                    }
                                }
                            }

                            if (isCorrect != TRUE)
                                SetDlgItemText(hDlg, IDC_FS_RESULT_NAME, "Wrong!");

                            break;
                        }
                    }
                    break;
                }
            }
            break;
    }

    return FALSE;
}


// OnChildDialogInit - Positions the child dialog box to fall
//     within the display area of the tab control.
VOID WINAPI OnChildDialogInit(HWND hwndDlg)
{
    HWND hwndParent = GetParent(hwndDlg);
    DLGHDR *pHdr = (DLGHDR *)GetWindowLong(hwndParent, GWL_USERDATA);
    SetWindowPos(hwndDlg, HWND_TOP, pHdr->rcDisplay.left, pHdr->rcDisplay.top, 0, 0, SWP_NOSIZE);
}


// Mesage handler for Select Formations box.
LRESULT CALLBACK DlgSelectFormationsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndListLetter = GetDlgItem(hDlg, IDC_SELECTFORMATIONS_NAME);
    HWND hwndOwner = GetWindow(hDlg, GW_OWNER);

/*    WORD  HiWwParam = HIWORD(wParam);
    WORD  LoWwParam = LOWORD(wParam);
    WORD  HiWlParam = HIWORD(lParam);
    WORD  LoWlParam = LOWORD(lParam);*/

    switch (message)
    {
        case WM_INITDIALOG:
        {
            int i;

            /*Set the icon*/
            //SetClassLong (hDlg, GCL_HICON, (DWORD) LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_FSTEST)));

            /*Change the text from "Intermediate" to "Junior / Int"*/
            /*HWND hButton = GetDlgItem(hDlg, IDC_SELECTINTERMEDIATEFORMATIONS);
            SetWindowText(hButton, "Junior / Int"); */

            /*Initialize the list box*/
            for (i = 0; i < sizeof(AllRandoms) / sizeof(AllRandoms[0]); i++)
            {
                char myStr[FORMATION_NAME_LEN + 10];
                strcpy_s(myStr, AllRandoms[i].Letter);
                strcat_s(myStr, " - ");
                strcat_s(myStr, AllRandoms[i].Name);

                SendMessage(hwndListLetter, LB_ADDSTRING, 0, (LPARAM)myStr);
                SendMessage(hwndListLetter, LB_SETITEMDATA, i, (LPARAM)i);
            }

            SendMessage(hwndListLetter, LB_SELITEMRANGEEX, (WPARAM)0, (LPARAM) sizeof(AllRandoms) / sizeof(AllRandoms[0]));
            return TRUE;
            break;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDOK:
                {
                    int i, NumItems, Arr[50];
                    NumItems = SendMessage(hwndListLetter, LB_GETSELITEMS, (WPARAM)50, (LPARAM)Arr);

                    for (i = 0; i < NumItems; i++)
                    {
                        memcpy(&CurrRandoms[i], &AllRandoms[Arr[i]], sizeof(AllRandoms[0]));
                    }

                    CurrRandomsLen = NumItems;
                    CurrRandomsModified = TRUE;

                    /*Owner of this dialog is NOT the child window that created it, but ITS owner.*/
                    DLGHDR *pHdr = (DLGHDR *)GetWindowLong(GetParent(hDlg), GWL_USERDATA);
                    SendMessage(pHdr->hwndDisplay, WM_INITDIALOG, (WPARAM)0, (LPARAM)0);

                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                    break;
                }


                case IDC_SELECTAFORMATIONS:
                case IDC_SELECTAAFORMATIONS:
                case IDC_SELECTAAAFORMATIONS:
                {
                    SendMessage(hwndListLetter, LB_SELITEMRANGEEX, (WPARAM)0, (LPARAM) sizeof(AllRandoms) / sizeof(AllRandoms[0]));
                    break;
                }

                case IDCANCEL:
                {
                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                    break;
                }
            }
        }
    }
    return FALSE;
}

