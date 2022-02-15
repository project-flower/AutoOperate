// AutoOperate.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "AutoOperate.h"

#define MAX_LOADSTRING 100
#define WM_AUTOOPERATE_NOTIFYICON (WM_APP+100)

// グローバル変数:
bool bOperated = false;
HINSTANCE hInst;                                // 現在のインターフェイス
NOTIFYICONDATA nid;
const int nRetryLimit = 30;
TCHAR szAppName[MAX_LOADSTRING];
TCHAR szTargetClassName[MAX_LOADSTRING];
TCHAR szTargetWindow[MAX_LOADSTRING];
TCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
BOOL CALLBACK       EnumChildProc(HWND hwnd, LPARAM lParam);
BOOL CALLBACK       EnumWindowsProc(HWND hWnd, LPARAM lParam);
BOOL                InitInstance(HINSTANCE, int);
ATOM                MyRegisterClass(HINSTANCE hInstance);
VOID CALLBACK       NotifyIcon(HWND hwnd, LPARAM lParam, BOOL& bHandled);
VOID CALLBACK       OnCommand(HWND hwnd, WPARAM wParam, BOOL& bHandled);
VOID CALLBACK       OnCreate(HWND hwnd);
VOID CALLBACK       OnDestroy(HWND hwnd);
VOID CALLBACK       OnPaint(HWND hwnd);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_AUTOOPERATE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AUTOOPERATE));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex = { 0 };

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AUTOOPERATE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_AUTOOPERATE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return FALSE;
    }

    return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL bHandled = FALSE;

    switch (message) {
    case WM_CREATE:
        OnCreate(hWnd);
        break;

    case WM_COMMAND:
        OnCommand(hWnd, wParam, bHandled);

        if (!bHandled) {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        break;

    case WM_PAINT:
        OnPaint(hWnd);
        break;

    case WM_DESTROY:
        OnDestroy(hWnd);
        break;

    case WM_AUTOOPERATE_NOTIFYICON:
        NotifyIcon(hWnd, lParam, bHandled);

        if (bHandled) {
            return TRUE;
        }

        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    TCHAR className[MAX_LOADSTRING];
    GetClassName(hwnd, className, MAX_LOADSTRING);

    if (_tcscmp(className, szTargetClassName)) {
        return TRUE;
    }

    SendMessage(hwnd, WM_SETTEXT, NULL, reinterpret_cast<LPARAM>(_T("あいうえお")));
    bOperated = true;
    return FALSE;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    TCHAR windowText[MAX_LOADSTRING];

    if (GetWindowText(hwnd, windowText, MAX_LOADSTRING) != 0) {
        if (_tcscmp(windowText, szTargetWindow)) {
            return TRUE;
        }

        EnumChildWindows(hwnd, EnumChildProc, 0);

        if (bOperated) {
            return FALSE;
        }
    }

    return TRUE;
}

VOID CALLBACK NotifyIcon(HWND hwnd, LPARAM lParam, BOOL& bHandled)
{
    TCHAR* szMenu;

    switch (LOWORD(lParam)) {
    case WM_LBUTTONDOWN:
        bOperated = false;
        EnumWindows(EnumWindowsProc, 0);
        break;
    case WM_RBUTTONDOWN:
        POINT point;
        HMENU hMenu, hSubMenu;
        szMenu = MAKEINTRESOURCE(IDM_POPUPMENU);
        hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), szMenu);
        hSubMenu = GetSubMenu(hMenu, 0);
        GetCursorPos(&point);
        SetForegroundWindow(hwnd);

        if (TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, point.x, point.y, 0, hwnd, NULL)) {
            PostMessage(hwnd, WM_NULL, 0, 0);
        }

        bHandled = DestroyMenu(hMenu);
        break;

    default:
        bHandled = FALSE;
        break;
    }
}

VOID CALLBACK OnCommand(HWND hwnd, WPARAM wParam, BOOL& bHandled)
{
    int wmId = LOWORD(wParam);

    // 選択されたメニューの解析:
    switch (wmId) {
    case IDM_EXIT:
        DestroyWindow(hwnd);
        break;

    default:
        bHandled = FALSE;
        return;
    }

    bHandled = TRUE;
}

VOID CALLBACK OnCreate(HWND hwnd)
{
    LoadString(hInst, IDS_APP_NAME, szAppName, MAX_LOADSTRING);
    LoadString(hInst, IDS_TARGET_CLASSNAME, szTargetClassName, MAX_LOADSTRING);
    LoadString(hInst, IDS_TARGET_WINDOW_NAME, szTargetWindow, MAX_LOADSTRING);
    HICON hIcon;
    hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_AUTOOPERATE));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 0;
    nid.uFlags = (NIF_MESSAGE | NIF_ICON | NIF_TIP);
    nid.uCallbackMessage = WM_AUTOOPERATE_NOTIFYICON;
    nid.hIcon = hIcon;

    for (int i = 0; i < nRetryLimit; ++i) {
        if (Shell_NotifyIcon(NIM_ADD, &nid)) {
            break;
        }

        if (GetLastError() != ERROR_TIMEOUT) {
            if (i > nRetryLimit) {
                MessageBox(hwnd, _T("エラー\r\nタスクトレイにアイコンを登録出来ません。"), szTitle, (MB_ICONEXCLAMATION | MB_OK));
                DestroyWindow(hwnd);
                return;
            }
        }
        else if (Shell_NotifyIcon(NIM_MODIFY, &nid)) {
            break;
        }

        Sleep(1000);
    }

    if (hIcon) {
        DestroyIcon(hIcon);
    }
}

VOID CALLBACK OnDestroy(HWND hwnd)
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
    PostQuitMessage(0);
}

VOID CALLBACK OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
}
