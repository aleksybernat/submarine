#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>

#include "Map.h"
#include "Subm.h"

#define TXT_BOLD printf("\x1b[1m");
#define TXT_YELLOW printf("\x1b[33m");
#define TXT_WHITE printf("\x1b[39m");

#define C_RESET printf("\x1b[0m\n");

#define BG_BLUE printf("\x1b[44m");
#define BG_CYAN printf("\x1b[46m");
#define BG_BLACK printf("\x1b[47m");

#define SLEEP usleep(600000);

void gotoxy(int, int);
int lrand(int, int); //losowanie z przedzialu

void clear();

int swim(Subm*, Map*, int, int);
bool dipOut(Subm*, int); //zanurzenie/wynurzenie
void hearing(Subm**, Subm*, Map*, int); //nasluchiwanie

void refreshMap(Subm*, Subm**, Map*);

bool round(Map*, Subm*, Subm**, int* ,int* ,int, int, int&);
bool enemyRound(Subm*, Map*, Subm*, int);

using namespace std;

int main(int argc, char const *argv[])
{
	system("clear");
	TXT_BOLD;

	int amount_of_enemies;
	if(argc>1) amount_of_enemies=atoi(argv[1]);
	else amount_of_enemies=6;

	int shooted_enemies=0;

	srand(time(NULL));
	Map* map = new Map(18);
	Subm* player = new Subm(1, lrand(0,map->size()-1),lrand(0,map->size()-1),10, lrand(10,40), lrand(10,15), lrand(1,7), lrand(6,11), lrand(30,50), lrand(100,120), lrand(2,4), lrand(10,200), lrand(4,8));
	//mtype, x, y, z, hp, speed, agility, sonar_range, am_of_torpedo, max_z, perm_sonar_range
	Subm* enemy[amount_of_enemies];


	map->changeCoordStatus(player->x(), player->y(), player->mtype());
	
	for(int i=0; i<amount_of_enemies; i++)
	{
		enemy[i] = new Subm(3, lrand(0,map->size()-1),lrand(0,map->size()-1),10, lrand(10,40), lrand(10,15), lrand(1,7), lrand(10,18), lrand(30,50), lrand(100, 120), lrand(2,4), lrand(10,20), lrand(7,11));
		map->changeCoordStatus(enemy[i]->x(), enemy[i]->y(), enemy[i]->mtype());
	}

	int counter=0;
	int last_x, last_y; //zmienne dot. ostatniego kursu

	last_y=-1;
	last_x=-1;

	while(round(map, player, enemy, &last_x, &last_y, counter, amount_of_enemies, shooted_enemies)==1)
	{
		counter++;

		for(int i=0; i<amount_of_enemies; i++)
		{
			enemyRound(enemy[i], map, player, amount_of_enemies);	
		}
	}

	C_RESET;

	delete map;
	delete player;
	delete[] *enemy;
	gotoxy(0, 30);
}

void gotoxy(int x, int y)
{
	printf("%c[%d;%df", 0x1b, y, x);
}

int lrand(int n1, int n2)
{
	return n1 + rand()%(n2-n1+1);
}

int swim(Subm* subm, Map* map, int x, int y)
{

	int x1 = subm->x();
	int y1 = subm->y();

	if ((x1==x && y1==y) || (x<0 || x>map->size() || y<0 || y>map->size())) return 0; //0 - bledny kurs

	//gdy kierunek jest rownolegly do ktorejs z osi wspolrzednych:
	if(x1 == x)
	{
		if (y>y1 && !map->coordStatus(x1, y1+1))
		{
			map->changeCoordStatus(subm->x(), subm->y(), 0);
			subm->changeCoord(x1, y1+1, subm->z());
			map->changeCoordStatus(subm->x(), subm->y(), subm->mtype());
			return 1;
		}  //poruszamy sie "w dół"
		else if(map->coordStatus(x1,y1+1)) return 2;
		if (y<y1 && !map->coordStatus(x1, y1-1))
		{
			map->changeCoordStatus(subm->x(), subm->y(), 0);
			subm->changeCoord(x1, y1-1, subm->z());
			map->changeCoordStatus(subm->x(), subm->y(), subm->mtype());
			return 1;
		}
		else if(map->coordStatus(x1,y1-1)) return 2;
	}
	if(y1 == y)
	{
		if (x>x1 && !map->coordStatus(x1+1, y1))
		{
			map->changeCoordStatus(subm->x(), subm->y(), 0);
			subm->changeCoord(x1+1, y1, subm->z());
			map->changeCoordStatus(subm->x(), subm->y(), subm->mtype());
			return 1;
		}//poruszamy sie "w bok"
		else if(map->coordStatus(x1+1,y1)) return 2;
		if (x<x1 && !map->coordStatus(x1-1, y1))
		{
			map->changeCoordStatus(subm->x(), subm->y(), 0);
			subm->changeCoord(x1-1, y1, subm->z());
			map->changeCoordStatus(subm->x(), subm->y(), subm->mtype());
			return 1;
		}
		else if(map->coordStatus(x1-1,y1)) return 2;
	}
	//gdy kierunek jest bardziej złożony to mam dwa punkty i potrzebuje wyznaczyc wsp. kierunkowy prostej:
	//w tym celu potraktuje okręt jako punkt 0,0 (czyli odejme jego wspolrzedne od współrzednych kursu)
	//to pozwoli mi pominac jeden parametr prostej (wspolczynnik b ze wzoru y=ax+b, bo prosta przechodzi przez 0,0)
	//a skoro mam dane dotyczace dwoch punktow przez ktory przechodzi prosta (tym drugim punktem jest
	//wyprowadzona wczesniej roznica), to wzor ostateczny wyglada tak:

	double a = (double)(y-y1)/(double)(x-x1);

	//znajac współczynnik kierunkowy moge okreslic kąt miedzy osią X a prostą. Jeśli kąt ten będzie większy niż
	//45 stopni to należy się skierować po współrzędnej y (w górę lub w dół), jeśli zaś mniejszy to w pierwszej
	//kolejności po x.
		if(a > 0.5)
	{
		if (y>y1 && !map->coordStatus(x1, y1+1))
		{
			map->changeCoordStatus(subm->x(), subm->y(), 0);
			subm->changeCoord(x1, y1+1, subm->z());
			map->changeCoordStatus(subm->x(), subm->y(), subm->mtype());
			return 1;
		}  //poruszamy sie "w dół"
		else if(map->coordStatus(x1,y1+1)) return 2;
		if (y<y1 && !map->coordStatus(x1, y1-1))
		{
			map->changeCoordStatus(subm->x(), subm->y(), 0);
			subm->changeCoord(x1, y1-1, subm->z());
			map->changeCoordStatus(subm->x(), subm->y(), subm->mtype());
			return 1;
		}
		else if(map->coordStatus(x1,y1-1)) return 2;
	}
	if(a <= 0.5)
	{
		if (x>x1 && !map->coordStatus(x1+1, y1))
		{
			map->changeCoordStatus(subm->x(), subm->y(), 0);
			subm->changeCoord(x1+1, y1, subm->z());
			map->changeCoordStatus(subm->x(), subm->y(), subm->mtype());
			return 1;
		}//poruszamy sie "w bok"
		else if(map->coordStatus(x1+1,y1)) return 2;
		if (x<x1 && !map->coordStatus(x1-1, y1))
		{
			map->changeCoordStatus(subm->x(), subm->y(), 0);
			subm->changeCoord(x1-1, y1, subm->z());
			map->changeCoordStatus(subm->x(), subm->y(), subm->mtype());
			return 1;
		}
		else if(map->coordStatus(x1-1,y1)) return 2;
	}
	return 0;
}


bool dipOut(Subm* subm, int z)
{
	if ((subm->z()==0 && z>0) || (subm->z()==subm->max_z() && z<0)) return 0;
	else
	{
		if (z<0) {subm->changeCoord(subm->x(), subm->y(), subm->z()-1); return 1;}
		if (z>0) {subm->changeCoord(subm->x(), subm->y(), subm->z()+1); return 1;}
		if (z==0) return 0;
	}
	return 0;
}

bool round(Map* map, Subm* player, Subm** enemy, int* last_x, int* last_y, int counter, int amount_of_enemies, int& shooted_enemies)
{

	int info=-1;
	int mv_points=20; //punkty ruchu na turę
	char action;
	do
	{
	
	C_RESET;
	TXT_BOLD;
	TXT_WHITE;
	gotoxy(0,0);
	printf("SUBMARINE SIMULATOR v. 1.0");

	map->showMap();
		if (action!='n')
		{
			for (int i=0; i<amount_of_enemies; i++)
			{
				if (player->distanceTo(enemy[i]) <= player->permSonarRange()) enemy[i]->changeMType(2, map); //widoczność
				else enemy[i]->changeMType(3, map);
				if (enemy[i]->isDeath()==1) enemy[i]->changeMType(4, map);
			}
		}

	C_RESET;
	TXT_BOLD;
	TXT_YELLOW;	
	switch (info) //komunikaty dodatkowe!
	{
		case 0:
		{gotoxy(62,20); printf("WPROWADZONO BŁEDNY KURS!"); break;}
		case 1:
		{gotoxy(62,20); printf("NA KURSIE ZNAJDUJE SIĘ OBCY OKRĘT!"); break;}
		case 2:
		{gotoxy(62,20); printf("NOWY KURS JEST TAKI JAK DOCELOWY!");break;}
		case 3:
		{gotoxy(62,20); printf("NA KURSIE ZNAJDUJE SIĘ OBCY OKRĘT!");break;}
	}
	info=-1;
	TXT_WHITE;

	gotoxy(1, 23); printf("Tura: %d   Pozostałe punkty ruchu: %d, Wrogich okrętów: %d/%d", counter, mv_points, amount_of_enemies-shooted_enemies, amount_of_enemies);
	gotoxy(62, 2); printf("STATYSTYKI OKRĘTU:");	
	gotoxy(62, 3); printf("> Wytrzymałość: %d", player->hp());	
	gotoxy(62, 4); printf("> Szybkość maksymalna: %d", player->sp());
	gotoxy(62, 5); printf("> Zwrotność: %d", player->agility());
	gotoxy(62, 6); printf("> Zasięg sonaru:");
	gotoxy(62, 7); printf(" >> Nasłuchiwanie: %d", player->sonarRange());
	gotoxy(62, 8); printf(" >> Sonar stały: %d", player->permSonarRange());
	gotoxy(62, 9); printf("> Liczba torped: %d", player->amOfTorpedo());
	gotoxy(62, 10); printf(" >> Siła torpedy: %d", player->attack());
	gotoxy(62, 11); printf(" >> Zasieg strzału: %d", player->attackRange());

	gotoxy(90, 2); printf("DOSTĘPNE AKCJE:");  
	gotoxy(90, 3); printf("Płyń          - p");
	gotoxy(90, 4); printf("Ostatni kurs  - o");
	gotoxy(90, 5); printf("Wynurz        - w");
	gotoxy(90, 6); printf("Zanurz        - z");
	gotoxy(90, 7); printf("Nasłuchuj     - n");
	gotoxy(90, 8); printf("Strzelaj      - s");

	gotoxy(120, 2); printf("INFORMACJE:");
	gotoxy(120, 3); printf("Koordynaty: (%d, %d, %d)", player->x(), player->y(), player->z());
	gotoxy(120, 4); printf("Ostatni kurs: (%d, %d)", *last_x, *last_y);


	gotoxy(62,16); printf("WPROWADŹ ZNAK AKCJI: ");
	cin >> action;
	cin.clear(); cin.sync();

	switch (action)
	{
		case 'p': 
		{

			int x, y;
			gotoxy(62,17);
			printf("Wprowadź kurs:");
			gotoxy(62,18); printf("x = "); cin >> x; cin.clear(); cin.sync();
			gotoxy(62,19); printf("y = "); cin >> y; cin.clear(); cin.sync();
			*last_x=x;
			*last_y=y;
			clear();
			TXT_BOLD;
			gotoxy(62,20); printf("Podpływanie...");

			int s=swim(player, map, x, y);

			if (s==0)
				{info=0; break;}
			if (s==2)
				{info=1; break;}
			mv_points-=4;
//			gotoxy(-12,0);
			break;
		}
		case 'o':
		{
			clear();
			gotoxy(62,17);
			TXT_BOLD;
			printf("Kurs: (%d, %d)", *last_x, *last_y);
			
		
			int s=swim(player, map, *last_x, *last_y);
			if (s==0)
				{info=2; break;}
			if (s==2)
				{info=3; break;}
			mv_points-=4;
			break;
		}
		case 'w':
		{
			clear();
			gotoxy(62,17);
			TXT_BOLD;
			printf("Wynurzanie...");
			dipOut(player, -1);

			break;
		}
		case 'z':
		{
			clear();
			gotoxy(62,17);
			TXT_BOLD;
			printf("Zanurzanie...");

			dipOut(player, 1);

			break;
		}
		case 'n':
		{
			clear();
			gotoxy(62,17);
			TXT_BOLD;
			printf("Nasłuchiwanie...");
			
			hearing(enemy, player, map, amount_of_enemies);
			
			mv_points-=10;
			break;
		}
		case 's':
		{
			int x,y;
			gotoxy(62,17);
			printf("Wprowadź koordynaty celu:");
			gotoxy(62,18); printf("x = "); cin >> x; cin.clear(); cin.sync();
			gotoxy(62,19); printf("y = "); cin >> y; cin.clear(); cin.sync();
			clear();
			TXT_BOLD;

			if (player->amOfTorpedo()>0 && map->coordStatus(x, y)!=0  && map->coordStatus(x, y)!=4)
				{
					double damageData[2];
					
					bool hit;
					for(int i=0; i<amount_of_enemies; i++)
					{
						if (enemy[i]->x()==x && enemy[i]->y()==y)
						{
							gotoxy(62,19);
							hit = enemy[i]->damage(player->attack(), player->distanceTo(enemy[i]), player->attackRange(), damageData[0], damageData[1]);
							player->getTorpedo(player->amOfTorpedo()-1);
							if (enemy[i]->kill()==1) shooted_enemies++;
							break;
						}
					}
					if (hit==1)
					{
						gotoxy(62,20); printf("Trafiono w cel: [Przy szansie %d%%]", (int)damageData[0]);
						gotoxy(62,21); printf("Otrzymane obrazenia: %d / %d", (int)damageData[1], player->attack());
					}
					else
					{
						gotoxy(62,20); printf("Nie trafiono w cel. [Przy szansie %d%%]", (int)damageData[0]);
					}
			}
		break;
		}
		case 'd':
		{
			int x,y;
			gotoxy(62,17);
			printf("Wprowadź koordynaty okrętu:");
			gotoxy(62,18); printf("x = "); cin >> x; cin.clear(); cin.sync();
			gotoxy(62,19); printf("y = "); cin >> y; cin.clear(); cin.sync();
			clear();
			TXT_BOLD;
			if (map->coordStatus(x, y)!=0)
				{
					for(int i=0; i<amount_of_enemies; i++)
					{
						if (enemy[i]->x()==x && enemy[i]->y()==y)
						{
							gotoxy(62,20); printf("DANE OKRĘTU:");
							gotoxy(62,21); printf("Hp: %d", enemy[i]->hp());
							gotoxy(62,22); printf("Odległość od okrętu: %f", enemy[i]->distanceTo(player));
							break;
						}
					}
				}		
			break;
		}
	}

	C_RESET;
	}
	while(mv_points>0);
	return 1;
}

bool enemyRound(Subm* subm, Map* map, Subm* player, int amount_of_enemies)
{
	if (subm->isDeath()==0)
	{
		int dir=lrand(0,3);

		if(dir==0) swim(subm, map, subm->x()+1 , subm->y());
		if(dir==1) swim(subm, map, subm->x() , subm->y()+1);
		if(dir==2) swim(subm, map, subm->x()-1 , subm->y());
		if(dir==3) swim(subm, map, subm->x() , subm->y()-1);

		if (player->distanceTo(subm) <= player->permSonarRange()) subm->changeMType(2, map); //widoczność
		else subm->changeMType(3, map);
	}
	else subm->changeMType(4, map);
	return 0;
}

void hearing(Subm** enemy, Subm* player, Map* map, int amount_of_enemies)
{
	for (int i=0; i<amount_of_enemies; i++)
	{
		if (player->distanceTo(enemy[i]) <= player->sonarRange()) enemy[i]->changeMType(2, map); //widoczność
		else enemy[i]->changeMType(3, map);
		if (enemy[i]->isDeath()==1) enemy[i]->changeMType(4, map);
	}
}

void clear()
{
	C_RESET;
	gotoxy(0,0); 
	for(int i=0; i<40000; printf(" "), i++); 
	gotoxy(0,0);
}

void refreshMap(Subm*, Subm**, Map*)
{

}