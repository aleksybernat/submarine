#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>

#include "Map.h"
#include "Subm.h"

#define TXT_BOLD printf("\x1b[1m");
#define TXT_YELLOW printf("\x1b[33m");
#define TXT_RED printf("\x1b[31m");
#define TXT_WHITE printf("\x1b[39m");
#define TXT_GREEN printf("\x1b[32m");

#define C_RESET printf("\x1b[0m\n");

#define BG_BLUE printf("\x1b[44m");
#define BG_CYAN printf("\x1b[46m");
#define BG_BLACK printf("\x1b[47m");
#define MV_POINTS 24

#define SLEEP usleep(600000);

void gotoxy(int, int);
int lrand(int, int); //losowanie z przedzialu

void clear();

int swim(Subm*, Map*, int, int);
bool dipOut(Subm*, int); //zanurzenie/wynurzenie
void hearing(Subm**, Subm*, Map*, int); //nasluchiwanie
int spToMv(Subm*);  //funkcja przelicza szybkosc statku na zużywane punkty ruchu

bool round(Map*, Subm*, Subm**, int* ,int* ,int, int, int&);
int enemyRound(Subm*, Map*, Subm*, int); //zwraca ilosc zadanych obrazeń

using namespace std;

int main(int argc, char const *argv[])
{
	system("clear");
	TXT_BOLD;

	int amount_of_enemies;
	if(argc>1) amount_of_enemies=atoi(argv[1]);
	else amount_of_enemies=4;
	if (amount_of_enemies>32) amount_of_enemies=32;

	int shooted_enemies=0;

	srand(time(NULL));
	Map* map = new Map(18);
	Subm* player = new Subm(1, lrand(0,map->size()-1),lrand(0,map->size()-1),10, lrand(20,50), lrand(4,13), lrand(1,7), lrand(7,15), lrand(100,200), lrand(11,20), lrand(3,6), lrand(10,20), lrand(4,8));
	//mtype, x, y, z, hp, speed, agility, sonar_range, am_of_torpedo, max_z, perm_sonar_range, attack, attack_range
	Subm* enemy[amount_of_enemies];

	int damage;

	map->changeCoordStatus(player->x(), player->y(), player->mtype());
	
	for(int i=0; i<amount_of_enemies; i++)
	{
		int x,y;
		for(x=lrand(0,map->size()-1), y=lrand(0,map->size()-1); map->coordStatus(x,y) ; x=lrand(0,map->size()-1), y=lrand(0,map->size()-1)); //dopóki stan wylosowanych koordynatów jest różny od zera to szukamy miejsca na statek
		enemy[i] = new Subm(3,x,y,10, lrand(10,20), lrand(10,15), lrand(1,7), lrand(10,18), lrand(30,50), lrand(13, 20), lrand(2,4), lrand(4,8), lrand(5,9));
		map->changeCoordStatus(enemy[i]->x(), enemy[i]->y(), enemy[i]->mtype());
	}

	int counter=0;
	int last_x, last_y; //zmienne dot. ostatniego kursu

	last_y=-1;
	last_x=-1;

	damage=0;
	while(round(map, player, enemy, &last_x, &last_y, counter, amount_of_enemies, shooted_enemies)==1)
	{
		system("clear");
		counter++;
		for(int i=0; i<amount_of_enemies; i++)
		{
			damage+=enemyRound(enemy[i], map, player, amount_of_enemies);	

			
		}
		if (damage>0)
			{
				TXT_BOLD;
				TXT_RED;
				gotoxy(100,20); printf("Trafiono w nasz okręt!");
				gotoxy(100,21); printf("Otrzymane obrazenia: %d", damage);
			damage=0;
			}
		if (player->hp()<=0 || shooted_enemies==amount_of_enemies)
		{
			break;
		}
	}
	
	clear();
	printf("SUBMARINE SIMULATOR v. 1.0");

	map->showMap();
		if (player->hp()<=0)
			{gotoxy(62, 2); TXT_RED; TXT_BOLD; printf("Porażka! Nasz okręt zatonął, koniec bitwy!");}
		if (shooted_enemies==amount_of_enemies)
			{gotoxy(62 ,2); TXT_RED; TXT_BOLD; printf("Zwycięstwo! Zatopiono wszystkie okręty!");}
		if (player->hp()>0 && shooted_enemies!=amount_of_enemies)
			{gotoxy(62 ,2); TXT_RED; TXT_BOLD; printf("Opuszczono nasz okręt.");}
		


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
	if ((subm->z()==0 && z<0) || (subm->z()==subm->max_z() && z>0)) return 0;
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
	int mv_points=24; //punkty ruchu na turę
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
		{gotoxy(100,18); printf("INFO: Wprowadzono błędny kurs."); break;}
		case 1:
		{gotoxy(100,18); printf("INFO: Na kursie znajduje się obcy okręt."); break;}
		case 2:
		{gotoxy(100,18); printf("INFO: Błędny kurs.");break;}
		case 3:
		{gotoxy(100,18); printf("INFO: Na kursie znajduje się obcy okręt.");break;}
		case 4:
		{gotoxy(100,18); printf("INFO: Brak wystarczającej ilości punktów ruchu."); break;}
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
	gotoxy(62, 12); printf("> Maksymalne zanurzenie: %d", player->max_z());

	gotoxy(100, 2); printf("DOSTĘPNE AKCJE  - SKRÓT  -  KOSZT:");  
	gotoxy(100, 3); printf("Płyń            -  p     -   %d",spToMv(player));
	gotoxy(100, 4); printf("Ostatni kurs    -  o     -   %d",spToMv(player));
	gotoxy(100, 5); printf("Wynurz          -  w     -   %d",spToMv(player));
	gotoxy(100, 6); printf("Zanurz          -  z     -   %d",spToMv(player));
	gotoxy(100, 7); printf("Nasłuchuj       -  n     -   12");
	gotoxy(100, 8); printf("Strzelaj        -  s     -   5");
	gotoxy(100, 9); printf("Omiń turę       -  q     -   0");
	gotoxy(100, 10); printf("WYJŚCIE Z GRY   -  X");

	gotoxy(100, 12); printf("INFORMACJE:");
	gotoxy(100, 13); printf("Koordynaty: (%d, %d, %d)", player->x(), player->y(), player->z());
	gotoxy(100, 14); printf("Ostatni kurs: (%d, %d)", *last_x, *last_y);
	gotoxy(100, 16); printf("PANEL KOMUNIKATÓW DODATKOWYCH:");

	gotoxy(62,16); printf("WPROWADŹ ZNAK AKCJI: ");
	cin >> action;
	cin.clear(); cin.sync();

	switch (action)
	{
		case 'p':
		{
			
			int x, y;
			gotoxy(62,17);
			if (mv_points>=spToMv(player))
			{
			printf("Wprowadź kurs:");
			gotoxy(62,18); printf("x = "); cin >> x; cin.clear(); cin.sync();
			gotoxy(62,19); printf("y = "); cin >> y; cin.clear(); cin.sync();
			*last_x=x;
			*last_y=y;
			clear();
			TXT_BOLD; TXT_GREEN;
			gotoxy(100,17); printf("Podpływanie...");
			TXT_WHITE;
			int s=swim(player, map, x, y);

			if (s==0)
				{info=0; break;}
			if (s==2)
				{info=1; break;}
			mv_points-=spToMv(player);
			break;
			}
			else
			{
			info=4;
			break;
			}
		}
		case 'o':
		{
			
			clear();
			gotoxy(100,17);
			TXT_BOLD;
			if (mv_points>=spToMv(player))
			{
			TXT_GREEN;
			printf("Kurs: (%d, %d)", *last_x, *last_y);
			TXT_WHITE;
		
			int s=swim(player, map, *last_x, *last_y);
			if (s==0)
				{info=2; break;}
			if (s==2)
				{info=3; break;}
			mv_points-=spToMv(player);
			break;
			}
			else
			{
			info=4;
			break;
			}
		}
		case 'w':
		{
			clear();
			gotoxy(100,17);
			TXT_BOLD;
			if (mv_points>=spToMv(player))
			{
			TXT_GREEN;
			printf("Wynurzanie...");
			TXT_WHITE;
			dipOut(player, -1);
			mv_points-=spToMv(player);
			break;
			}
			else
			{
			info=4;
			break;
			}
		}
		case 'z':
		{
			clear();
			gotoxy(100,17);
			TXT_BOLD;
			if (mv_points>=spToMv(player))
			{
			TXT_GREEN;
			printf("Zanurzanie...");
			
			TXT_WHITE;
			mv_points-=spToMv(player);
			dipOut(player, 1);

			break;
			}
			else
			{
			info=4;
			break;
			}
		}
		case 'n':
		{
			clear();
			gotoxy(100,17);
			TXT_BOLD;
			if (mv_points>=12)
			{
			TXT_GREEN;
			printf("Nasłuchiwanie...");
			TXT_WHITE;
			hearing(enemy, player, map, amount_of_enemies);
			
			mv_points-=12;
			break;
			}
			else
			{
			info=4;
			break;
			}
		}
		case 's':
		{
			int x,y;
			gotoxy(62,17);
			if (mv_points>=5)
			{
			printf("Wprowadź koordynaty celu:");
			gotoxy(62,18); printf("x = "); cin >> x; cin.clear(); cin.sync();
			gotoxy(62,19); printf("y = "); cin >> y; cin.clear(); cin.sync();
			clear();
			TXT_BOLD;
			player->getTorpedo(player->amOfTorpedo()-1);
			mv_points-=5;

			if (player->amOfTorpedo()>0 && map->coordStatus(x, y)!=0  && map->coordStatus(x, y)!=4)
				{
					double damageData[2];
					
					bool hit;
					for(int i=0; i<amount_of_enemies; i++)
					{
						if (enemy[i]->x()==x && enemy[i]->y()==y && enemy[i]->mtype()!=4)
						{
							gotoxy(62,19);
							hit = enemy[i]->damage(player->attack(), player->distanceTo(enemy[i]), player->attackRange(), damageData[0], damageData[1]);
			
							if (enemy[i]->kill()==1) shooted_enemies++;
							break;
						}
					}
					TXT_GREEN;
					if (hit==1)
					{
						gotoxy(100,20); printf("Trafiono w cel: [Przy szansie %d%%]", (int)damageData[0]);
						gotoxy(100,21); printf("Otrzymane obrazenia: %d / %d", (int)damageData[1], player->attack());
					}
					else
					{
						gotoxy(100,20); printf("Nie trafiono w cel. [Przy szansie %d%%]", (int)damageData[0]);
					}
					TXT_WHITE;
				break;
				}

			if(player->amOfTorpedo()==0) {gotoxy(100,20); TXT_RED; printf("BRAK TORPED!"); TXT_WHITE;}
			if(map->coordStatus(x,y)==0 || map->coordStatus(x,y)==4) {gotoxy(100,20);TXT_YELLOW; printf("Strzelono w miejsce gdzie nie ma okrętu."); TXT_WHITE;}
		break;
		}
			else
			{
			info=4;
			break;
			}
		}
		case 'q':
		{
			gotoxy(100, 17);
			TXT_GREEN;
			printf("Czekanie...");
			TXT_WHITE;
			mv_points=0;
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
							TXT_GREEN;
							gotoxy(100,20); printf("DANE OKRĘTU:");
							gotoxy(100,21); printf("Hp: %d", enemy[i]->hp());
							gotoxy(100,22); printf("Odległość od okrętu: %f", enemy[i]->distanceTo(player));
							TXT_WHITE;
							break;
						}
					}
				}		
			break;
		}
		case 'X':
		{
			return 0;
			break;
		}
	}

	C_RESET;
	}
	while(mv_points>0);
	return 1;
}

int enemyRound(Subm* subm, Map* map, Subm* player, int amount_of_enemies)
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
	
	if (subm->distanceTo(player)<subm->attackRange())
		{
			double damageData[2];
			bool hit = player->damage(subm->attack(), subm->distanceTo(player),subm->attackRange(),damageData[0], damageData[1]);
			TXT_BOLD;
			TXT_RED;
			if (hit==1)
			{
				return damageData[1];
			}
			else return 0;
		}

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
	system("clear");
	gotoxy(0,0); 
	for(int i=0; i<22*4*3; printf(" "), i++); 
	gotoxy(0,0);
}

int spToMv(Subm* subm)
{
	if (subm->sp()>=MV_POINTS) return 1;
	if (subm->sp()<=1) return MV_POINTS;

	return MV_POINTS/subm->sp(); 
}