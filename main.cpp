
#include <windows.h> // windows.h contains our DOS friendly functions that we will use
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "SFML/Window.hpp"
#include "pieces.h"

#define HEIGHT 20
#define WIDTH 20
using namespace std;

// prototypes
void game(bool &gamestatus);
void printwindow(COORD mpos, CONSOLE_SCREEN_BUFFER_INFO mscreen, HANDLE mout_handle);
bool isbusy(int px, int py);
bool isallowed(int mpiece, int mrotation, int mposx, int mposy);
void deletepossiblelines();
bool isgameover();
void initboard();
void drawpiece(int mpiece, int mrotation, int mposx, int mposy, int k);
void delpiece(int mpiece, int mrotation, int mposx, int mposy); 
void deleteline(int py);
static int getxposition(int mpiece, int mrotation);
static int getyposition(int mpiece, int mrotation);
int getrand(int pa, int pb);
void genpiece();


// globals
int gamefield[HEIGHT][WIDTH];
int mpiece, mrotation, mposx, mposy;
int score;
int level;
DWORD info;
COORD pos;
CONSOLE_SCREEN_BUFFER_INFO screen_info;
HANDLE output_handle;
clock_t mtime1 = clock();
bool gamerunning;
bool restart;

// SFML stuff
sf::Window window(sf::VideoMode(10, 10), "SFML window"); 
sf::Event event;

int main()
{

	// init console	
	output_handle = GetStdHandle(STD_OUTPUT_HANDLE);  
	GetConsoleScreenBufferInfo(output_handle, &screen_info);

	// main loop
	while (true)
	{
		// START
		system("cls");
		pos.X = screen_info.srWindow.Right / 2 - 3; // Center X - (1/2 string length) 
		pos.Y = screen_info.srWindow.Bottom / 2;    // Center Y
		SetConsoleCursorPosition(output_handle, pos);
		cout << "TETRIS" << endl;
		pos.X = screen_info.srWindow.Right / 2 - 8;
		pos.Y = screen_info.srWindow.Bottom / 2 + 1;
		SetConsoleCursorPosition(output_handle, pos);
		cout << "ENTER LEVEL (0-9)" << endl;

		while (true)
		{
			window.pollEvent(event);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
			{
				level = 1;
				break;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
			{
				level = 2;
				break;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
			{
				level = 3;
				break;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
			{
				level = 4;
				break;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
			{
				level = 5;
				break;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
			{
				level = 6;
				break;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7))
			{
				level = 7;
				break;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8))
			{
				level = 8;
				break;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9))
			{
				level = 9;
				break;
			}
		}

		system("cls");

		// init var
		srand(time(NULL));
		score = 0;
		pos.X = screen_info.srWindow.Right / 2 - 11; // Center X - (1/2 string length) 
		pos.Y = screen_info.srWindow.Top;    // Center Y
		initboard();
		printwindow(pos, screen_info, output_handle);
		genpiece();
		drawpiece(mpiece, mrotation, mposx, mposy, 1);
		gamerunning = true;

		game(gamerunning);

		// game over
		system("cls");
		pos.X = screen_info.srWindow.Right / 2 - 20; // Center X - (1/2 string length) 
		pos.Y = screen_info.srWindow.Bottom / 2;    // Center Y
		SetConsoleCursorPosition(output_handle, pos);
		cout << "GAME OVER - press R to RESTART, ESC to QUIT";
		
		while (true)
		{
			window.pollEvent(event);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{	
				restart = true;
				break;
			}

			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				restart = false;
				break;
			}
		}

		if (restart == false)
			break;
	}
	return 0;
}


// update board 
void printwindow(COORD mpos, CONSOLE_SCREEN_BUFFER_INFO mscreen, HANDLE mout_handle)
{

	for (int i = 0; i < HEIGHT; i++)
	{
		mpos.X = mscreen.srWindow.Right / 2 - 11; // Center X - (1/2 string length) 
		mpos.Y = mscreen.srWindow.Top + 1 + i;    // Center Y
		SetConsoleCursorPosition(mout_handle, mpos);
		
		cout << "<|";

		for (int j = 0; j < WIDTH; j++)
		{
			if (gamefield[i][j] == 0)
				cout << ".";
			else if (gamefield[i][j] == 2)
				cout << "#";
			else if (gamefield[i][j] == 1)
				cout << "o";
		}

		cout << "|>";
	}


	mpos.X = mscreen.srWindow.Right / 2 - 11; // Center X - (1/2 string length) 
	mpos.Y = mscreen.srWindow.Top + 1 + HEIGHT;    // Center Y
	SetConsoleCursorPosition(mout_handle, mpos);

	cout << "<|";
	for (int j = 0; j < WIDTH; j++)
	{
		cout << "-";
	}
	cout << "|>" << endl;
	cout << endl;

	mpos.X = mscreen.srWindow.Right / 2 - 20; // Center X - (1/2 string length)
	mpos.Y = mscreen.srWindow.Top + 3 + HEIGHT;    // Center Y
	SetConsoleCursorPosition(mout_handle, mpos);
	cout << score << endl;
}

void game(bool &gamestatus)
{
	while (gamestatus)
	{
		deletepossiblelines();
		if (isgameover())
		{
			break;
		}
		window.pollEvent(event);
		switch (event.type)
		{
		case sf::Event::KeyPressed:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))

			{
				if (isallowed(mpiece, mrotation, mposx, mposy - 1))
				{
					delpiece(mpiece, mrotation, mposx, mposy);
					mposy--;
					drawpiece(mpiece, mrotation, mposx, mposy, 1);
					printwindow(pos, screen_info, output_handle);
				}

			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
			{
				if (isallowed(mpiece, mrotation, mposx, mposy + 1))
				{
					delpiece(mpiece, mrotation, mposx, mposy);
					mposy++;
					drawpiece(mpiece, mrotation, mposx, mposy, 1);
					printwindow(pos, screen_info, output_handle);
				}
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
			{
				if (isallowed(mpiece, (mrotation + 1) % 4, mposx, mposy))
				{
					delpiece(mpiece, mrotation, mposx, mposy);
					mrotation = (mrotation + 1) % 4;
					drawpiece(mpiece, mrotation, mposx, mposy, 1);
					printwindow(pos, screen_info, output_handle);
				}
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				if (isallowed(mpiece, mrotation, mposx + 1, mposy))
				{
					delpiece(mpiece, mrotation, mposx, mposy);
					mposx++;
					drawpiece(mpiece, mrotation, mposx, mposy, 1);
				}

				printwindow(pos, screen_info, output_handle);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				gamestatus = false;
			}
		}
		default:
			break;
		}

		clock_t mtime2 = clock();

		if ((mtime2 - mtime1) > 1000 - level * 100)
		{
			if (isallowed(mpiece, mrotation, mposx + 1, mposy))
			{
				delpiece(mpiece, mrotation, mposx, mposy);
				mposx++;
				drawpiece(mpiece, mrotation, mposx, mposy, 1);
			}
			else
			{
				drawpiece(mpiece, mrotation, mposx, mposy, 2);
				score = score + 21;
				genpiece();
				drawpiece(mpiece, mrotation, mposx, mposy, 1);
			}
			printwindow(pos, screen_info, output_handle);
			mtime1 = clock();
		}

	}
}

void initboard()
{
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			gamefield[i][j] = 0;
}

void deleteline(int py)
{
	for (int i = py; i > 0; i--)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			gamefield[i][j] = gamefield[i - 1][j];
		}
	}
}

void deletepossiblelines()
{
	for (int i = 0; i < HEIGHT; i++)
	{
		int j = 0;
		while (j < WIDTH)
		{
			if (gamefield[i][j] != 2) 
				break;
			else if (gamefield[i][j] == 2) 
				j++;
		}
		if (j == WIDTH)
			deleteline(i);
	}
}


bool isgameover()
{
	for (int j = 0; j < WIDTH; j++)
	{
		if (gamefield[0][j] == 2) return true;
	}
	return false;
}

void drawpiece(int mpiece, int mrotation, int mposx, int mposy, int k)
{
	for (int i = mposx, ii = 0; i < 5 + mposx; i++, ii++)
		for (int j = mposy,  jj = 0; j < 5 + mposy; j++, jj++)
			if (mPieces[mpiece][mrotation][ii][jj] == 1)
			{ 
			gamefield[i][j] = k*mPieces[mpiece][mrotation][ii][jj];
			}
}

void delpiece(int mpiece, int mrotation, int mposx, int mposy)
{
	for (int i = mposx, ii = 0; i < 5 + mposx; i++, ii++)
		for (int j = mposy, jj = 0; j < 5 + mposy; j++, jj++)
			if (mPieces[mpiece][mrotation][ii][jj] == 1)
			{
				gamefield[i][j]=0;
			}				
}

bool isbusy(int px, int py)
{
	if (gamefield[px][py] == 2) return true;
	else return false;
}


bool isallowed(int mpiece, int mrotation, int mposx, int mposy)
{
	for (int i = mposx, ii = 0; i < 5 + mposx; i++, ii++)
		for (int j = mposy, jj = 0; j < 5 + mposy; j++, jj++)
		{
			if (j < 0 || j > WIDTH-1 || i > HEIGHT-1 )
			{
				if (mPieces[mpiece][mrotation][ii][jj] == 1)
					return false;
			}

			if (i > 0)
			{
				if ((mPieces[mpiece][mrotation][ii][jj] == 1) && (isbusy(i,j)))
					return false;
			}
		}
	return true;
}

int getxposition(int mpiece, int mrotation)
{
	return pieceinitpos[mpiece][mrotation][0];
}

int getyposition(int mpiece, int mrotation)
{
	return pieceinitpos[mpiece][mrotation][1];
}

int getrand(int pa, int pb)
{
	return rand() % (pb - pa + 1) + pa;
}

void genpiece()
{
	mpiece = getrand(0, 6);
	mrotation = getrand(0, 3);
	mposx = getxposition(mpiece, mrotation);
	mposy = getyposition(mpiece, mrotation);

}

