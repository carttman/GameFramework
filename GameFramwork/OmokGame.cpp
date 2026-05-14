#include "OmokGame.h"

#include <iostream>
#include <ostream>

#include "GameTimer.h"
#include "Utillity.h"

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
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				OmokGame::OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			else if (msg.message == WM_RBUTTONDOWN)
			{
				OmokGame::OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			else if (msg.message == WM_MOUSEMOVE)
			{
				OmokGame::OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
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

	//�޸� DC�� �׷��� ����� ���� DC(m_hFrontDC)�� ����
	BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);

}

void OmokGame::DrawBoard(HDC hdc)
{
	const int boardLeft = m_BoardOffsetX;
	const int boardTop = m_BoardOffsetY;
	const int boardRight = boardLeft + BOARD_PIXEL_SIZE;
	const int boardBottom = boardTop + BOARD_PIXEL_SIZE;

	// 판 배경(나무색)
	HBRUSH hBoardBrush = CreateSolidBrush(RGB(220, 179, 92));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBoardBrush);
	HPEN hNoPen = (HPEN)GetStockObject(NULL_PEN);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hNoPen);
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

					if (m_MousePos.x >= gridLeft && m_MousePos.x <= gridRight)
						if (m_MousePos.y >= gridTop && m_MousePos.y <= gridBottom)
						{
							if (isClicked)
							{
								isClicked = false;
								// 인덱스 판단하기 
								Board[i][j] = 1;
							}
						}



				}

		}
	}

	for (int i = 0; i < BOARD_LINE_COUNT; i++)
	{
		int currY = gridTop + i * BOARD_CELL_SIZE;

		for (int j = 0; j < BOARD_LINE_COUNT; j++)
		{
			int currX = gridLeft + j * BOARD_CELL_SIZE;

			if (!Board[i][j])
				continue;

			Ellipse(hdc, currX - 10, currY - 10, currX + 10, currY + 10);
		}
	}

	//Rectangle(hdc, m_MousePos.x - 10, m_MousePos.y - 10, m_MousePos.x + 10, m_MousePos.y + 10);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBoardBrush);
	DeleteObject(hLinePen);
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
	m_MousePos = { x, y };
}

void OmokGame::OnLButtonDown(int x, int y)
{
	const int boardLeft = m_BoardOffsetX;
	const int boardTop = m_BoardOffsetY;
	
	const int gridLeft = boardLeft + BOARD_PADDING;
	const int gridTop = boardTop + BOARD_PADDING;
	const int gridRight = gridLeft + BOARD_CELL_SIZE * (BOARD_LINE_COUNT - 1);
	const int gridBottom = gridTop + BOARD_CELL_SIZE * (BOARD_LINE_COUNT - 1);

	if (m_MousePos.x >= gridLeft && m_MousePos.x <= gridRight )
		if (m_MousePos.y >= gridTop && m_MousePos.y <= gridBottom )
			isClicked = true;
}

void OmokGame::OnRButtonDown(int x, int y)
{

}

void OmokGame::FixedUpdate()
{
}

void OmokGame::LogicUpdate()
{

}

