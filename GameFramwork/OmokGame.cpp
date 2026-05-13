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

	//�޸� DC�� �׷��� ����� ���� DC(m_hFrontDC)�� ����
	BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);

}

void OmokGame::OnResize(int width, int height)
{
	std::cout << __FUNCTION__ << std::endl;

	learning::SetScreenSize(width, height);

	__super::OnResize(width, height);

	m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

	HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

	DeleteObject(hPrevBitmap);
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
