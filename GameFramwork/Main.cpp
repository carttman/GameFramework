#include "MyFirstWndGame.h"
#include <iostream>

#include "OmokGame.h"
#include "SceneManager.h"
#include "TestScene.h"


int main()
{

	// SceneManager 사용 예제 - 간단한 씬 등록 및 전환
	SceneManager mgr;

	mgr.RegisterScene(new TestScene("TestScene", "test01"));
	mgr.RegisterScene(new TestScene("TestScene", "test02"));

	mgr.ChangeScene("test01");

	mgr.Update(0.016f); // 60 FPS 가정, 1초에 60번 업데이트

	mgr.Render();

	mgr.ChangeScene("test02");

	mgr.Update(0.016f); // 60 FPS 가정, 1초에 60번 업데이트

	mgr.Render();


	MyFirstWndGame game;
	//OmokGame game;
	if (false == game.Initialize())
	{
		std::cout << "Failed to initialize the game. Error: " << GetLastError() << std::endl;
		return -1;
	}

	game.Run();

	game.Finalize();

	return 0;

}
