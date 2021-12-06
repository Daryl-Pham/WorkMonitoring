#include <iostream>
#include <HookKeyboardAndMouseLib.h>

// http://www.kumei.ne.jp/c_lang/sdk2/sdk_161.htm

#include <tchar.h>
#include <windows.h>


namespace
{

TCHAR szClassName[] = _T("hookmain");        //ウィンドウクラス
BOOL bHook = FALSE;

const UINT MSG_KEYBOARD = WM_APP;
const UINT MSG_MOUSE = WM_APP + 1;

//ウィンドウプロシージャ
static auto kb = []() {
    OutputDebugString(_T("App keyboar callback\n"));
};
static auto mouse = []() {
    OutputDebugString(_T("App mouse callback\n"));
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    int id;
    static HINSTANCE hInst;

    switch (msg) {
    case WM_CREATE:
#if 1
        SetHook(hWnd, MSG_KEYBOARD, MSG_MOUSE);
#else
        SetHook2(kb, mouse);
#endif
        break;
    case WM_CLOSE:
        if (bHook) {
            MessageBox(hWnd, _T("フックが解除されていません"), _T("注意！！"), MB_OK);
            break;
        }
        id = MessageBox(hWnd,
            _T("終了してもよいですか"),
            _T("終了確認"),
            MB_YESNO | MB_ICONQUESTION);
        if (id == IDYES) {
#if 1
            ResetHook();
#else
            ResetHook2();
#endif
            DestroyWindow(hWnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case MSG_KEYBOARD:
        OutputDebugString(_T("App MSG_KEYBOARD\n"));
        break;
    case MSG_MOUSE:
        OutputDebugString(_T("App MSG_MOUSE\n"));
        break;
    default:
        return (DefWindowProc(hWnd, msg, wp, lp));
    }
    return 0;
}

//ウィンドウ・クラスの登録

ATOM InitApp(HINSTANCE hInst)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;    //プロシージャ名
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;        //インスタンス
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = _T("MYMENU");    //メニュー名
    wc.lpszClassName = szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    return (RegisterClassEx(&wc));
}

//ウィンドウの生成
BOOL InitInstance(HINSTANCE hInst, int nCmdShow)
{
    HWND hWnd;

    hWnd = CreateWindow(szClassName,
        _T("HookKeyboardAndMouseApp"),    //タイトルバーにこの名前が表示されます
        WS_OVERLAPPEDWINDOW,    //ウィンドウの種類
        CW_USEDEFAULT,    //Ｘ座標
        CW_USEDEFAULT,    //Ｙ座標
        CW_USEDEFAULT,    //幅
        CW_USEDEFAULT,    //高さ
        NULL,            //親ウィンドウのハンドル、親を作るときはNULL
        NULL,            //メニューハンドル、クラスメニューを使うときはNULL
        hInst,            //インスタンスハンドル
        NULL);
    if (!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

}

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst,
    LPSTR lpsCmdLine, int nCmdShow)
{
    MSG msg;

    if (!InitApp(hCurInst))
        return FALSE;
    if (!InitInstance(hCurInst, nCmdShow))
        return FALSE;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
