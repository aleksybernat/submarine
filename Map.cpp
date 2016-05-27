#include <cstdio>

#include "Map.h"

#define TXT_BOLD printf("\x1b[1m");
#define TXT_CYAN printf("\x1b[36m");
#define TXT_BLACK printf("\x1b[30m");
#define TXT_WHITE printf("\x1b[34m");


#define C_RESET printf("\x1b[0m");

#define BG_BLUE printf("\x1b[44m");
#define BG_CYAN printf("\x1b[46m");
#define BG_RED printf("\x1b[41m");
#define BG_WHITE printf("\x1b[47m");
#define BG_YELLOW printf("\x1b[43m");

Map::Map(int size)
{
	int i, j;
	_size = size;

	for(i=0; i<_size; i++)
	{
		for(j=0; j<_size; j++)
		{
			_map[j][i]=0;
		}
	}
}

void Map::showMap()
{
	int i, j;
	BG_CYAN;
	TXT_BOLD;

	TXT_WHITE;

	printf("\nY\\X");


	for (int i=0; i<_size; i++)
	{
		printf("%2d|", i);
	}


	printf("X\\Y");
	C_RESET;
	printf("\n");

	for(i=0; i<_size; i++)
	{

		TXT_BOLD;
		TXT_WHITE;
		BG_CYAN;
		printf("%2d:", i);

		TXT_CYAN;
		BG_BLUE;


		for(j=0; j<_size; j++)
		{
			switch (_map[j][i])
			{
				case 0: {printf(" ~ "); break;}
				case 1: {TXT_BLACK; BG_WHITE; printf("<o>"); TXT_CYAN; BG_BLUE; break;}
				case 2: {TXT_BLACK; BG_RED; printf("<x>"); TXT_CYAN; BG_BLUE; break;}
				case 3: {printf(" ~ "); break;}
				case 4: {TXT_BLACK; BG_YELLOW; printf("<d>"); TXT_CYAN; BG_BLUE; break;}
			}
		}

		C_RESET;
		TXT_BOLD;
		TXT_WHITE;
		BG_CYAN;
		printf(":%-2d", i);
		C_RESET;
		printf("\n");


	}
	

	TXT_BOLD;
	BG_CYAN;
	TXT_WHITE;

	printf("Y/X");


	for (int i=0; i<_size; i++)
	{
		printf("%2d|", i);
	}
	TXT_WHITE;
	printf("X/Y");
	C_RESET;
}

void Map::changeCoordStatus(int x, int y, int type)
{
	_map[x][y]=type;
}