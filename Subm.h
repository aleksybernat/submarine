#pragma once

#include "Map.h"

class Subm
{
	int _x, _y, _z;
	int _mtype; //typ pokazywany na mapie: 1 - statek gracza, 2 - statek wroga, 3 - niewidoczny statek wroga, 4 - zniszczony statek wroga

	int _hp; //calkowita wytrzymalosc
	int _attack; //sila torpedy
	int _sp; //maksymalna predkosc
	int _agility; //zwrotnosc
	int _sonar_range; //calkowity zasieg sonaru (podczas nasluchiwania)
	int _perm_sonar_range; //sonar stały
	int _max_z;
	int _attack_range;
	bool _is_death;

	int _am_of_torpedo; //liczba torped


public:
	Subm(int mtype, int x, int y, int z, int hp, int sp, int agility, int sonar_range, int am_of_torpedo, int max_z, int perm_sonar_range, int attack, int attack_range);
	
	int x() {return _x;}
	int y() {return _y;}
	int z() {return _z;}
	int mtype() {return _mtype;}

	int hp() {return _hp;}
	void getHp(int hp);
	int sp() {return _sp;}
	int agility() {return _agility;}
	int sonarRange() {return _sonar_range;}
	int permSonarRange() {return _perm_sonar_range;}
	int amOfTorpedo() {return _am_of_torpedo;}
	void getTorpedo(int amount);
	int max_z() {return _max_z;}
	int attack() {return _attack;}
	int attackRange() {return _attack_range;}
	bool isDeath() {return _is_death;}
	bool kill();

	void showCoord();

	void changeCoord(int x, int y, int z);

	bool changeMType(int n, Map* map);

	double distanceTo(Subm* subm);

	bool damage(int attack, double distance, int attack_range, double& chance, double& damage); //sila ataku strzelajacego i dystans do strzelającego


};