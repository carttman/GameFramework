#include "OmokGame.h"

#include <iostream>
#include <ostream>

#include "GameTimer.h"
#include "Utillity.h"
#include "tchar.h"

bool OmokGame::Initialize()
{
    m_pGameTimer = new GameTimer();
    m_pGameTimer->Reset();

    const wchar_t* className = L"OmokGame";
    const wchar_t* windowName = L"OmokGame";

    if (false == __super::Create(className, windowName, 1024, 720))
    {
        return false;
    }


    RECT rcClient = {};
    GetClientRect(m_hWnd, &rcClient);
    m_width = rcClient.right - rcClient.left;
    m_height = rcClient.bottom - rcClient.top;

    m_hFrontDC = GetDC(m_hWnd);
    m_hBackDC = CreateCompatibleDC(m_hFrontDC);
    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    m_hDefaultBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    m_BoardOffsetX = (m_width - BOARD_PIXEL_SIZE) / 2;
    m_BoardOffsetY = (m_height - BOARD_PIXEL_SIZE) / 2;

    return true;
}

void OmokGame::Run()
{
    MSG msg = {0};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                OnLButtonDown();
            }
            else if (msg.message == WM_RBUTTONDOWN)
            {
                OnRButtonDown();
            }
            else if (msg.message == WM_MOUSEMOVE)
            {
                OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            Update();
            Render();
        }
    }
}

void OmokGame::Finalize()
{
    delete m_pGameTimer;
    m_pGameTimer = nullptr;

    __super::Destroy();
}

void OmokGame::Update()
{
    m_pGameTimer->Tick();

    LogicUpdate();

    m_fDeltaTime = m_pGameTimer->DeltaTimeMS();
    m_fFrameCount += m_fDeltaTime;

    while (m_fFrameCount >= 200.0f)
    {
        FixedUpdate();
        m_fFrameCount -= 200.0f;
    }
}

void OmokGame::Render()
{
    //Clear the back buffer
    ::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);

    DrawBoard(m_hBackDC);

    TurnText(m_hBackDC);
    HelpText(m_hBackDC);
    
    BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void OmokGame::DrawBoard(HDC hdc)
{
    const int boardLeft = m_BoardOffsetX;
    const int boardTop = m_BoardOffsetY;
    const int boardRight = boardLeft + BOARD_PIXEL_SIZE;
    const int boardBottom = boardTop + BOARD_PIXEL_SIZE;

    // 판 배경
    HBRUSH hBoardBrush = CreateSolidBrush(RGB(220, 179, 92));
    HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBoardBrush));
    HPEN hNoPen = static_cast<HPEN>(GetStockObject(NULL_PEN));
    HPEN hOldPen = static_cast<HPEN>(SelectObject(hdc, hNoPen));
    Rectangle(hdc, boardLeft, boardTop, boardRight + 1, boardBottom + 1);
    SelectObject(hdc, hOldPen);

    // 격자선
    HPEN hLinePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, hLinePen);

    const int gridLeft = boardLeft + BOARD_PADDING;
    const int gridTop = boardTop + BOARD_PADDING;
    const int gridRight = gridLeft + BOARD_CELL_SIZE * (BOARD_LINE_COUNT - 1);
    const int gridBottom = gridTop + BOARD_CELL_SIZE * (BOARD_LINE_COUNT - 1);

    for (int i = 0; i < BOARD_LINE_COUNT; ++i)
    {
        const int y = gridTop + i * BOARD_CELL_SIZE;
        MoveToEx(hdc, gridLeft, y, nullptr);
        LineTo(hdc, gridRight, y);

        const int x = gridLeft + i * BOARD_CELL_SIZE;
        MoveToEx(hdc, x, gridTop, nullptr);
        LineTo(hdc, x, gridBottom);
    }

    // 마우스 위치로 해당 격자의 위치 판별
    for (int i = 0; i < BOARD_LINE_COUNT; i++)
    {
        int currY = gridTop + i * BOARD_CELL_SIZE;

        for (int j = 0; j < BOARD_LINE_COUNT; j++)
        {
            int currX = gridLeft + j * BOARD_CELL_SIZE;

            if (m_MousePos.x >= currX - 10 && m_MousePos.x <= currX + 10)
                if (m_MousePos.y >= currY - 10 && m_MousePos.y <= currY + 10)
                {
                    Ellipse(hdc, currX - 10, currY - 10, currX + 10, currY + 10);

                    // 격자 안에서만 좌클릭 이벤트 받는다
                    if (m_MousePos.x >= gridLeft && m_MousePos.x <= gridRight)
                        if (m_MousePos.y >= gridTop && m_MousePos.y <= gridBottom)
                        {
                            if (m_IsLClicked)
                            {
                                m_IsLClicked = false;

                                if (Board[i][j])
                                    continue;

                                TurnText(hdc);
                                Board[i][j] = static_cast<int>(m_CurrTurn);
                                CheckGameOver();
                                
                                if (m_CurrTurn == Turn::BLACK)
                                    m_CurrTurn = Turn::WHITE;
                                else if (m_CurrTurn == Turn::WHITE)
                                    m_CurrTurn = Turn::BLACK;
                            }
                        }
                }
        }
    }
    
    DrawGOStone(hdc, gridTop, gridLeft);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBoardBrush);
    DeleteObject(hLinePen);
}

void OmokGame::DrawGOStone(HDC hdc, int top, int left) const
{
    // 바둑돌 브러시 생성
    HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH hOldStoneBrush = static_cast<HBRUSH>(SelectObject(hdc, hBlackBrush));

    for (int i = 0; i < BOARD_LINE_COUNT; i++)
    {
        int currY = top + i * BOARD_CELL_SIZE;

        for (int j = 0; j < BOARD_LINE_COUNT; j++)
        {
            int currX = left + j * BOARD_CELL_SIZE;

            if (!Board[i][j])
                continue;

            if (Board[i][j] == static_cast<int>(Turn::BLACK))
                SelectObject(hdc, hBlackBrush);
            else if (Board[i][j] == static_cast<int>(Turn::WHITE))
                SelectObject(hdc, hWhiteBrush);

            Ellipse(hdc, currX - 10, currY - 10, currX + 10, currY + 10);
        }
    }

    // 원래 브러시 복원 후 생성한 브러시 모두 삭제
    SelectObject(hdc, hOldStoneBrush);
    DeleteObject(hBlackBrush);
    DeleteObject(hWhiteBrush);
}

void OmokGame::OnResize(int width, int height)
{
    std::cout << __FUNCTION__ << std::endl;

    learning::SetScreenSize(width, height);

    __super::OnResize(width, height);

    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    DeleteObject(hPrevBitmap);

    m_BoardOffsetX = (m_width - BOARD_PIXEL_SIZE) / 2;
    m_BoardOffsetY = (m_height - BOARD_PIXEL_SIZE) / 2;
}

void OmokGame::OnClose()
{
    std::cout << __FUNCTION__ << std::endl;

    SelectObject(m_hBackDC, m_hDefaultBitmap);

    DeleteObject(m_hBackBitmap);
    DeleteDC(m_hBackDC);

    ReleaseDC(m_hWnd, m_hFrontDC);
}

void OmokGame::OnMouseMove(int x, int y)
{
    m_MousePosPrev = m_MousePos;
    m_MousePos = {x, y};
}

void OmokGame::OnLButtonDown()
{
    const int boardLeft = m_BoardOffsetX;
    const int boardTop = m_BoardOffsetY;

    const int gridLeft = boardLeft + BOARD_PADDING;
    const int gridTop = boardTop + BOARD_PADDING;
    const int gridRight = gridLeft + BOARD_CELL_SIZE * (BOARD_LINE_COUNT - 1);
    const int gridBottom = gridTop + BOARD_CELL_SIZE * (BOARD_LINE_COUNT - 1);

    if (m_Winner != Turn::NONE)
        return;
    
    if (m_MousePos.x >= gridLeft && m_MousePos.x <= gridRight)
        if (m_MousePos.y >= gridTop && m_MousePos.y <= gridBottom)
            m_IsLClicked = true;
}

void OmokGame::OnRButtonDown()
{
    InitBoard();
}

void OmokGame::FixedUpdate()
{
}

void OmokGame::LogicUpdate()
{
}

void OmokGame::CheckGameOver()
{
    int blackCount = 0;
    int whiteCount = 0;

    for (int i = 0; i < BOARD_LINE_COUNT; i++)
    {
        for (int j = 0; j < BOARD_LINE_COUNT; j++)
        {
            // 가로 ==================================
            for (int k = 0; k < 5; k++)
            {
                if (Board[i][j + k] == static_cast<int>(Turn::BLACK))
                    blackCount++;
                else if (Board[i][j + k] == static_cast<int>(Turn::WHITE))
                    whiteCount++;
            }

            if (blackCount == 5)
            {
                m_Winner = Turn::BLACK;
                return;
            }

            if (whiteCount == 5)
            {
                m_Winner = Turn::WHITE;
                return;
            }

            blackCount = 0;
            whiteCount = 0;
            // 가로 ==================================

            // 아래 대각 ===================================
            for (int k = 0; k < 5; k++)
            {
                if (Board[i + k][j + k] == static_cast<int>(Turn::BLACK))
                    blackCount++;
                else if (Board[i + k][j + k] == static_cast<int>(Turn::WHITE))
                    whiteCount++;
            }

            if (blackCount == 5)
            {
                m_Winner = Turn::BLACK;
                return;
            }

            if (whiteCount == 5)
            {
                m_Winner = Turn::WHITE;
                return;
            }

            blackCount = 0;
            whiteCount = 0;
            // 아래 대각 ===================================

            // 아래 ========================================
            for (int k = 0; k < 5; k++)
            {
                if (Board[i + k][j] == static_cast<int>(Turn::BLACK))
                    blackCount++;
                else if (Board[i + k][j] == static_cast<int>(Turn::WHITE))
                    whiteCount++;
            }

            if (blackCount == 5)
            {
                m_Winner = Turn::BLACK;
                return;
            }

            if (whiteCount == 5)
            {
                m_Winner = Turn::WHITE;
                return;
            }

            blackCount = 0;
            whiteCount = 0;

            // 아래 ========================================

            // 윗 대각 =========================================
            for (int k = 0; k < 5; k++)
            {
                if (Board[i - k][j + k] == static_cast<int>(Turn::BLACK))
                    blackCount++;
                else if (Board[i - k][j + k] == static_cast<int>(Turn::WHITE))
                    whiteCount++;
            }

            if (blackCount == 5)
            {
                m_Winner = Turn::BLACK;
                return;
            }

            if (whiteCount == 5)
            {
                m_Winner = Turn::WHITE;
                return;
            }

            blackCount = 0;
            whiteCount = 0;
            // 윗 대각 =========================================
        }
    }
}

void OmokGame::TurnText(HDC hdc) const
{
    if (m_Winner == Turn::BLACK)
        TextOut(hdc, 10, 100, _T("Black wins!"), 11);
    else if (m_Winner == Turn::WHITE)
        TextOut(hdc, 10, 100, _T("White wins!"), 11);
    
    if (m_CurrTurn == Turn::BLACK)
        TextOut(hdc, 10, 10, _T("Black's turn"), 12);
    else
        TextOut(hdc, 10, 10, _T("White's turn"), 12);
}

void OmokGame::HelpText(HDC hdc) const
{
    TextOut(hdc, m_width / 2, 10, _T("Left click to place stone"), 25);
    TextOut(hdc, m_width / 2, 25, _T("Right click to restart"), 22);   
}

void OmokGame::InitBoard()
{
    m_CurrTurn = Turn::BLACK;
    m_Winner = Turn::NONE;
    m_IsLClicked = false;
    
    for (int i = 0; i < BOARD_LINE_COUNT; i++)
        for (int j = 0; j < BOARD_LINE_COUNT; j++)
            Board[i][j] = 0;
}
