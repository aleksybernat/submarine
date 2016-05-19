#pragma once

class Map
{
	int _size;
	int _map[256][256];

public:
	Map(int size);

	int size() {return _size;}
	int coordStatus(int x, int y) {return _map[x][y];}
	void showMap();

	void changeCoordStatus(int x, int y, int type);
};