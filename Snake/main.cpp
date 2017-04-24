#include <stdio.h>
#include "Snake.h"
#include <thread>
#include <conio.h>

void main()
{
	FixConsoleWindow();
	StartGame();
	
	char temp = 'D';
	std::thread t1(ThreadFunction);
	HANDLE handle_t1 = t1.native_handle();

	while (1)
	{
		//if (_kbhit())
		//{
			temp = _getch();
			temp = toupper(temp);
		//}

		if (snake.status == ALIVE)
		{
			if (temp == 'P')
				PauseGame(handle_t1);
			else if (temp == 27)
			{
				ExitGame(handle_t1);
				break;
			}
			else if (temp == 'L')
			{
				PauseGame(handle_t1);
				SaveGame();
				ExitGame(handle_t1);
				break;
			}
			else if (temp == 'T')
			{
				PauseGame(handle_t1);
				LoadGame();
				ResumeThread(handle_t1);
				PauseGame(handle_t1);
			}
			else
			{
				ResumeThread(handle_t1);

				if ((temp == 'A' || temp == 'D' || temp == 'S' || temp == 'W') && temp != CHAR_LOCK)
				{
					if (temp == 'A')
						CHAR_LOCK = 'D';
					else if (temp == 'D')
						CHAR_LOCK = 'A';
					else if (temp == 'S')
						CHAR_LOCK = 'W';
					else
						CHAR_LOCK = 'S';

					MOVING = temp;
				}
			}
		}
		else
		{
			if (temp == 'Y')
				StartGame();
			else
			{
				ExitGame(handle_t1);
				break;
			}
		}
	}

	t1.join();

	system("pause");
}


//void main()
//{
//	fstream inFile("test.txt");
//	Point p;
//
//	inFile >> p;
//
//	printf("%d %d", p.x, p.y);
//
//
//	system("pause");
//}