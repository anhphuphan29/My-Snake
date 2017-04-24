#include "Snake.h"


bool STAGE;
Snake snake;
Food food;
Gate gate;
Status GameStatus;
char MOVING;
char CHAR_LOCK;
int LEVEL;
bool PAUSE;

static const int BOARD_LEFT_EDGE = BOARD_WIDTH_OFFSET;			// Cạnh trái của khung Game (x)
static const int BOARD_RIGHT_EDGE = CONSOLE_MAX_WIDTH - BOARD_WIDTH_OFFSET - 1;			// Cạnh phải (x)
static const int BOARD_TOP_EDGE = BOARD_HEIGHT_OFFSET;			// Cạnh trên (y)
static const int BOARD_BOTTOM_EDGE = CONSOLE_MAX_HEIGHT - BOARD_HEIGHT_OFFSET - 1;		// Cạnh dưới (y)


void FixConsoleWindow(void)
{
	// Cố định cửa sổ console, ngăn người dùng thay đổi kích thước và làm mờ nút Maximize
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);

	// Ẩn con trỏ của màn hình Console
	CONSOLE_CURSOR_INFO info;
	HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(ConsoleHandle, &info);

	// Cố định kích thước buffer của cửa sổ Console với một Size xác định
	COORD ConsoleBufferSize = { CONSOLE_MAX_WIDTH, CONSOLE_MAX_HEIGHT };
	SetConsoleScreenBufferSize(ConsoleHandle, ConsoleBufferSize);

	// Cố dịnh kích thước cửa sổ Console với một Size xác định
	SMALL_RECT Rect = { 0,0,ConsoleBufferSize.X - 1,ConsoleBufferSize.Y - 1 };
	SetConsoleWindowInfo(ConsoleHandle, TRUE, &Rect);
}

void GotoXY(Point pos)
{
	COORD coord;
	coord.X = pos.x;
	coord.Y = pos.y;
	HANDLE ConsoleHanle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(ConsoleHanle, coord);
}

void DrawBoard(void)
{
	GotoXY({ BOARD_LEFT_EDGE, BOARD_TOP_EDGE });		// Góc trên bên trái
	printf("%c", 201);
	GotoXY({ BOARD_RIGHT_EDGE, BOARD_TOP_EDGE });		// Góc trên bên phải
	printf("%c", 187);
	GotoXY({ BOARD_LEFT_EDGE, BOARD_BOTTOM_EDGE });		// Góc dưới bên trái
	printf("%c", 200);
	GotoXY({ BOARD_RIGHT_EDGE, BOARD_BOTTOM_EDGE });	//Góc dưới bên phải
	printf("%c", 188);

	for (int i = BOARD_LEFT_EDGE + 1; i < BOARD_RIGHT_EDGE; i++)	// Cạnh trên
	{
		GotoXY({ i, BOARD_TOP_EDGE });
		printf("%c", 205);
	}

	for (int i = BOARD_LEFT_EDGE + 1; i < BOARD_RIGHT_EDGE; i++)	// Cạnh dưới
	{
		GotoXY({ i, BOARD_BOTTOM_EDGE });
		printf("%c", 205);
	}

	for (int i = BOARD_TOP_EDGE + 1; i < BOARD_BOTTOM_EDGE; i++)	// Cạnh trái
	{
		GotoXY({ BOARD_LEFT_EDGE, i });
		printf("%c", 186);
	}

	for (int i = BOARD_TOP_EDGE + 1; i < BOARD_BOTTOM_EDGE; i++)	// Cạnh phải
	{
		GotoXY({ BOARD_RIGHT_EDGE, i });
		printf("%c", 186);
	}
}

bool operator==(Point pos1, Point pos2)
{
	if (pos1.x == pos2.x && pos1.y == pos2.y)
		return true;

	return false;
}

ostream & operator<<(ostream & outDev, Point & pos)
{
	outDev << '(' << pos.x << ',' << pos.y << ')';

	return outDev;
}

istream & operator >> (istream & inDev, Point & pos)
{
	char temp;

	inDev >> (char)temp >> pos.x >> (char)temp >> pos.y >> (char)temp;

	return inDev;
}

bool isValidGate(Point pos)
{
	for (int i = 0; i < snake.size; i++)
	{
		for (int x = gate.left.x; x <= gate.right.x; x++)
		{
			for (int y = gate.top.y; y <= gate.bottom.y; y++)
				if (snake.part[i].pos.x == x && snake.part[i].pos.y == y)
					return false;
		}
	}

	return true;
}

void GenerateGate(void)
{
	Point pos;
	srand(time(NULL));
	if (GameStatus == ONGOING)
	{
		do
		{
			pos.x = rand() % (BOARD_RIGHT_EDGE - BOARD_LEFT_EDGE - 4) + (BOARD_LEFT_EDGE + 2);
			pos.y = rand() % (BOARD_BOTTOM_EDGE - BOARD_TOP_EDGE - 4) + (BOARD_TOP_EDGE + 2);
		} while (!isValidGate(pos));
	}
	else
	{
		pos.x = rand() % (BOARD_RIGHT_EDGE - BOARD_LEFT_EDGE - 4) + (BOARD_LEFT_EDGE + 2);
		pos.y = rand() % (BOARD_BOTTOM_EDGE - BOARD_TOP_EDGE - 4) + (BOARD_TOP_EDGE + 2);
	}

	gate.pos = pos;
	gate.left = { gate.pos.x - 1,gate.pos.y };
	gate.right = { gate.pos.x + 1,gate.pos.y };
	gate.top = { gate.pos.x,gate.pos.y - 1 };
	gate.bottom = { gate.pos.x,gate.pos.y + 1 };
	gate.status = OPEN;
	gate.count = 0;
}

void DrawGate(void)
{
	GotoXY({ gate.pos.x - 1,gate.pos.y - 1 });
	printf("%c%c%c", 218, 196, 191);

	GotoXY({ gate.pos.x - 1,gate.pos.y });
	printf("%c %c", 179, 179);

	GotoXY({ gate.pos.x - 1,gate.pos.y + 1 });
	printf("%c%c%c", 192, 196, 217);


	gate.count = 0;
	gate.status = OPEN;
}

void EraseGate(void)
{
	GotoXY({ gate.pos.x - 1,gate.pos.y - 1 });
	printf("   ");
	GotoXY({ gate.pos.x - 1,gate.pos.y });
	printf("   ");
	GotoXY({ gate.pos.x - 1,gate.pos.y + 1 });
	printf("   ");
}

void GenerateSnakeDirection(void)
{
	int height = BOARD_BOTTOM_EDGE - BOARD_TOP_EDGE + 1;
	int width = BOARD_RIGHT_EDGE - BOARD_LEFT_EDGE + 1;

	int left = gate.pos.x - BOARD_LEFT_EDGE + 1;
	int right = BOARD_RIGHT_EDGE - gate.pos.x + 1;
	int top = gate.pos.y - BOARD_TOP_EDGE + 1;
	int bottom = BOARD_BOTTOM_EDGE - gate.pos.y + 1;

	if (left > right)
	{
		if (bottom > top)
		{
			if (left / (double)width > bottom / (double)height)
				MOVING = 'A';
			else
				MOVING = 'S';
		}
		else
		{
			if (left / (double)width > top / (double)height)
				MOVING = 'A';
			else
				MOVING = 'W';
		}
	}
	else
	{
		if (bottom > top)
		{
			if (right / (double)width > bottom / (double)height)
				MOVING = 'D';
			else
				MOVING = 'S';
		}
		else
		{
			if (right / (double)width > top / (double)height)
				MOVING = 'D';
			else
				MOVING = 'W';
		}
	}

	if (MOVING == 'A')
		CHAR_LOCK = 'D';
	else if (MOVING == 'D')
		CHAR_LOCK = 'A';
	else if (MOVING == 'S')
		CHAR_LOCK = 'W';
	else
		CHAR_LOCK = 'S';
}

void DrawSnake(void)
{
	HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(ConsoleHandle, 10);
	if (GameStatus == START_LEVEL)
	{
		int i = 0;
		while (gate.pos.x != snake.part[i].pos.x || gate.pos.y != snake.part[i].pos.y)
			i++;

		for (int j = 0; j <= i; j++)
		{
			GotoXY(snake.part[j].pos);
			printf("%d", snake.part[j].data);
		}
	}
	else
	{
		for (int i = 0; i < snake.size; i++)
		{
			GotoXY(snake.part[i].pos);
			printf("%d", snake.part[i].data);
		}
	}
}

bool isValidFood(Point pos)
{
	if (GameStatus == START_LEVEL)
	{
		for (int x = gate.left.x; x <= gate.right.x; x++)
		{
			for (int y = gate.top.y; y <= gate.bottom.y; y++)
				if (x == pos.x && y == pos.y)
					return false;
		}
	}
	else
	{
		for (int i = 0; i < snake.size; i++)
			if (pos.x == snake.part[i].pos.x && pos.y == snake.part[i].pos.y)
				return false;
	}

	return true;
}

void GenerateFood(void)
{
	srand(time(NULL));

	do
	{
		food.pos.x = rand() % (BOARD_RIGHT_EDGE - BOARD_LEFT_EDGE - 2) + (BOARD_LEFT_EDGE + 1);
		food.pos.y = rand() % (BOARD_BOTTOM_EDGE - BOARD_TOP_EDGE - 2) + (BOARD_TOP_EDGE + 1);
	} while (!isValidFood(food.pos));

	food.data = MSSV[food.index];
}

void DrawFood(void)
{
	GotoXY(food.pos);
	printf("%d", food.data);
}

void ResetSnake(void)
{	
	snake.size = 7;
	snake.part = (Part*)realloc(snake.part, snake.size * sizeof(Part));

	for (int i = 0; i < snake.size; i++)
		snake.part[i].data = MSSV[i];

	snake.part[0].pos = gate.pos;
	snake.status = ALIVE;
	snake.oldTailPos = { 0,0 };
}

void ResetData(void)
{
	food.index = 0;

	GenerateGate();
	DrawGate();

	GenerateFood();
	DrawFood();

	snake.size = 7;
	ResetSnake();
}

void StartGame(void)
{
	srand(time(NULL));
	system("cls");
	GameStatus = START_LEVEL;
	LEVEL = 1;
	PAUSE = false;

	DrawBoard();

	GenerateGate();
	DrawGate();

	GenerateFood();
	DrawFood();

	snake.size = 7;
	ResetSnake();
	GenerateSnakeDirection();
}

void StartLevel(void)
{

}

void PauseGame(HANDLE t)
{
	if (!PAUSE)
	{
		SuspendThread(t);
		PAUSE = true;
	}
	else
	{
		ResumeThread(t);
		PAUSE = false;
	}
}

void ExitGame(HANDLE t)
{
	if (snake.part != NULL)
		free(snake.part);

	system("cls");
	TerminateThread(t, 0);
}

void SaveGame(void)
{
	GotoXY({ 0,0 });
	char filename[30];

	while(true)
	{
		printf("Input file name to save: ");
		gets_s(filename);

		if (isExist(filename))
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 100; j++)
				{
					GotoXY({ j,i });
					printf(" ");
				}
			}
			GotoXY({ 0,0 });

			printf("File is already exist!\n");
		}
		else
			break;
	}


	ofstream outFile(filename);

	outFile << (char)MOVING << endl;
	outFile << (char)CHAR_LOCK << endl;

	outFile << snake.size << endl;
	for (int i = 0; i < snake.size; i++)
		outFile << snake.part[i].pos << " " << snake.part[i].data << endl;
	outFile << snake.status << endl;
	outFile << snake.oldTailPos << endl;

	outFile << food.pos << endl;
	outFile << food.index << endl;
	outFile << food.data << endl;
	outFile << food.status << endl;

	outFile << gate.pos << " " << gate.left << " " << gate.right << " " << gate.top << " " << gate.bottom << endl;
	outFile << gate.status << endl;
	outFile << gate.count << endl;

	outFile << GameStatus << endl;
	outFile << LEVEL << endl;
	outFile << PAUSE << endl;

	outFile.close();

	GotoXY({ 0,0 });
	if (isExist(filename))
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 100; j++)
			{
				GotoXY({ j,i });
				printf(" ");
			}
		}
	}
	GotoXY({ 0,0 });
	printf("Game saved\n");
	system("pause");
}

void LoadGame(void)
{
	
	GotoXY({ 0,0 });
	char filename[30];
	fstream inFile;
	int temp;

	while (true)
	{
		printf("Input file name to load: ");
		gets_s(filename);
		inFile.open(filename);

		if (!inFile)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 100; j++)
				{
					GotoXY({ j,i });
					printf(" ");
				}
			}
			GotoXY({ 0,0 });

			printf("File is already exist!\n");
		}
		else
			break;
	}

	inFile >> (char)MOVING;
	inFile >> (char)CHAR_LOCK;

	inFile >> snake.size;
	snake.part = (Part*)realloc(snake.part, snake.size * sizeof(Part));
	for (int i = 0; i < snake.size; i++)
		inFile >> snake.part[i].pos >> snake.part[i].data;
	inFile >> temp;
	snake.status = (Status)temp;
	inFile >> snake.oldTailPos;

	inFile >> food.pos;
	inFile >> food.index;
	inFile >> food.data;
	inFile >> temp;
	food.status = (Status)temp;

	inFile >> gate.pos >> gate.left >> gate.right >> gate.top >> gate.bottom;
	inFile >> temp;
	gate.status = (Status)temp;
	inFile >> gate.count;

	inFile >> temp;
	GameStatus = (Status)temp;
	inFile >> LEVEL;
	PAUSE = false;


	system("cls");
	DrawBoard();
	if (gate.status == OPEN)
		DrawGate();
	if (food.status != OUT_OF_FOOD)
	{
		DrawFood();
		food.status = EXIST;
	}
	DrawSnake();




	inFile.close();
}

bool isExist(const char * filename)
{
	fstream inFile(filename);

	if (inFile)
	{
		inFile.close();
		return true;
	}
	
	return false;
}

void MoveRight(void)
{
	int size = snake.size;

	snake.oldTailPos = snake.part[size - 1].pos;

	for (int i = snake.size - 1; i > 0; i--)
		snake.part[i].pos = snake.part[i - 1].pos;

	snake.part[0].pos.x++;

	if (snake.part[0].pos.x == BOARD_RIGHT_EDGE)
		ProcessDead();
	else if (snake.part[0].pos == food.pos && food.status != OUT_OF_FOOD)
		ProcessEat();
	else if (snake.part[0].pos == gate.pos && gate.status == OPEN && food.status == OUT_OF_FOOD)
		ProcessGate();
	else
	{
		for (int i = 3; i < snake.size; i++)
			if (snake.part[0].pos == snake.part[i].pos)
				ProcessDead();
	}

	if (GameStatus == START_LEVEL && snake.part[snake.size - 1].pos == gate.right)
		GameStatus = ONGOING;
}

void MoveLeft(void)
{
	snake.oldTailPos = snake.part[snake.size - 1].pos;

	for (int i = snake.size - 1; i > 0; i--)
		snake.part[i].pos = snake.part[i - 1].pos;

	snake.part[0].pos.x--;

	if (snake.part[0].pos.x == BOARD_LEFT_EDGE)
		ProcessDead();
	else if (snake.part[0].pos == food.pos && food.status != OUT_OF_FOOD)
		ProcessEat();
	else if (snake.part[0].pos == gate.pos && gate.status == OPEN && food.status == OUT_OF_FOOD)
		ProcessGate();
	else
	{
		for (int i = 3; i < snake.size; i++)
			if (snake.part[0].pos == snake.part[i].pos)
				ProcessDead();
	}

	if (GameStatus == START_LEVEL && snake.part[snake.size - 1].pos == gate.left)
		GameStatus = ONGOING;
}

void MoveUp(void)
{
	snake.oldTailPos = snake.part[snake.size - 1].pos;

	for (int i = snake.size - 1; i > 0; i--)
		snake.part[i].pos = snake.part[i - 1].pos;

	snake.part[0].pos.y--;

	if (snake.part[0].pos.y == BOARD_TOP_EDGE)
		ProcessDead();
	else if (snake.part[0].pos == food.pos && food.status != OUT_OF_FOOD)
		ProcessEat();
	else if (snake.part[0].pos == gate.pos && gate.status == OPEN && food.status == OUT_OF_FOOD)
		ProcessGate();
	else
	{
		for (int i = 3; i < snake.size; i++)
			if (snake.part[0].pos == snake.part[i].pos)
				ProcessDead();
	}
	if (GameStatus == START_LEVEL && snake.part[snake.size - 1].pos == gate.top)
		GameStatus = ONGOING;
}

void MoveDown(void)
{
	snake.oldTailPos = snake.part[snake.size - 1].pos;

	for (int i = snake.size - 1; i > 0; i--)
		snake.part[i].pos = snake.part[i - 1].pos;

	snake.part[0].pos.y++;

	if (snake.part[0].pos.y == BOARD_BOTTOM_EDGE)
		ProcessDead();
	else if (snake.part[0].pos == food.pos && food.status != OUT_OF_FOOD)
		ProcessEat();
	else if (snake.part[0].pos == gate.pos && gate.status == OPEN && food.status == OUT_OF_FOOD)
		ProcessGate();
	else
	{
		for (int i = 3; i < snake.size; i++)
			if (snake.part[0].pos == snake.part[i].pos)
				ProcessDead();
	}

	if (GameStatus == START_LEVEL && snake.part[snake.size - 1].pos == gate.bottom)
		GameStatus = ONGOING;
}

bool isOutOfGate(void)
{
	Point pos = snake.part[snake.size - 1].pos;

	if (pos.x - gate.pos.x == 2 || pos.x - gate.pos.x == -2 || pos.y - gate.pos.y == 2 || pos.y - gate.pos.y == -2)
		return true;

	return false;
}

bool isCollide(void)
{
	return false;
}

void ProcessDead(void)
{
	snake.status = DEAD;
	GotoXY({ 0, CONSOLE_MAX_HEIGHT - 2 });
	printf("Dead, type y to contine or any key to exit: ");
}

void ProcessEat(void)
{
	snake.size++;
	snake.part = (Part*)realloc(snake.part, snake.size * sizeof(Part));
	snake.part[snake.size - 1].data = food.data;
	snake.part[snake.size - 1].pos = snake.oldTailPos;
	snake.oldTailPos = { 0,0 };

	food.index++;
	GenerateFood();
	food.status = NOT_EXIST;


	if (food.index == MAX_SIZE_FOOD)
	{
		food.status = OUT_OF_FOOD;
		GenerateGate();
		DrawGate();
	}
}

void ProcessGate(void)
{
	for (int i = 0; i < snake.size; i++)
	{
		GotoXY(snake.oldTailPos);
		printf(" ");

		for (int j = i; j < snake.size; j++)
		{
			GotoXY(snake.part[j].pos);
			printf("%d", snake.part[j].data);
		}

		if (i == snake.size - 1)
			DrawGate();

		snake.oldTailPos = snake.part[snake.size - 1].pos;

		for (int j = snake.size - 1; j > i; j--)
		{
			snake.part[j].pos = snake.part[j - 1].pos;
		}

		Sleep(SPEED_CONSTANT / LEVEL);
	}
	
	Sleep(1000);
	EraseGate();
	Sleep(2000);


	GameStatus = START_LEVEL;
	LEVEL++;
	food.index = 0;
	
	GenerateGate();
	GenerateFood();
	food.status = NONE;

	DrawGate();
	DrawFood();
	snake.part[0].pos = gate.pos;
	snake.status = ALIVE;
	snake.oldTailPos = { 0,0 };
	if (LEVEL > MAX_LEVEL)
	{
		ResetSnake();
		LEVEL = 1;
	}
	GenerateSnakeDirection();
}

void ThreadFunction(void)
{
	while (1)
	{
		if (snake.status == ALIVE)
		{
			if (GameStatus != START_LEVEL && snake.oldTailPos.x != 0 && snake.oldTailPos.y != 0)
			{
				GotoXY(snake.oldTailPos);
				printf(" ");
			}

			DrawSnake();

			if (food.status == NOT_EXIST)
			{
				DrawFood();
				food.status == EXIST;
			}
			
			if (gate.status == OPEN && food.status != OUT_OF_FOOD)
			{
				if (gate.count == 20 + snake.size)
				{
					EraseGate();
					gate.status = CLOSE;
				}
				else
					gate.count++;
			}

			switch (MOVING)
			{
			case 'A':
				MoveLeft();
				break;
			case 'D':
				MoveRight();
				break;
			case 'W':
				MoveUp();
				break;
			case 'S':
				MoveDown();
			}

			Sleep(SPEED_CONSTANT / LEVEL);
		}

		//if (GameStatus == START_LEVEL && snake.part[snake.size - 1].pos == gate.pos)
		//	GameStatus = ONGOING;
	}
}
