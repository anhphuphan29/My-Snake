#ifndef _SNAKE_H
#define _SNAKE_H

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <fstream>
#include <sys/stat.h>

using namespace std;

#define CONSOLE_MAX_HEIGHT 40
#define CONSOLE_MAX_WIDTH 100

#define BOARD_WIDTH_OFFSET 5
#define BOARD_HEIGHT_OFFSET 3

#define MAX_SIZE_FOOD 7
#define SPEED_CONSTANT 50
#define MAX_LEVEL 3

const int MSSV[] = { 1,6,1,2,5,1,1 };


enum Status
{
	NONE,

	DEAD,
	ALIVE,

	OPEN,
	CLOSE,
	ACTIVE,

	OUT_OF_FOOD,
	EXIST,
	NOT_EXIST,

	START_LEVEL,
	ONGOING
};



struct Point
{
	int x, y;
};

struct Part
{
	Point pos;
	int data;
};

struct Snake
{
	Part *part;
	int size;
	Status status;
	Point oldTailPos;
};

struct Food
{
	Point pos;
	int index, data;
	Status status;
};

struct Gate
{
	Point pos, left, right, top, bottom;
	Status status;
	int count;
};

extern bool STAGE;
extern Snake snake;
extern Food food;
extern Gate gate;
extern Status GameStatus;
extern char MOVING;
extern char CHAR_LOCK;
extern int LEVEL;
extern bool PAUSE;

void FixConsoleWindow(void);
void GotoXY(Point pos);
void DrawBoard(void);

bool operator == (Point pos1, Point pos2);
ostream& operator << (ostream& outDev, Point& pos);
istream& operator >> (istream& inDev, Point& pos);

bool isValidGate(Point pos);
void GenerateGate(void);
void DrawGate(void);
void EraseGate(void);

void GenerateSnakeDirection(void);
void DrawSnake(void);

bool isValidFood(Point pos);
void GenerateFood(void);
void DrawFood(void);
void ResetSnake(void);
void ResetData(void);
void StartGame(void);

void StartLevel(void);
void PauseGame(HANDLE t);
void ExitGame(HANDLE t);
void SaveGame(void);
void LoadGame(void);
bool isExist(const char *filename);

void MoveRight(void);
void MoveLeft(void);
void MoveUp(void);
void MoveDown(void);

bool isOutOfGate(void);
bool isCollide(void);
void ProcessDead(void);
void ProcessEat(void);
void ProcessGate(void);

void ThreadFunction(void);

#endif // !_SNAKE_H
