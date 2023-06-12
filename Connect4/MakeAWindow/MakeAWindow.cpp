#include <windows.h>
#include <d2d1.h>
#include <windowsx.h>
#include <time.h>
#include <cstdio>

#include<string>
#pragma comment(lib, "d2d1")
#include "Position.h"
#include "MinMaxWABPruning.h"

int  turn = -1;

template <class DERIVED_TYPE>
class BaseWindow
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow() : m_hwnd(NULL) { }

    BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = 700,
        int nHeight = 600,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    )
    {
        WNDCLASS wc = { 0 };

        wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();

        RegisterClass(&wc);

        m_hwnd = CreateWindowEx(
            dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
        );

        return (m_hwnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hwnd; }

protected:

    virtual PCWSTR  ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    HWND m_hwnd;
};

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class MainWindow : public BaseWindow<MainWindow>
{
    ID2D1Factory* pFactory;
    ID2D1HwndRenderTarget* pRenderTarget;
    ID2D1SolidColorBrush* pBrush;
    ID2D1SolidColorBrush* YBrush;
    ID2D1SolidColorBrush* RBrush;
    float fullX;
    float fullY;
    int count = 0;
    int column[7] = { 0 };
   
    Position P;
    Col_Score AI;

    void    CalculateLayout();
    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();
    void    PlayerTurn(int);
    void    AITurn();
    int     FindColumn(int);
    void    InsertToken(int);
    int     DisplayGameOverMessageBox();
    
public:

    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL), YBrush(NULL), RBrush(NULL)
    {
    }

    PCWSTR  ClassName() const { return L"Circle Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

int MainWindow::DisplayGameOverMessageBox()
{
    int msgboxID = MessageBox(
        NULL,
        (LPCWSTR)L"GameOver\nDo you want to try again?",
        (LPCWSTR)L"GameOver",
         MB_YESNO | MB_DEFBUTTON2
    );
    
    switch (msgboxID)
    {
    case IDYES:
        PostMessage(m_hwnd, WM_PAINT , 0, 0);
        break;
    case IDNO:
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        break;
    }

    return msgboxID;
}

// Recalculate drawing layout when the size of the window changes.

void MainWindow::CalculateLayout()
{
    if (pRenderTarget != NULL)
    {
        D2D1_SIZE_F size = pRenderTarget->GetSize();
        fullX = size.width;
        fullY = size.height;
    }
}

HRESULT MainWindow::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &pBrush);
            if (SUCCEEDED(hr)) {
                hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &YBrush);
                if (SUCCEEDED(hr)) {
                    hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &RBrush);
                    if (SUCCEEDED(hr))
                    {
                        CalculateLayout();
                    }
                }
            }
            
        }
    }
    return hr;
}

void MainWindow::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
}

void MainWindow::OnPaint()
{
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        HWND hd = GetDesktopWindow();
        
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(
            D2D1::Point2F(100.f, 100.f),
            50.f,
            50.f
        );
        for (int i = 1; i <= 6; i++) {
            pRenderTarget->DrawLine(
                D2D1::Point2F(fullX*i / 7, 0.00),
                D2D1::Point2F(fullX*i / 7, fullY),
                pBrush,
                2.0f
            );
        }
        
        for (int i = 1; i <= 5; i++) {
            pRenderTarget->DrawLine(
                D2D1::Point2F(0.00, fullY *i/ 6),
                D2D1::Point2F(fullX, fullY*i / 6),
                pBrush,
                2.0f
            );
        }
        float placeX = fullX / 14;
        float placeY = fullY / 12;
        for (int i = 0; i <= 6; i++) {
            for (int j = 0; j <= 5; j++) {
                ellipse = D2D1::Ellipse(
                    D2D1::Point2F(placeX+fullX * i / 7, placeY+fullY*j / 6),
                    46.f,
                    46.f
                );
                pRenderTarget->DrawEllipse(ellipse, pBrush, 1.0f);
            }
        }
        
        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}

void MainWindow::InsertToken(int posX) {
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr)) {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);
        pRenderTarget->BeginDraw();
        float placeX = fullX / 14;
        float placeY = fullY / 12;
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(
            D2D1::Point2F(placeX + fullX * posX / 7, fullY - placeY - fullY * column[posX] / 6),
            46.f,
            46.f
        );
        if (count % 2) {
            pRenderTarget->FillEllipse(ellipse, YBrush);
        }
        else {
            pRenderTarget->FillEllipse(ellipse, RBrush);
        }
        
        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}

int MainWindow::FindColumn(int ptx) {
    if (ptx > 0 && ptx <= fullX * 1 / 7) { 
        return 0;
    }
    if (ptx > fullX * 1 / 7 && ptx <= fullX * 2 / 7) { 
        return 1;
    }
    if (ptx > fullX * 2 / 7 && ptx <= fullX * 3 / 7) {
        return 2;
    }
    if (ptx > fullX * 3 / 7 && ptx <= fullX * 4 / 7) {
        return 3;
    }
    if (ptx > fullX * 4 / 7 && ptx <= fullX * 5 / 7) {
        return 4;
    }
    if (ptx > fullX * 5 / 7 && ptx <= fullX * 6 / 7) {
        return 5;
    }
    if (ptx > fullX * 6 / 7 && ptx <= fullX) {
        return 6;
    }
}

void MainWindow::PlayerTurn(int CursorPos) {
    int col = FindColumn(CursorPos);
    if (P.canPlay(col)) {
        P.play(col);
        count++;
        InsertToken(col);
        column[col]++;
        if (P.isWinningMove( true)) {
            P.gameover = true;
            MessageBox(NULL, (LPCWSTR)L"GameOver!Player wins!", (LPCWSTR)L"GameOver", MB_OK); //(LPCWSTR)L"GameOver"
            PostQuitMessage(0);
        }
        if (P.nbMoves() == 42) {
            P.gameover = true;
            MessageBox(NULL, (LPCWSTR)L"GameOver!Draw!", (LPCWSTR)(LPCWSTR)L"GameOver", MB_OK);
            PostQuitMessage(0);
        }
    }
}

void MainWindow::AITurn() {
    AIAGAIN:
    AI = MinMaxWABPruning(P, -1000000000, 1000000000, true, 5);
    if (P.canPlay(AI.COL)) {
        P.AIplay(AI.COL);
        count++;
        InsertToken(AI.COL);
        column[AI.COL]++;
        if (P.isWinningMove(false)) {
            P.gameover = true;
            MessageBox(NULL, (LPCWSTR)L"GameOver!AI wins!", (LPCWSTR)(LPCWSTR)L"GameOver",  MB_OK);
            PostQuitMessage(0);
        }
        if (P.nbMoves() == 42) {
            P.gameover = true;
            MessageBox(NULL, (LPCWSTR)L"GameOver!Draw!", (LPCWSTR)(LPCWSTR)L"GameOver", MB_OK);
            PostQuitMessage(0);
        }
    }
    else {
        goto AIAGAIN;
    }
}

void MainWindow::Resize()
{
    if (pRenderTarget != NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        pRenderTarget->Resize(size);
        CalculateLayout();
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    MainWindow win;

    if (!win.Create(L"Connect 4", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);
    srand(time(NULL));
    turn = rand() % 2;
    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        return 0;

    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        OnPaint();
        return 0;

    case WM_LBUTTONDOWN:
       
        switch (turn) {
        case 0:
            if (!P.gameover) { //player turn
                PlayerTurn(GET_X_LPARAM(lParam));
            }
            if (!P.gameover) { //AI turn
                Sleep(500);
                AITurn();
            }
            break;
        case 1:
            if (!P.gameover) { //AI turn
                AITurn();
            }
            turn = 0;
            break;
        }
        
       
        return 0;
        
    case WM_SIZE:
        Resize();
        return 0;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}