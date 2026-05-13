#include "MyFirstWndGame.h"
#include "GameTimer.h"
#include "Collider.h"
#include "GameObject.h"
#include <iostream>
#include <assert.h>

#include "RenderHelp.h"

using namespace learning;

constexpr int MAX_GAME_OBJECT_COUNT = 1000;

bool MyFirstWndGame::Initialize()
{
	m_pGameTimer = new GameTimer();
	m_pGameTimer->Reset();

	const wchar_t* className = L"MyFirstWndGame";
	const wchar_t* windowName = L"MyFirstWndGame";

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

	m_GameObjectPtrTable = new GameObjectBase * [MAX_GAME_OBJECT_COUNT];

	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		m_GameObjectPtrTable[i] = nullptr;
	}

#pragma region resource
	// 파일의 절대 경로와 상대 경로 구분
	// IDE에서 인지하는 현재 경로와 실제 실행 파일을 바로 실행했을 때 경로 기준이 다름
	// ./ 는 상대 경로
	m_pPlayerBitmapInfo = renderHelp::CreateBitmapInfo(L"../Resource/redbird.png");
	m_pEnemyBitmapInfo = renderHelp::CreateBitmapInfo(L"../Resource/graybird.png");
	if (m_pPlayerBitmapInfo == nullptr)
	{
		std::cout << "Bitmap Load Failed!" << std::endl;
		return false;
	}
	if (m_pEnemyBitmapInfo == nullptr)
	{
		std::cout << "Bitmap Load Failed!" << std::endl;
		 return false;
	}
	#pragma endregion
	
	CreatePlayer();
	return true;

}

void MyFirstWndGame::Run()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				MyFirstWndGame::OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			else if (msg.message == WM_RBUTTONDOWN)
			{
				MyFirstWndGame::OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			else if (msg.message == WM_MOUSEMOVE)
			{
				MyFirstWndGame::OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
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

void MyFirstWndGame::Finalize()
{
	delete m_pGameTimer;
	m_pGameTimer = nullptr;

	if (m_GameObjectPtrTable)
	{
		for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
		{
			if (m_GameObjectPtrTable[i])
			{
				delete m_GameObjectPtrTable[i];
				m_GameObjectPtrTable[i] = nullptr;
			}
		}
		delete m_GameObjectPtrTable;
	}

	__super::Destroy();
}

void MyFirstWndGame::FixedUpdate()
{
	if (m_EnemySpawnPos.x != 0 && m_EnemySpawnPos.y != 0)
	{
		CreateEnemy();
	}
}

 void MyFirstWndGame::CollisionCheck()
 {
 	GameObject* player = GetPlayer();
 	ColliderCircle* playerCircle = player->GetColiderCircle();

 	bool isPlayerColide = false;
 	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
 	{
 		if (m_GameObjectPtrTable[i])
 		{
 			m_GameObjectPtrTable[i]->Update(m_fDeltaTime);

 			if (i > 0)
 			{
 				GameObject* enemy = dynamic_cast<GameObject*>(m_GameObjectPtrTable[i]);
 				ColliderCircle* enemyCircle = enemy->GetColiderCircle();

 				Vector2f enemyPos = enemyCircle->center;
 				Vector2f playerPos = playerCircle->center;

 				float dis = playerPos.Distance(enemyPos) - static_cast<float>(playerCircle->radius) - static_cast<float>(enemyCircle->radius);

 				if (dis < 0)
 				{
 					enemy->isColide = true;
 					isPlayerColide = true;
 				}
 				else
 				{
 					enemy->isColide = false;
 				}
 			}
 		}
 	}

 	if (isPlayerColide)
 		player->isColide = true;
 	else
 		player->isColide = false;
 }

void MyFirstWndGame::LogicUpdate()
{
	UpdatePlayerInfo();
	
	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		if (m_GameObjectPtrTable[i])
		{
			m_GameObjectPtrTable[i]->Update(m_fDeltaTime);
		}
	}
	UpdateEnemyInfo();
	CollisionCheck();
}

void MyFirstWndGame::CreatePlayer()
{
	assert(m_GameObjectPtrTable[0] == nullptr && "Player object already exists!");

	GameObject* pNewObject = new GameObject(ObjectType::PLAYER);

	pNewObject->SetName("Player");
	pNewObject->SetPosition(0.0f, 0.0f);  
	pNewObject->SetSpeed(1.0f);  

	//====================bitmap
	pNewObject->SetWidth(100); // 일단, 임의로 설정
	pNewObject->SetHeight(100); // 일단, 임의로 설정
	pNewObject->SetBitmapInfo(m_pPlayerBitmapInfo);
	//====================
	pNewObject->SetColliderCircle(50.0f); 
	m_GameObjectPtrTable[0] = pNewObject;
}

 void MyFirstWndGame::CreateEnemy() 
 {
 	float x = m_EnemySpawnPos.x;
 	float y = m_EnemySpawnPos.y;
 	float radius = 50.0f;
 	m_EnemySpawnPos = { 0, 0 };

 	GameObject* pNewObject = new GameObject(ObjectType::ENEMY);
 	pNewObject->SetName("Enemy");
 	pNewObject->SetPosition(x, y);
 	pNewObject->SetSpeed(0.5f);   
 	pNewObject->SetColliderCircle(radius); 
 	
 	pNewObject->SetWidth(100);
 	pNewObject->SetHeight(100);
 	pNewObject->SetBitmapInfo(m_pEnemyBitmapInfo);
 	
 	bool flag = false;
 	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; i++)
 	{
 		if (m_GameObjectPtrTable[i] != nullptr)
 		{
 			GameObject* other = dynamic_cast<GameObject*>(m_GameObjectPtrTable[i]);

 			auto otherCircle = other->GetColiderCircle();

 			Vector2f otherPos = otherCircle->center;
 			Vector2f myPos = { x, y };

 			float dis = myPos.Distance(otherPos) - static_cast<float>(otherCircle->radius) - radius;

 			if (dis < 0)
 			{
 				flag = true;
 				break;
 			}
 		}
 	}

 	int i = 0;
 	while (++i < MAX_GAME_OBJECT_COUNT) 
 	{
 		if (nullptr == m_GameObjectPtrTable[i])
 		{
 			m_GameObjectPtrTable[i] = pNewObject;
 			break;
 		}
 	}

 	if (i == MAX_GAME_OBJECT_COUNT || flag) 
 	{
 		
 		delete pNewObject;
 		pNewObject = nullptr;
 		m_GameObjectPtrTable[i] = nullptr;
 	}
 }

void MyFirstWndGame::UpdatePlayerInfo()
{
	static GameObject* pPlayer = GetPlayer();

	assert(pPlayer != nullptr);

	Vector2f mousePos(m_PlayerTargetPos.x, m_PlayerTargetPos.y);
	Vector2f playerPos = pPlayer->GetPosition();

	Vector2f playerDir = mousePos - playerPos;
	float distance = playerDir.Length(); 

	if (distance > 50.f) 
	{
		playerDir.Normalize();
		pPlayer->SetDirection(playerDir); 
		pPlayer->SetSpeed(1.0f);
	}
	else
	{
		pPlayer->SetDirection(Vector2f(0, 0)); 
		pPlayer->SetSpeed(0);
	}
}

 void MyFirstWndGame::UpdateEnemyInfo()
 {
 	const auto player = GetPlayer();
 	ColliderCircle* playerCircle = player->GetColiderCircle();
 	if (player == nullptr)
 		return;
 	
 	// 플레이어가 움직였다면 enemy 전부 따라가게 한다.
 	// enemy는 플레이어 원을 넘지 않도록 한다.
 	for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
 	{
 		if (m_GameObjectPtrTable[i] == nullptr)
 			continue;
 		
 		// Player와 Enemy의 충돌 원의 범위를 계산하여 겹치지 않도록 한다
 		GameObject* enemy = dynamic_cast<GameObject*>(m_GameObjectPtrTable[i]);
 		ColliderCircle* enemyCircle = enemy->GetColiderCircle();
 		
 		Vector2f playerPos = playerCircle->center;
 		Vector2f enemyPos = enemyCircle->center;

 		float dist = playerPos.Distance(enemyPos) - static_cast<float>(playerCircle->radius) - static_cast<float>(enemyCircle->radius);

 		Vector2f dir = playerPos - enemyPos;
 		dir.Normalize();
 		
 		if (dist <= 0)
 		{
 			enemy->SetDirection(-dir);
 			enemy->SetSpeed(1.5f);
 		}
 		else
 		{
 			enemy->SetDirection(dir);
 			enemy->SetSpeed(0.5f);
 		}
 		
 		if (player->GetSpeed() <= 0)
 		{
 			enemy->SetDirection(0);
 			enemy->SetSpeed(0);
 		}
 	}
 	
 	//enemy들끼리 겹치지않도록 한다.
 	for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
 	{
 		if (m_GameObjectPtrTable[i] == nullptr)
 			continue;
 		
 		// Enemy들의 충돌 원의 범위를 계산하여 겹치지 않도록 한다
 		GameObject* enemy = dynamic_cast<GameObject*>(m_GameObjectPtrTable[i]);
 		ColliderCircle* enemyCircle = enemy->GetColiderCircle();

 		for (int j = i; j < MAX_GAME_OBJECT_COUNT; ++j)
 		{
 			if (m_GameObjectPtrTable[j] == nullptr || enemy == m_GameObjectPtrTable[j])
 				continue;
 			
 			GameObject* otherEnemy = dynamic_cast<GameObject*>(m_GameObjectPtrTable[j]);
 			ColliderCircle* otherEnemyCircle = otherEnemy->GetColiderCircle();
 			
 			Vector2f enemyPos = enemyCircle->center;
 			Vector2f otherEnemyPos = otherEnemyCircle->center;
 	
 			float dist = otherEnemyPos.Distance(enemyPos) - static_cast<float>(otherEnemyCircle->radius) - static_cast<float>(enemyCircle->radius);
 	
 			Vector2f dir = otherEnemyPos - enemyPos;
 			dir.Normalize();
 			
 			if (dist <= 0)
 			{
 				enemy->SetDirection(-dir);
 				enemy->SetSpeed(1.5f);
 			}
 		}
 	}
 }

void MyFirstWndGame::Update()
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

void MyFirstWndGame::Render()
{
	//Clear the back buffer
	::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);

	for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		if (m_GameObjectPtrTable[i])
		{
			m_GameObjectPtrTable[i]->Render(m_hBackDC);
		}
	}

	//�޸� DC�� �׷��� ����� ���� DC(m_hFrontDC)�� ����
	BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void MyFirstWndGame::OnResize(int width, int height)
{
	std::cout << __FUNCTION__ << std::endl;

	learning::SetScreenSize(width, height);

	__super::OnResize(width, height);

	m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

	HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

	DeleteObject(hPrevBitmap);
}

void MyFirstWndGame::OnClose()
{
	std::cout << __FUNCTION__ << std::endl;

	SelectObject(m_hBackDC, m_hDefaultBitmap);

	DeleteObject(m_hBackBitmap);
	DeleteDC(m_hBackDC);

	ReleaseDC(m_hWnd, m_hFrontDC);
}

void MyFirstWndGame::OnMouseMove(int x, int y)
{
	/*   std::cout << __FUNCTION__ << std::endl;
	   std::cout << "x: " << x << ", y: " << y << std::endl;*/
	m_MousePosPrev = m_MousePos;
	m_MousePos = { x, y };
}

void MyFirstWndGame::OnLButtonDown(int x, int y)
{
	/*  std::cout << __FUNCTION__ << std::endl;
 std::cout << "x: " << x << ", y: " << y << std::endl;*/

	m_PlayerTargetPos.x = x;
	m_PlayerTargetPos.y = y;

}

void MyFirstWndGame::OnRButtonDown(int x, int y)
{
	/*  std::cout << __FUNCTION__ << std::endl;
   std::cout << "x: " << x << ", y: " << y << std::endl;*/

	m_EnemySpawnPos.x = x;
	m_EnemySpawnPos.y = y;
}