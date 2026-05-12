#pragma once
#include "NzWndBase.h"
#include "Utillity.h"

// [CHECK] #7  ���� ������ ����Ͽ� ��������� �������� ����.
class GameTimer;
class GameObjectBase;
class GameObject;
namespace renderHelp
{
    class BitmapInfo;
}

class MyFirstWndGame : public NzWndBase
{
public:
    MyFirstWndGame() = default;
    ~MyFirstWndGame() override = default;

    bool Initialize();
    void Run();
    void Finalize();

private:
    void Update();
    void Render();

    void OnResize(int width, int height) override;
    void OnClose() override;

    void OnMouseMove(int x, int y);
    void OnLButtonDown(int x, int y);
    void OnRButtonDown(int x, int y);

    void FixedUpdate();
    //void CollisionCheck();
    void LogicUpdate();

    void CreatePlayer();
    //void CreateEnemy();

    void UpdatePlayerInfo();

    //void UpdateEnemyInfo();
    GameObject* GetPlayer() const
    {
	    return (GameObject*)m_GameObjectPtrTable[0];
    }

private:
    HDC m_hFrontDC = nullptr;
    HDC m_hBackDC = nullptr;
    HBITMAP m_hBackBitmap = nullptr;
    HBITMAP m_hDefaultBitmap = nullptr;

    // [CHECK] #8. ���� Ÿ�̸Ӹ� ����Ͽ� �������� �����ϴ� ����.
    GameTimer* m_pGameTimer = nullptr;
    float m_fDeltaTime = 0.0f;
    float m_fFrameCount = 0.0f;

    // [CHECK] #8. ���� ������Ʈ�� �����ϴ� �����̳�.
    GameObjectBase** m_GameObjectPtrTable = nullptr;

    struct MOUSE_POS
    {
        int x = 0;
        int y = 0;

        bool operator!=(const MOUSE_POS& other) const
        {
            return (x != other.x || y != other.y);
        }
    };

    MOUSE_POS m_MousePos = { 0, 0 };
    MOUSE_POS m_MousePosPrev = { 0, 0 };

    MOUSE_POS m_PlayerTargetPos = { 0, 0 };
    MOUSE_POS m_EnemySpawnPos = { 0, 0 };

#pragma region resource
    using BitmapInfo = renderHelp::BitmapInfo;
    BitmapInfo* m_pPlayerBitmapInfo = nullptr;
    BitmapInfo* m_pEnemyBitmapInfo = nullptr;
#pragma endregion
};