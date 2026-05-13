#include "MyFirstWndGame.h"
#include <iostream>

#include "OmokGame.h"


int main()
{
	//MyFirstWndGame game;
	OmokGame game;
	if (false == game.Initialize())
	{
		std::cout << "Failed to initialize the game. Error: " << GetLastError() << std::endl;
		return -1;
	}

	game.Run();

	game.Finalize();

	return 0;

}
