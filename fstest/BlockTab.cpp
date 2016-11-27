/*
*  (c) Copyright 2016 Dougal Seeley.  All rights reserved.
*/

// BlockTab.cpp : Defines the entry point for the application.
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
extern bool isColour;

static int gRandomIndex;    //The current Random

typedef struct tag_BlockFormation_t
{
    char Number[3];
    int ResourceBw;
    char Block1[FORMATION_NAME_LEN];
    char Block2[FORMATION_NAME_LEN];
} BlockFormation_t;

BlockFormation_t AllBlocks[] = {
    "1", IDB_BW_1, "Snowflake", "Snowflake",
    "2", IDB_BW_2, "Sidebody Donut", "Sideflake Donut",
    "3", IDB_BW_3, "Side flake opal", "Turf",
    "4", IDB_BW_4, "Monopod", "Monopod",
    "5", IDB_BW_5, "Opal", "Opal",
    "6", IDB_BW_6, "Stardian", "Stardian",
    "7", IDB_BW_7, "Sidebuddies", "Sidebuddies",
    "8", IDB_BW_8, "Canadian Tee", "Canadian Tee",
    "9", IDB_BW_9, "Cat + Accordian", "Cat + Accordian",
    "10", IDB_BW_10, "Diamond", "Bunyip",
    "11", IDB_BW_11, "Photon", "Photon",
    "12", IDB_BW_12, "Bundy", "Bundy",
    "13", IDB_BW_13, "Offset", "Spinner",
    "14", IDB_BW_14, "Bipole", "Bipole",
    "15", IDB_BW_15, "Caterpillar", "Caterpillar",
    "16", IDB_BW_16, "Compressed Accordian", "Box",
    "17", IDB_BW_17, "Danish Tee", "Murphy",
    "18", IDB_BW_18, "Zircon", "Zircon",
    "19", IDB_BW_19, "Ritz", "Icepick",
    "20", IDB_BW_20, "Piver", "Viper",
    "21", IDB_BW_21, "ZigZag", "Marquis",
    "22", IDB_BW_22, "Tee", "Chinese Tee"
};

BlockFormation_t *CurrBlocks = (BlockFormation_t *)malloc(sizeof(AllBlocks));
int CurrBlocksLen;
bool CurrBlocksModified = FALSE;


/*---------------------------------------------------------------*/
// Procedure....: APP_DlgProc()
// Description..: Responds to all messages sent to the dialog
/*---------------------------------------------------------------*/
BOOL CALLBACK DlgBlockProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hwndListNumber = GetDlgItem(hDlg, IDC_TABBLOCKS_NUMBER_ANS);
    HWND hwndListBlock1 = GetDlgItem(hDlg, IDC_TABBLOCKS_BLOCK_ANS1);
    HWND hwndListBlock2 = GetDlgItem(hDlg, IDC_TABBLOCKS_BLOCK_ANS2);
    HWND hwndParent = GetParent(hDlg);

    switch(uiMsg)
    {
    case WM_INITDIALOG:
        {
            int i;

            OnChildDialogInit(hDlg);

            if (CurrBlocksModified == FALSE)
            {
                memcpy(CurrBlocks, AllBlocks, sizeof(AllBlocks));
                CurrBlocksLen = sizeof(AllBlocks)/sizeof(AllBlocks[0]);
            }

            /*Set the icon*/
            SetClassLong (hDlg, GCL_HICON, (DWORD) LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_FSTEST)));

            /*Reset the lists*/
            SendMessage(hwndListNumber, LB_RESETCONTENT, 0, 0);
            SendMessage(hwndListBlock1, LB_RESETCONTENT, 0, 0);
            SendMessage(hwndListBlock2, LB_RESETCONTENT, 0, 0);

            /*Initialize the list boxes*/
            for (i=0; i<CurrBlocksLen; i++)
            {
                SendMessage(hwndListNumber, LB_ADDSTRING, 0, (LPARAM) CurrBlocks[i].Number);
                SendMessage(hwndListNumber, LB_SETITEMDATA, i, (LPARAM) i);

                /*ItemData gets set to 0 if the ListBox is sorted - dunno why.*/
                SendMessage(hwndListBlock1, LB_ADDSTRING, 0, (LPARAM) CurrBlocks[i].Block1);
                SendMessage(hwndListBlock1, LB_SETITEMDATA, i, (LPARAM) i);
                SendMessage(hwndListBlock2, LB_ADDSTRING, 0, (LPARAM) CurrBlocks[i].Block2);
                SendMessage(hwndListBlock2, LB_SETITEMDATA, i, (LPARAM) i);
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
                HWND hwndPicture = GetDlgItem(hDlg, IDC_TABBLOCKS_BLOCKPICTURE);
                LPCTSTR ImgResName=0;

                GetWindowRect(hwndPicture, &rcPic);

                //SendDlgItemMessage(hDlg, IDC_FS_PICTURE, WM_SHOWWINDOW, (WPARAM)SW_SHOW, (LPARAM)0);
                gRandomIndex = (int)floor(((double)myRnum/RAND_MAX)*CurrBlocksLen);

                ImgResName = MAKEINTRESOURCE(CurrBlocks[gRandomIndex].ResourceBw);

                myBmpOrig = (HBITMAP)LoadImage(g_hinst, ImgResName, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
                myBmp = ScaleBitmap(myBmpOrig, (WORD)(rcPic.right - rcPic.left), (WORD)(rcPic.bottom - rcPic.top));

                SendDlgItemMessage(hDlg, IDC_TABBLOCKS_BLOCKPICTURE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)myBmp);

                /*Clear the result fields*/
                SetDlgItemText(hDlg, IDC_TABBLOCKS_BLOCK_RESULT1, "");
                SetDlgItemText(hDlg, IDC_TABBLOCKS_BLOCK_RESULT2, "");
                SetDlgItemText(hDlg, IDC_TABBLOCKS_NUMBER_RESULT, "");

                /*Deselect everything*/
                SendMessage(hwndListBlock1, LB_SETCURSEL, -1, 0L);
                SendMessage(hwndListBlock2, LB_SETCURSEL, -1, 0L);
                SendMessage(hwndListNumber, LB_SETCURSEL, -1, 0L);

                if (CheatMode == TRUE)
                {
                    SendMessage(hDlg, WM_COMMAND, LOWORD(IDC_GIVEUP), 0);
                }

                return FALSE;
                break;
            }

            case IDC_GIVEUP:
            {
                /*The Number is easy*/
                SendMessage(hwndListNumber, LB_SETCURSEL, gRandomIndex, 0L);

                int SelPosBlock1 = SendMessage(hwndListBlock1, LB_FINDSTRINGEXACT, 0, (LPARAM)CurrBlocks[gRandomIndex].Block1);
                SendMessage(hwndListBlock1, LB_SETCURSEL, SelPosBlock1, 0L);
                int SelPosBlock2 = SendMessage(hwndListBlock2, LB_FINDSTRINGEXACT, 0, (LPARAM)CurrBlocks[gRandomIndex].Block2);
                SendMessage(hwndListBlock2, LB_SETCURSEL, SelPosBlock2, 0L);

                /*Clear the result fields*/
                SetDlgItemText(hDlg, IDC_TABBLOCKS_BLOCK_RESULT1, "");
                SetDlgItemText(hDlg, IDC_TABBLOCKS_BLOCK_RESULT2, "");
                SetDlgItemText(hDlg, IDC_TABBLOCKS_NUMBER_RESULT, "");

                break;
            }

            case ID_OPTIONS_SELECTFORMATIONS:
            {
                DialogBox(g_hinst, MAKEINTRESOURCE(IDD_SELECTFORMATIONS), hDlg, (DLGPROC)DlgSelectFormationsProc);
                break;
            }

            case IDC_TABBLOCKS_NUMBER_ANS:
            {
                switch (HIWORD(wParam))
                {
                    case LBN_SELCHANGE:
                    {
                        int CurSel = SendMessage(hwndListNumber, LB_GETCURSEL, 0, 0);

                        if (CurSel == gRandomIndex)
							SetDlgItemText(hDlg, IDC_TABBLOCKS_NUMBER_RESULT, "Correct!");
                        else
							SetDlgItemText(hDlg, IDC_TABBLOCKS_NUMBER_RESULT, "Wrong!");
                        break;
                    }
                }
                break;
            }

            case IDC_TABBLOCKS_BLOCK_ANS1:
            {
                switch (HIWORD(wParam))
                {
                    case LBN_SELCHANGE:
                    {
                        int i;
                        char Buf[FORMATION_NAME_LEN];
                        bool isCorrect = FALSE;
                        int CurSel = SendMessage(hwndListBlock1, LB_GETCURSEL, 0, 0);
                        SendMessage(hwndListBlock1, LB_GETTEXT, CurSel, (LPARAM)Buf);

                        for (i=0; i<CurrBlocksLen; i++)
                        {
                            if (strcmp(Buf, CurrBlocks[gRandomIndex].Block1) == 0)
                            {
								SetDlgItemText(hDlg, IDC_TABBLOCKS_BLOCK_RESULT1, "Correct!");
								isCorrect = TRUE;
								if (SyncLetterWithName == TRUE)	/*Set the Letter*/
                                {
									SendMessage(hwndListNumber, LB_SETCURSEL, gRandomIndex, 0L);
                                    /*Clear the result fields*/
                                    SetDlgItemText(hDlg, IDC_TABBLOCKS_NUMBER_RESULT, "");
                                }
                            }
                        }

                        if (isCorrect!=TRUE)
							SetDlgItemText(hDlg, IDC_TABBLOCKS_BLOCK_RESULT1, "Wrong!");

                        break;
                    }
                }
                break;
            }
            case IDC_TABBLOCKS_BLOCK_ANS2:
            {
                switch (HIWORD(wParam))
                {
                    case LBN_SELCHANGE:
                    {
                        int i;
                        char Buf[FORMATION_NAME_LEN];
                        bool isCorrect = FALSE;
                        int CurSel = SendMessage(hwndListBlock2, LB_GETCURSEL, 0, 0);
                        SendMessage(hwndListBlock2, LB_GETTEXT, CurSel, (LPARAM)Buf);

                        for (i=0; i<CurrBlocksLen; i++)
                        {
                            if (strcmp(Buf, CurrBlocks[gRandomIndex].Block2) == 0)
                            {
								SetDlgItemText(hDlg, IDC_TABBLOCKS_BLOCK_RESULT2, "Correct!");
								isCorrect = TRUE;
								if (SyncLetterWithName == TRUE)	/*Set the Letter*/
                                {
									SendMessage(hwndListNumber, LB_SETCURSEL, gRandomIndex, 0L);
                                    /*Clear the result fields*/
                                    SetDlgItemText(hDlg, IDC_TABBLOCKS_NUMBER_RESULT, "");
                                }
                            }
                        }

                        if (isCorrect!=TRUE)
							SetDlgItemText(hDlg, IDC_TABBLOCKS_BLOCK_RESULT2, "Wrong!");

                        break;
                    }
                }
                break;
            }

        }
        break;

        case WM_CONTEXTMENU:
        {
            //MessageBox(hDlg, "Context Menu", "!", MB_OK);
        }
    }

    return FALSE;
}


// OnChildDialogInit - Positions the child dialog box to fall
//     within the display area of the tab control.
VOID WINAPI OnChildDialogInit(HWND hwndDlg)
{
    HWND hwndParent = GetParent(hwndDlg);
    DLGHDR *pHdr = (DLGHDR *) GetWindowLong(hwndParent, GWL_USERDATA);
    SetWindowPos(hwndDlg, HWND_TOP, pHdr->rcDisplay.left, pHdr->rcDisplay.top, 0, 0, SWP_NOSIZE);
}


// Mesage handler for Select Formations box.
LRESULT CALLBACK DlgSelectFormationsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndListLetter = GetDlgItem(hDlg, IDC_SELECTFORMATIONS_NAME);
    HWND hwndOwner = GetWindow(hDlg, GW_OWNER);

    switch (message)
    {
        case WM_INITDIALOG:
        {
            int i;

            /*Initialize the list box*/
            for (i=0; i<sizeof(AllBlocks)/sizeof(AllBlocks[0]); i++)
            {
                char myStr[FORMATION_NAME_LEN+10];
                wsprintf(myStr, "%s: %s -> %s", AllBlocks[i].Number, AllBlocks[i].Block1, AllBlocks[i].Block2 );

                SendMessage(hwndListLetter, LB_ADDSTRING, 0, (LPARAM) myStr);
                SendMessage(hwndListLetter, LB_SETITEMDATA, i, (LPARAM) i);
            }

            SendMessage( hwndListLetter, LB_SELITEMRANGEEX, (WPARAM) 0, (LPARAM) sizeof(AllBlocks)/sizeof(AllBlocks[0]));
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
                    NumItems = SendMessage( hwndListLetter, LB_GETSELITEMS, (WPARAM) 50, (LPARAM) Arr);

                    for (i=0; i<NumItems; i++)
                    {
                        memcpy(&CurrBlocks[i], &AllBlocks[Arr[i]], sizeof(AllBlocks[0]));
                    }

                    CurrBlocksLen = NumItems;
                    CurrBlocksModified = TRUE;

                    /*Owner of this dialog is NOT the child window that created it, but ITS owner.*/
                    DLGHDR *pHdr = (DLGHDR *) GetWindowLong(GetParent(hDlg), GWL_USERDATA);
                    SendMessage( pHdr->hwndDisplay, WM_INITDIALOG, (WPARAM) 0, (LPARAM) 0);

                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                    break;
                }

                case IDC_SELECTAAAFORMATIONS:
                {
                    SendMessage( hwndListLetter, LB_SELITEMRANGEEX, (WPARAM) 0, (LPARAM) sizeof(AllBlocks)/sizeof(AllBlocks[0]));
                    break;
                }

                case IDC_SELECTAAFORMATIONS:
                {
					/*1,2,4,6,7,8,9,11,13,14,15,18,19,20,21,22*/
                    SendMessage( hwndListLetter, LB_SELITEMRANGEEX, (WPARAM) 0, (LPARAM) sizeof(AllBlocks)/sizeof(AllBlocks[0]));
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 3 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 5 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 10 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 12 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 16 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 17 -1);
                    break;
                }

                case IDC_SELECTAFORMATIONS:
                {
					/*2,4,6,7,8,9,19,21*/
                    SendMessage( hwndListLetter, LB_SELITEMRANGEEX, (WPARAM) 0, (LPARAM) sizeof(AllBlocks)/sizeof(AllBlocks[0]));
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 1 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 3 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 5 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 10 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 11 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 12 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 13 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 14 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 15 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 16 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 17 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 18 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 20 -1);
                    SendMessage( hwndListLetter, LB_SETSEL, (WPARAM) FALSE, (LPARAM) 22 -1);
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

