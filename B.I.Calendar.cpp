///////////////////////////////////////////
//
//	Name: B.I.Calendar
//
//	Copyright: Igor Baklikov © 2010-2012
//
//	Author: Igor Baklikov
//
//	Date: 22.02.20 01:20
//
//	Description: Simple Windows Calendar
//
///////////////////////////////////////////


#ifndef UNICODE
#define UNICODE
#endif

#define WM_FLIPPED_TO_TRAY (WM_APP + 1994)

#include <windows.h>
#include <string>
#include "menu.h"

static wchar_t szSetFile[1024], szLangFile[1024], szFile[1024], LngFile[512], FilePath[1024], TempFile[1024], Tray[3][128];
wchar_t days[8][8], monthName[12][32];
static int widtha[] = {0,40,80,120,160,200,240,280};

static HWND Month, Year, Day, DayNumb, Week, hwnd, WeekName, Mon, Tue, Wed, Thu, Fri, Sat, Sun, hTaskBar;
static NOTIFYICONDATAW m_tnd;
static HINSTANCE hThisInstance, hInstance;
static HMENU hMenu, hPopup, hSub, hSub2;
static HFONT hFont, hFont2, hFont4, hFont5, hFont6;
static POINT pt;
wchar_t szCurrentDir[MAX_PATH];
static int nCurrentLanguage = ID_LANGS;
static int nLanguages = 0;
const wchar_t *BackInf[9] = {
      L"Changing the Language...",
      L"Language has been changed. Do you wish to restart programm now?",
      L"Yes",
      L"No",
      L"Warning!!!",
      L"One version of this application is already running!!!",
      L"OK",
      L"ERROR!!!",
      L"This language has been selected! Please, choose another one."
};

LOGFONT LF= {-18, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET,
             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, 0, L"Arial"
            },
            LF2= {-11, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, 0, L"Arial"
                 },
                 LF3= {-20, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET,
                       OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, 0, L"Arial"
                      },
                      LF4= {-25, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, 0, L"Arial"
                           };
HICON hIcon = LoadIconW(hThisInstance, L"AppIconSm");

LRESULT CALLBACK EegDlgProc(HWND hEeg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        SendMessage(hEeg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        hFont6 = CreateFontIndirect(&LF4);
        SendDlgItemMessage(hEeg, IDC_T, WM_SETFONT, (WPARAM)hFont6, MAKELPARAM(TRUE,0));
        return 0;
    }
    break;
    case WM_CTLCOLORSTATIC:
    {
        if((HWND)lParam == GetDlgItem(hEeg, IDC_T))
        {
            SetTextColor((HDC)wParam, RGB(75,75,75));
            SetBkMode((HDC)wParam, TRANSPARENT );
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
    }
    break;
    case WM_PAINT:
    {
        HBITMAP hbmp;
        HDC hdc, MemDC;
        PAINTSTRUCT  ps;
        hbmp = (HBITMAP)LoadBitmapW(hInstance, L"EgG");
        hdc = BeginPaint(hEeg, &ps);
        MemDC = CreateCompatibleDC(hdc);
        SelectObject(MemDC, hbmp);
        BitBlt(hdc, 0, 0, 500, 140, MemDC, 0, 0, SRCCOPY);
        DeleteDC(MemDC);
        DeleteObject(hbmp);
        EndPaint(hEeg, &ps);
    }
    break;
    case WM_COMMAND:
    {
        if(LOWORD(wParam) == IDOK)
        {
            DeleteObject(hFont6);
            EndDialog(hEeg, 0);
        }
    }
    break;
    case WM_DESTROY:
    {
        DeleteObject(hFont6);
        EndDialog(hEeg, 0);
    }
    break;
    case WM_CLOSE:
    {
        DeleteObject(hFont6);
        EndDialog(hEeg, 0);
    }
    break;
    default:
        return false;
    }
    return true;

}

LRESULT CALLBACK WarnDlgProc(HWND hWarn, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        SendMessage(hWarn, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        wchar_t temp[1024], name[1024];
        GetPrivateProfileStringW(L"DIALOG", L"WarningName", BackInf[4], temp, 512, szLangFile);
        swprintf_s(name, L"B.I.Calendar: %s", temp);
        SetWindowTextW(hWarn, name);
        GetPrivateProfileStringW(L"DIALOG", L"Text2", BackInf[5], temp, 512, szLangFile);
        SetDlgItemTextW(hWarn, IDC_TEXT, temp);
        SetDlgItemTextW(hWarn, IDOK, BackInf[6]);
        hFont5 = CreateFontIndirect(&LF3);
        SendDlgItemMessage(hWarn, IDC_TEXT, WM_SETFONT, (WPARAM)hFont5, MAKELPARAM(TRUE,0));
        return 0;
    }
    break;
    case WM_CTLCOLORSTATIC:
    {
        if((HWND)lParam == GetDlgItem(hWarn, IDC_TEXT))
        {
            SetTextColor((HDC)wParam, RGB(75,75,75));
            SetBkMode((HDC)wParam, TRANSPARENT );
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
    }
    break;
    case WM_PAINT:
    {
        HBITMAP hbmp;
        HDC hdc, MemDC;
        PAINTSTRUCT  ps;
        hbmp = (HBITMAP)LoadBitmapW(hInstance, L"WarN");
        hdc = BeginPaint(hWarn, &ps);
        MemDC = CreateCompatibleDC(hdc);
        SelectObject(MemDC, hbmp);
        BitBlt(hdc, 0, 0, 500, 140, MemDC, 0, 0, SRCCOPY);
        DeleteDC(MemDC);
        DeleteObject(hbmp);
        EndPaint(hWarn, &ps);
    }
    break;
    case WM_COMMAND:
    {
        if(LOWORD(wParam) == IDOK)
        {
            DeleteObject(hFont5);
            EndDialog(hWarn, 0);
        }
    }
    break;
    case WM_DESTROY:
    {
        DeleteObject(hFont5);
        EndDialog(hWarn, 0);
    }
    break;
    case WM_CLOSE:
    {
        DeleteObject(hFont5);
        EndDialog(hWarn, 0);
    }
    break;
    default:
        return false;
    }
    return true;
}

LRESULT CALLBACK ErrDlgProc(HWND hErr, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        SendMessage(hErr, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        wchar_t temp[1024], name[1024];
        GetPrivateProfileStringW(L"DIALOG", L"ErrorName", BackInf[7], temp, 512, szLangFile);
        swprintf_s(name, L"B.I.Calendar: %s", temp);
        SetWindowTextW(hErr, name);
        GetPrivateProfileStringW(L"DIALOG", L"Text3", BackInf[8], temp, 512, szLangFile);
        SetDlgItemTextW(hErr, IDC_TEXT, temp);
        hFont4 = CreateFontIndirect(&LF3);
        SendDlgItemMessage(hErr, IDC_TEXT, WM_SETFONT, (WPARAM)hFont4, MAKELPARAM(TRUE,0));
        return 0;
    }
    break;
    case WM_CTLCOLORSTATIC:
    {
        if((HWND)lParam == GetDlgItem(hErr, IDC_TEXT))
        {
            SetTextColor((HDC)wParam, RGB(75,75,75));
            SetBkMode((HDC)wParam, TRANSPARENT );
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
    }
    break;
    case WM_PAINT:
    {
        HBITMAP hbmp;
        HDC hdc, MemDC;
        PAINTSTRUCT  ps;
        hbmp = (HBITMAP)LoadBitmapW(hInstance, L"ErroR");
        hdc = BeginPaint(hErr, &ps);
        MemDC = CreateCompatibleDC(hdc);
        SelectObject(MemDC, hbmp);
        BitBlt(hdc, 0, 0, 500, 140, MemDC, 0, 0, SRCCOPY);
        DeleteDC(MemDC);
        DeleteObject(hbmp);
        EndPaint(hErr, &ps);
    }
    break;
    case WM_COMMAND:
    {
        if(LOWORD(wParam) == IDOK)
        {
            DeleteObject(hFont4);
            EndDialog(hErr, 0);
        }
    }
    break;
    case WM_DESTROY:
    {
        DeleteObject(hFont4);
        EndDialog(hErr, 0);
    }
    break;
    case WM_CLOSE:
    {
        DeleteObject(hFont4);
        EndDialog(hErr, 0);
    }
    break;
    default:
        return false;
    }
    return true;

}

LRESULT WINAPI DefDlgProc2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const wchar_t *AboutBackUp[6] = {
            L"Error...",
            L"About...",
            L"Version:",
            L"Creation date:",
            L"Autor:",
            L"This program and all her parts are completely free!!!"
    };

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        wchar_t Version[1024], CreationDate[1024], Name[1024], DlgName[1024], temp[1024];
        GetPrivateProfileStringW(L"PROGRAMM", L"Version", AboutBackUp[0], Version, 512, szSetFile);
        SetDlgItemTextW(hDlg, IDC_TEXT5, Version);
        GetPrivateProfileStringW(L"PROGRAMM", L"CreationDate", AboutBackUp[0], CreationDate, 512, szSetFile);
        SetDlgItemTextW(hDlg, IDC_TEXT6, CreationDate);
        GetPrivateProfileStringW(L"DIALOG", L"AboutName", AboutBackUp[1], DlgName, 512, szLangFile);
        swprintf_s(Name, L"B.I.Calendar: %s", DlgName);
        SetWindowTextW(hDlg, Name);
        GetPrivateProfileStringW(L"DIALOG", L"Version", AboutBackUp[2], temp, 512, szLangFile);
        SetDlgItemTextW(hDlg, IDC_TEXT2, temp);
        GetPrivateProfileStringW(L"DIALOG", L"Date", AboutBackUp[3], temp, 512, szLangFile);
        SetDlgItemTextW(hDlg, IDC_TEXT3, temp);
        GetPrivateProfileStringW(L"DIALOG", L"Autor", AboutBackUp[4], temp, 512, szLangFile);
        SetDlgItemTextW(hDlg, IDC_TEXT, temp);
        GetPrivateProfileStringW(L"DIALOG", L"Notation", AboutBackUp[5], temp, 512, szLangFile);
        SetDlgItemTextW(hDlg, IDC_TEXT7, temp);
        hFont = CreateFontIndirect(&LF);
        hFont2 = CreateFontIndirect(&LF2);
        SendDlgItemMessage(hDlg, IDC_TEXT, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE,0));
        SendDlgItemMessage(hDlg, IDC_TEXT2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE,0));
        SendDlgItemMessage(hDlg, IDC_TEXT3, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE,0));
        SendDlgItemMessage(hDlg, IDC_TEXT5, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE,0));
        SendDlgItemMessage(hDlg, IDC_TEXT6, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE,0));
        SendDlgItemMessage(hDlg, IDC_TEXT7, WM_SETFONT, (WPARAM)hFont2, MAKELPARAM(TRUE,0));
        return 0;
    }
    break;
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDOK)
        {
            DeleteObject(hFont);
            DeleteObject(hFont2);
            EndDialog(hDlg, 0);
        }
        if (LOWORD(wParam) == ID_AB)
        {
            wchar_t about[1024];
            swprintf_s(about, L"%s\\ReadMe.txt", szCurrentDir);
            ShellExecuteW(NULL, L"open", about, NULL, NULL, SW_SHOWDEFAULT);
        }
        if (LOWORD(wParam) == ID_LINK)
        {
            ShellExecuteW(NULL, L"open", L"http://vk.com/id35869176", NULL, NULL, SW_SHOWNORMAL);
        }
    }
    break;
    case WM_PAINT:
    {
        HBITMAP hbmp;
        HDC hdc, MemDC;
        PAINTSTRUCT  ps;
        hbmp = (HBITMAP)LoadBitmapW(hInstance, L"AbouT");
        hdc = BeginPaint(hDlg, &ps);
        MemDC = CreateCompatibleDC(hdc);
        SelectObject(MemDC, hbmp);
        BitBlt(hdc, 0, 0, 525, 300, MemDC, 0, 0, SRCCOPY);
        DeleteDC(MemDC);
        DeleteObject(hbmp);
        EndPaint(hDlg, &ps);
    }
    break;
    case WM_CTLCOLORSTATIC:
    {
        if((HWND)lParam == GetDlgItem(hDlg, IDC_TEXT7))
        {
            SetTextColor((HDC)wParam, RGB(220,0,0));
            SetBkMode((HDC)wParam, TRANSPARENT );
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        SetTextColor((HDC)wParam, RGB(0,0,0));
        SetBkMode((HDC)wParam, TRANSPARENT );
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    break;
    case WM_DESTROY:
    {
        DeleteObject(hFont);
        DeleteObject(hFont2);
        DestroyIcon(hIcon);
        EndDialog(hDlg, 0);
    }
    break;
    case WM_CLOSE:
    {
        DeleteObject(hFont);
        DeleteObject(hFont2);
        DestroyIcon(hIcon);
        EndDialog(hDlg, 0);
    }
    break;
    default:
        return false;
    }
    return true;
}

int getNumberOfDaysInMonth(int month, int year)
{
    int daysInMonth = 0;
    switch(month)
    {
    case 1:
        daysInMonth = 31;
        break;
    case 2:
        daysInMonth = (year % 4 == 0) ? 29 : 28;
        break;
    case 3:
        daysInMonth = 31;
        break;
    case 4:
        daysInMonth = 30;
        break;
    case 5:
        daysInMonth = 31;
        break;
    case 6:
        daysInMonth = 30;
        break;
    case 7:
        daysInMonth = 31;
        break;
    case 8:
        daysInMonth = 31;
        break;
    case 9:
        daysInMonth = 30;
        break;
    case 10:
        daysInMonth = 31;
        break;
    case 11:
        daysInMonth = 30;
        break;
    case 12:
        daysInMonth = 31;
        break;
    default:
        daysInMonth = 0;
    }
    return daysInMonth;
}

int getWeekDay(int year, int month, int day)
{
    int d = (14 - month) / 12;
    int y = year - d;
    int m = month + 12 * d - 2;
    int weekday = (7000 + (day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12)) % 7;
    if(weekday == 0)
    {
        weekday = 7;
    }
    return weekday;
}

int getWeekNumber(int year, int month, int day, int weekday)
{
    SYSTEMTIME time1 = {static_cast<WORD>(year), 1, 0, 1, 0, 0, 0, 0}, time2 = { static_cast<WORD>(year), static_cast<WORD>(month), 0, static_cast<WORD>(day), 0, 0, 0, 0};
    FILETIME  tm1,tm2;
    __int64 t1,t2,res;

    SystemTimeToFileTime( &time1, &tm1 );
    SystemTimeToFileTime( &time2, &tm2 );
    t1 = ( ( ( __int64 )tm1.dwHighDateTime ) << 32   ) | tm1.dwLowDateTime;
    t2 = ( ( ( __int64 )tm2.dwHighDateTime ) << 32   ) | tm2.dwLowDateTime;

    res = ( t2 - t1 ) / 10000000 /  60 /  60 /  24 ;
    long long weeknumber = (res + (static_cast<long long>(weekday) - 1)) / 7;
    if (weekday <= 4)
    {
        weeknumber = weeknumber + 1;
    }
    return static_cast<int>(weeknumber);
}

void trayNotification(int number)
{
    if ((0 <= number) && (number < 3))
    {
    }
    else
        number = 0;
    if (nullptr == lstrcpynW(m_tnd.szTip, Tray[number], sizeof(m_tnd.szTip) / sizeof(m_tnd.szTip[0]))) {
            return;
    }
    Shell_NotifyIcon (NIM_MODIFY, &m_tnd);
}

void setTrayInfo(void)
{
    const wchar_t *TrayBack[5]= {L"Calendar for current month",L"Baklikov Igor \x00A9 2011-2012",
                           L"(Minimized to Task Bar)",L"(Minimized to Tray)", L"<no data>"
                          };
    wchar_t szParam[1024], Buf[1024], Buf2[4][64], Buf3[1024];
    GetPrivateProfileStringW(L"PROGRAMM", L"Version", TrayBack[4], Buf3, 512, szSetFile);
    for (int i = 0; i < 4; i++)
    {
        swprintf_s(szParam, L"%d\0", i);
        GetPrivateProfileStringW(L"TRAY", szParam, TrayBack[i], Buf, 512, szLangFile);
        wcscpy_s(Buf2[i], Buf);
        if (i > 0)
        {
            swprintf_s(Tray[i-1], L"%s %s\n%s", Buf2[0], Buf3, Buf2[i]);
        }
    }
    trayNotification(0);
}

void setNewLanguage(int i)
{
    wchar_t hI[20] = {};
    _itow_s(i-ID_LANGS, hI, 10);
    GetPrivateProfileStringW(L"LANGFILES", hI, szLangFile, LngFile, 512, TempFile);
    WritePrivateProfileStringW(L"PROGRAMM", L"MainLanguage", LngFile, szSetFile);
    nCurrentLanguage = i;
    GetPrivateProfileStringW(L"PROGRAMM", L"MainLanguage", L"english.lng", LngFile, 512, szSetFile);
    swprintf_s(szLangFile, L"%s\\Languages\\%s", szCurrentDir, LngFile);
    setTrayInfo();
}

void getMainPathes(void)
{
    int i = GetCurrentDirectoryW(MAX_PATH, (wchar_t*)szCurrentDir);
    if (i > 0 && szCurrentDir[i - 1] == L'\\')
    {
        szCurrentDir[i - 1] = L'\0';
    }
    swprintf_s(szSetFile, L"%s\\Settings\\BICalendar.ini", szCurrentDir);
    GetPrivateProfileStringW(L"PROGRAMM", L"MainLanguage", L"english.lng", LngFile, 512, szSetFile);
    swprintf_s(szLangFile, L"%s\\Languages\\%s", szCurrentDir, LngFile);
    swprintf_s(TempFile, L"%s\\Settings\\Calendar.tmp", szCurrentDir);
}

void setMenuText(HMENU hPopup, HMENU hSub, HMENU hSub2)
{
    const wchar_t *menuBak[7] = {L"&Show",L"Minimize to &Tray",L"Minimize to Task &Bar",L"&Languages",L"&Help",L"&Close",L"&About..."};
    const wchar_t *Hots[7] = {L"\tCtrl-Win-C",L"\tCtrl-Win-H",L"\tCtrl-Win-T",L"",L"",L"\tAlt-F4",L"\tCtrl-F1"};
    static wchar_t FileName[MAX_PATH] = {};
    static WIN32_FIND_DATAW wfd;
    wchar_t lStr[1024];
    static int i = 0;
    void *hSearch;
    hSearch = FindFirstFileW(L"Languages\\*.lng", &wfd);
    if (hSearch != INVALID_HANDLE_VALUE)
    {
        do
        {
            swprintf_s(szFile, L"%s", wfd.cFileName);
            swprintf_s(FileName, L"%s", szFile);
            swprintf_s(FilePath, L"%s\\Languages\\%s", szCurrentDir, szFile);
            if (!wcscmp(FilePath, szLangFile))
            {
                nCurrentLanguage = ID_LANGS + nLanguages;
            }
            GetPrivateProfileStringW(L"ABOUT", L"0", L"Bad File...\t(ERROR)", lStr, 512, FilePath);
            AppendMenuW(hSub, MF_STRING, ID_LANGS + nLanguages, lStr);
            SetMenuItemBitmaps(hSub, ID_LANGS + nLanguages, MF_BYCOMMAND, LoadBitmap(hInstance, L"LngUn"),
                               LoadBitmap(hInstance, L"Lng"));
            AppendMenuW(hSub, MF_SEPARATOR, 0, L"");
            wchar_t hCurrrentLanguage[48] = {};
            _itow_s(i, hCurrrentLanguage, 10);
            //
            //MessageBoxW(NULL, hCurrrentLanguage, L"Hi", MB_OK);
            //
            WritePrivateProfileStringW(L"LANGFILES", hCurrrentLanguage, FileName, TempFile);
            nLanguages++;
            i++;
        }
        while (FindNextFileW(hSearch, &wfd));
    }
    else
    {
        AppendMenuW(hSub, MF_STRING, (UINT)NULL, L"No any languages found\t(ER)");
        AppendMenuW(hSub, MF_SEPARATOR, 0, L"");
    }
    FindClose(hSearch);
    wchar_t szParamm[12], menuName[12][64], Buferr[1024];
    for (int i = 0; i < 7; i++)
    {
        swprintf_s(szParamm, L"%d\0", i);
        GetPrivateProfileStringW(L"MENU", szParamm, menuBak[i], Buferr, 512, szLangFile);
        wcscpy_s(menuName[i], Buferr);
        wcscat_s(menuName[i], Hots[i]);
        if (i < 6)
        {
            if (i == 3 || i == 4)
            {
                if (i == 3)
                    ModifyMenuW(hPopup, (UINT)hSub, MF_BYCOMMAND, (UINT)hSub, menuName[i]);
                if (i == 4)
                    ModifyMenuW(hPopup, (UINT)hSub2, MF_BYCOMMAND, (UINT)hSub2, menuName[i]);
            }
            else
                ModifyMenuW(hPopup, IDM_SHOW+i, MF_BYCOMMAND, IDM_SHOW+i, menuName[i]);
        }
        else
        {
            ModifyMenuW(hSub2, IDM_ABOUT, MF_BYCOMMAND, IDM_ABOUT, menuName[i]);
        }
    }
    SetMenuItemBitmaps(hPopup, IDM_SHOW, MF_BYCOMMAND, LoadBitmap(hInstance, L"ShoW"), LoadBitmap(hInstance, L"ShoW"));
    SetMenuItemBitmaps(hPopup, IDM_HIDE, MF_BYCOMMAND, LoadBitmap(hInstance, L"TraY"), LoadBitmap(hInstance, L"TraY"));
    SetMenuItemBitmaps(hPopup, IDM_HIDETB, MF_BYCOMMAND, LoadBitmap(hInstance, L"HidE"), LoadBitmap(hInstance, L"HidE"));
    SetMenuItemBitmaps(hPopup, (UINT)hSub, MF_BYCOMMAND, LoadBitmap(hInstance, L"LngM"), LoadBitmap(hInstance, L"LngM"));
    SetMenuItemBitmaps(hPopup, (UINT)hSub2, MF_BYCOMMAND, LoadBitmap(hInstance, L"HelP"), LoadBitmap(hInstance, L"HelP"));
    SetMenuItemBitmaps(hPopup, IDM_CLOSE, MF_BYCOMMAND, LoadBitmap(hInstance, L"ClosE"), LoadBitmap(hInstance, L"ClosE"));
    SetMenuItemBitmaps(hSub2, IDM_ABOUT, MF_BYCOMMAND, LoadBitmap(hInstance, L"HelP"), LoadBitmap(hInstance, L"HelP"));
    CheckMenuItem(hSub, nCurrentLanguage, MF_CHECKED);
    //SetMenuDefaultItem(hPopup, IDM_SHOW, MF_BYCOMMAND);
    SetMenuDefaultItem(hSub, nCurrentLanguage, MF_BYCOMMAND);
}

void setWindowTitleW(HWND hwnd, int day, int month, wchar_t *yearAr, wchar_t *DayOfWeek)
{
    wchar_t Title[64];
    swprintf_s(Title, L"B.I.Calendar        %.2i.%.2i.%s %s", day, month, yearAr, DayOfWeek);
    SetWindowTextW(hwnd, Title);
}

void getMainWindowText(int month)
{
    const wchar_t *monthRec[12] = {L"JANUARY",L"FEBRUARY",L"MARCH",L"APRIL",L"MAY",L"JUNE",
                             L"JULY",L"AUGUST",L"SEPTEMBER",L"OCTOBER",L"NOVEMBER",L"DECEMBER"
                            };
    const wchar_t *daysRec[8] = {L"Wk",L"Mn",L"Tu",L"Wd",L"Th",L"Fr",L"St",L"Sn"};
    wchar_t szParam[1024], Bufer[1024], szParam2[1024], Bufer2[1024];
    swprintf_s(szParam, L"%d\0", month-1);
    GetPrivateProfileStringW(L"MONTHES", szParam, monthRec[month-1], Bufer, 512, szLangFile);
    wcscpy_s(monthName[month-1], Bufer);
    for (int i = 0; i <= 7; i++)
    {
        swprintf_s(szParam2, L"%d\0", i);
        GetPrivateProfileStringW(L"WEEKDAYS", szParam2, daysRec[i], Bufer2, 512, szLangFile);
        wcscpy_s(days[i], Bufer2);
    }
    SetWindowTextW(Month, monthName[month-1]);
    SetWindowTextW(WeekName, days[0]);
    SetWindowTextW(Mon, days[1]);
    SetWindowTextW(Tue, days[2]);
    SetWindowTextW(Wed, days[3]);
    SetWindowTextW(Thu, days[4]);
    SetWindowTextW(Fri, days[5]);
    SetWindowTextW(Sat, days[6]);
    SetWindowTextW(Sun, days[7]);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    int year = st.wYear;
    int month = st.wMonth;
    int day = st.wDay;
    int daysInMonth = getNumberOfDaysInMonth(month, year);
    wchar_t yearName[MAX_PATH];
    _itow_s(year, yearName, 10);
    int weekday = getWeekDay(year, month, 1);
    int weekday2 = getWeekDay(year, 1, 1);
    int weekday3 = getWeekDay(year, month, day);
    int WN = getWeekNumber(year, month, 1, weekday2);
    wchar_t WNA[MAX_PATH];
    switch (message)
    {
    case WM_DESTROY:
    {
        //UnregisterHotKey(hwnd, 0);
        //UnregisterHotKey(hwnd, 1);
        //UnregisterHotKey(hwnd, 2);
        Shell_NotifyIcon (NIM_DELETE, &m_tnd);
        DestroyIcon(hIcon);
        DeleteFile(TempFile);
        DestroyWindow(hwnd);
        PostQuitMessage (0);
    }
    break;
    case WM_FLIPPED_TO_TRAY:
    {
        if(wParam == 1994 && lParam == WM_LBUTTONDBLCLK)
        {
            if(IsIconic(hwnd) == TRUE)
            {
                FlashWindow(hwnd, true);
                ShowWindow(hwnd, SW_SHOW);
                ShowWindow(hwnd, SW_RESTORE);
                trayNotification(0);
            }
            else
            {
                ShowWindow(hwnd, SW_MINIMIZE);
            }
        }
        if(wParam == 1994 && lParam == WM_RBUTTONUP)
        {
            hMenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(ID_MENU));
            hPopup = GetSubMenu(hMenu, 0);
            hSub = GetSubMenu(hPopup, 5);
            hSub2 = GetSubMenu(hPopup, 6);
            setMenuText(hPopup, hSub, hSub2);
            if (month == 6 && day == 23)
            {
                AppendMenuW(hSub2, MF_POPUP, IDM_EEGG, L"EaStEr &EgG_)))");
                SetMenuItemBitmaps(hSub2, IDM_EEGG, MF_BYCOMMAND, LoadBitmap(hInstance, L"EgGy"), LoadBitmap(hInstance, L"EgGy"));
                AppendMenuW(hSub2, MF_SEPARATOR, 0, L"");
                SetMenuDefaultItem(hSub2, IDM_EEGG, MF_BYCOMMAND);
            }
            SetForegroundWindow(hwnd);
            GetCursorPos(&pt);
            TrackPopupMenu(hPopup, 0, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        }
    }
    break;
    case WM_SIZE:
    {
        if(wParam == SIZE_MINIMIZED)
        {
            trayNotification(1);
        }
        else if(wParam == SIZE_RESTORED)
        {
            trayNotification(0);
        }
        else
        {
            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        }
    }
    break;
    /*case WM_HOTKEY:
    {
        switch (wParam)
        {
        case 0:
            SendMessage(hwnd, WM_COMMAND, LOWORD(IDM_SHOW), (LPARAM) NULL);
            break;
        case 1:
            SendMessage(hwnd, WM_COMMAND, LOWORD(IDM_HIDE), (LPARAM) NULL);
            break;
        case 2:
            SendMessage(hwnd, WM_COMMAND, LOWORD(IDM_HIDETB), (LPARAM) NULL);
            break;
        }
    }
    break;*/
    case WM_COMMAND:
    {
        if (wParam == 1)
        {
            SendMessage(hwnd, WM_FLIPPED_TO_TRAY, 1994, WM_RBUTTONUP);
        }
        if (wParam == nCurrentLanguage)
        {
            FlashWindow(hwnd, true);
            DialogBox(hThisInstance, MAKEINTRESOURCE (IDD_ERROR), hwnd, (DLGPROC)ErrDlgProc);
        }
        if (wParam != nCurrentLanguage)
        {
            for (UINT i = ID_LANGS; i < (ID_LANGS + (UINT)nLanguages); i++)
            {
                if (i == wParam)
                {
                    setNewLanguage(i);
                    getMainWindowText(month);
                    setWindowTitleW(hwnd, day, month, yearName, days[weekday3]);
                }
            }
        }
        if (LOWORD(wParam == IDM_SHOW))
        {
            if (IsIconic(hwnd) == TRUE)
            {
                ShowWindow(hwnd, SW_SHOW);
                ShowWindow(hwnd, SW_RESTORE);
                trayNotification(0);
            }
            else
                FlashWindow(hwnd, true);
        }
        if (LOWORD(wParam == IDM_HIDE))
        {
            if (IsIconic(hwnd) == false)
            {
                ShowWindow(hwnd, SW_MINIMIZE);
            }
            ShowWindow(hwnd, SW_HIDE);
            trayNotification(2);
        }
        if (LOWORD(wParam == IDM_HIDETB))
        {
            if (IsIconic(hwnd) == false)
            {
                ShowWindow(hwnd, SW_MINIMIZE);
            }
            else
            {
                FlashWindow(hwnd, true);
                ShowWindow(hwnd, SW_SHOW);
            }
            trayNotification(1);
        }
        if (LOWORD(wParam == IDM_ABOUT))
        {
            DialogBox(hThisInstance, MAKEINTRESOURCE (IDD_DIALOG), hwnd, (DLGPROC)DefDlgProc2);
        }
        if (LOWORD(wParam == IDM_CLOSE))
        {
            SendMessage(hwnd, WM_DESTROY, (WPARAM)NULL, (LPARAM)NULL);
        }
        if (LOWORD(wParam == IDM_EEGG))
        {
            DialogBox(hThisInstance, MAKEINTRESOURCE (IDD_EEG), hwnd, (DLGPROC)EegDlgProc);
        }
    }
    break;
    case WM_CREATE:
    {
        //RegisterHotKey(hwnd, 0, MOD_CONTROL + MOD_WIN, 67);
        //RegisterHotKey(hwnd, 1, MOD_CONTROL + MOD_WIN, 72);
        //RegisterHotKey(hwnd, 2, MOD_CONTROL + MOD_WIN, 84);
        int high = 50;
        int count = 0;
        int width = 0;
        getMainWindowText(month);
        Month = CreateWindowW(L"BUTTON", monthName[month - 1], WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 40, 0, 200, 25, hwnd, NULL, 0, 0);
        Year = CreateWindowW(L"BUTTON", yearName, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 240, 0, 80, 25, hwnd, NULL, 0, 0);
        int high2 = 20;
        for (int w = 1; w < 7; w++)
        {
            high2 = high2 + 30;
            _itow_s(WN, WNA, 10);
            Week = CreateWindowW(L"BUTTON", WNA, WS_CHILD | WS_VISIBLE, 0, high2, 40, 30, hwnd, NULL, 0, 0);
            EnableWindow(Week, false);
            WN++;
        }
        WeekName = CreateWindowW(L"BUTTON", days[0], WS_CHILD | WS_VISIBLE, widtha[0], 25, 40, 25, hwnd, NULL, 0, 0);
        Mon = CreateWindowW(L"BUTTON", days[1], WS_CHILD | WS_VISIBLE, widtha[1], 25, 40, 25, hwnd, NULL, 0, 0);
        Tue = CreateWindowW(L"BUTTON", days[2], WS_CHILD | WS_VISIBLE, widtha[2], 25, 40, 25, hwnd, NULL, 0, 0);
        Wed = CreateWindowW(L"BUTTON", days[3], WS_CHILD | WS_VISIBLE, widtha[3], 25, 40, 25, hwnd, NULL, 0, 0);
        Thu = CreateWindowW(L"BUTTON", days[4], WS_CHILD | WS_VISIBLE, widtha[4], 25, 40, 25, hwnd, NULL, 0, 0);
        Fri = CreateWindowW(L"BUTTON", days[5], WS_CHILD | WS_VISIBLE, widtha[5], 25, 40, 25, hwnd, NULL, 0, 0);
        Sat = CreateWindowW(L"BUTTON", days[6], WS_CHILD | WS_VISIBLE, widtha[6], 25, 40, 25, hwnd, NULL, 0, 0);
        Sun = CreateWindowW(L"BUTTON", days[7], WS_CHILD | WS_VISIBLE, widtha[7], 25, 40, 25, hwnd, NULL, 0, 0);
        EnableWindow(WeekName, false);
        EnableWindow(Sat, false);
        EnableWindow(Sun, false);
        if (weekday > 1)
        {
            int s = 0;
            for (int j = 1; j < weekday; j++)
            {
                width = width + 40;
                HWND Nul = CreateWindowW(L"BUTTON", L" ", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, s = s + 40, 50, 40, 30, hwnd, NULL, 0, 0);
                SendMessage(Nul, BM_SETSTATE, true, 0L);
                EnableWindow(Nul, false);
            }
        }
        else
        {
            high = 20;
        }
        for (int j = weekday; j < 43; j++)
        {
            width = width + 40;
            const wchar_t* daynum[32]= {L"1",L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"10",L"11",L"12",L"13",L"14",L"15",
                                  L"16",L"17",L"18",L"19",L"20",L"21",L"22",L"23",L"24",L"25",L"26",L"27",L"28",L"29",L"30",L"31",L" "
                                 };
            if ((j - 1) % 7 == 0)
            {
                width = 40;
                high = high + 30;
            }
            if ((j - 1) % 7 - 6 == 0 || (j - 1) % 7 == 0 || count > daysInMonth)
            {
                EnableWindow(DayNumb, false);
            }
            if (count >= daysInMonth)
            {
                count = 31;
            }
            DayNumb = CreateWindowW(L"BUTTON", daynum[count++], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                    width, high, 40, 30, hwnd, NULL, 0, 0);
            if (count == day || count > 31)
            {
                SendMessage(DayNumb, BM_SETSTATE, true, 0L);
                EnableWindow(DayNumb, false);
            }
            Day = CreateWindowW(L"BUTTON", L"!!!!!", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 0, 0, 40, 25, hwnd, (HMENU) 1, 0, 0);
        }
        setWindowTitleW(hwnd, day, month, yearName, days[weekday3]);
        return 0;
    }
    break;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

wchar_t szClassName[ ] = L"IG_CALENDAR";

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
    getMainPathes();

    hInstance = (HMODULE) GetModuleHandle(NULL);
    hIcon = LoadIconW(hInstance, L"AppIconSm");

    HANDLE NODUBSTART = CreateMutexW(NULL, false, L"IG_CALENDAR");
    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        DialogBox(hThisInstance, MAKEINTRESOURCE (IDD_WARNING), hwnd, (DLGPROC)WarnDlgProc);
        CloseHandle(NODUBSTART);
        return 0;
    }

    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIconW(hThisInstance, L"AppIcon");
    wincl.hIconSm = LoadIconW(hThisInstance, L"AppIconSm");
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+4);

    if (!RegisterClassEx (&wincl))
        return 0;

    RECT r, tr;
    GetWindowRect(GetDesktopWindow(), &r);
    hTaskBar = FindWindow(L"Shell_TrayWnd", NULL);
    GetWindowRect(hTaskBar, &tr);
    if (tr.top == r.top && tr.left == r.left && tr.right == r.right)
    {
        r.bottom = r.bottom + tr.bottom;
        r.right = tr.right;
    }
    if (tr.bottom == r.bottom && tr.left == r.left && tr.right == r.right)
    {
        r.bottom = tr.top;
        r.right = tr.right;
    }
    if (tr.top == r.top && tr.bottom == r.bottom && tr.left == r.left)
    {
        r.bottom = tr.bottom;
        r.right = r.right + tr.right;
    }
    if (tr.top == r.top && tr.bottom == r.bottom && tr.right == r.right)
    {
        r.bottom = tr.bottom;
        r.right = tr.left;
    }
    else
    {
        r.bottom = r.bottom;
        r.right = r.right;
    }
    
    int wdt = ((r.right - 326) / 2);
    int high = ((r.bottom - 258) / 2);

    hwnd = CreateWindowExW(0, szClassName, L"B.I.Calendar", (WS_OVERLAPPEDWINDOW)& ~(WS_MAXIMIZEBOX | WS_THICKFRAME), wdt, high,
                           326, 258, HWND_DESKTOP, NULL, hThisInstance, NULL);
    FlashWindow(hwnd, true);
    setTrayInfo();

    CreateFile(TempFile, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_NEW,
               FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    memset(&m_tnd, 0, sizeof(m_tnd));
    m_tnd.cbSize = sizeof(NOTIFYICONDATAW);
    m_tnd.hWnd = hwnd;
    m_tnd.uID = 1994;
    m_tnd.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    m_tnd.hIcon = LoadIconW(hThisInstance, L"AppIconSm");
    if (nullptr == lstrcpynW(m_tnd.szTip, L"Data Processing Error", sizeof(m_tnd.szTip) / sizeof(m_tnd.szTip[0]))) {
            return -1;
    }
    m_tnd.uCallbackMessage = WM_FLIPPED_TO_TRAY;
    Shell_NotifyIcon (NIM_ADD, &m_tnd);
    trayNotification(0);

    nFunsterStil = SW_SHOWDEFAULT;
    ShowWindow (hwnd, nFunsterStil);
    UpdateWindow(hwnd);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}
