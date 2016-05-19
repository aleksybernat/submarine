#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>

#include "Subm.h"
#include "Map.h"

using namespace std;

Subm::Subm(int mtype, int x, int y, int z, int hp, int sp, int agility, int sonar_range, int am_of_torpedo, int max_z, int perm_sonar_range, int attack, int attack_range)
{
	_mtype=mtype;
	_x=x;
	_y=y;
	_z=z;

	_hp=hp;
	_sp=sp;
	_agility=agility;
	_sonar_range=sonar_range;
	_am_of_torpedo=am_of_torpedo;
	_max_z=max_z;
	_perm_sonar_range=perm_sonar_range;
	_attack=attack;
	_attack_range=attack_range;
	_is_death=0;
}

void Subm::showCoord()
{
	cout << "x = " << _x << endl << "y = " << _y << endl;
}

void Subm::changeCoord(int x, int y, int z)
{
	_x=x;
	_y=y;
	_z=z;
}

double Subm::distanceTo(Subm* subm)
{
	double x1 = _x;
	double y1 = _y;
	double x2 = subm->x();
	double y2 = subm->y();
	double z1 = _z;
	double z2 = subm->z();

	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
}

bool Subm::changeMType(int n, Map* map)
{
	if (n>0 && n<5)
	{
		_mtype=n;
		map->changeCoordStatus(_x, _y, n);

		return 1;
	}
	else return 0;
}

bool Subm::damage(int attack, double distance, int attack_range, double& chance, double& damage)
{
	srand(time(NULL));
	chance=(-(1/(double)(attack_range*attack_range))*distance*distance+1)*100;
	//wzór na określenie szansy trafienia - skąd taki?
	//proste - szansa zależy od dystansu ( im dalej, tym mniejsza, bliższa zeru szansa). Potraktowałem tę
	//zależność parabolicznie. Przy dystansie 0 szansa wynosi jeden, przy dystansie równym zasięgowi maksymalnemu
	//szansa wynosi zero. (podobnie jak przy dystansie ujemnym, którego nie rozpatruję). Wygenerowało mi to
	///trzy punkty - punkt (0,1) oznaczający 100% szans na trafienie, punkt (Z,0) czyli x=zasięg, y=0 (szansa).
	//Podobnie (-Z,0) po drugiej stronie osi OY. Z tego udaje się wyznaczyć trzy współczynniki równania y=ax^2 + bx + c;
	//Z układu równań po podstawieniu wychodzi: a = 1/(Z^2), b = 0 , c = 1. Z - max zasięg strzału.

	//ostatecznie: szansa = 1/(zasięg^2)*dystans^2 + 1;

	if (chance<=0) chance=1; //a gdy szansa jest ujemna to niech bedzie to 1%
	damage=0;

	if (rand()%101 < chance)  //trafiony czy nie?
	{	

		damage=(int)attack*0.2+rand()%((attack+1 - (int)((attack*2)/10)));  //wybieramy ile obrazen dostanie okręt  

		_hp-=(int)damage;
		return 1;
	}

	return 0;
}

void Subm::getTorpedo(int amount)
{
	_am_of_torpedo=amount;
}

bool Subm::kill()
{
	if (_hp<=0) {_is_death=1; return 1;}
	else return 0;
}