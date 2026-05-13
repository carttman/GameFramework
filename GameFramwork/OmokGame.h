#pragma once
#include "NzWndBase.h"

class GameTimer;

class OmokGame : public NzWndBase
{
public:
	OmokGame() = default;
	~OmokGame() override = default;

    bool Initialize();
    void Run();
    void Finalize();

private:
    void Update();
    void Render();

    void DrawBoard(HDC hdc);

    void OnResize(int width, int height) override;
    void OnClose() override;

    void OnMouseMove(int x, int y);
    void OnLButtonDown(int x, int y);
    void OnRButtonDown(int x, int y);

    void FixedUpdate();
    void LogicUpdate();

private:
    HDC m_hFrontDC = nullptr;
    HDC m_hBackDC = nullptr;
    HBITMAP m_hBackBitmap = nullptr;
    HBITMAP m_hDefaultBitmap = nullptr;

    GameTimer* m_pGameTimer = nullptr;
    float m_fDeltaTime = 0.0f;
    float m_fFrameCount = 0.0f;

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

    static constexpr int BOARD_LINE_COUNT = 15;        // 15x15 (표준 오목 판)
    static constexpr int BOARD_CELL_SIZE = 40;         // 한 칸의 픽셀 크기
    static constexpr int BOARD_PADDING = 30;           // 격자 바깥 여백
    static constexpr int BOARD_PIXEL_SIZE = BOARD_CELL_SIZE * (BOARD_LINE_COUNT - 1) + BOARD_PADDING * 2;

    int m_BoardOffsetX = 0;
    int m_BoardOffsetY = 0;
};

