#ifndef TILE_H
#define TILE_H
#include "Square.h"
#include <iostream>
#include "Utils.h"
#include <vector>
#include <fstream>

typedef struct {
	Orientation s1,s2; // s1 <= s2 .
} Rail;

bool operator == (Rail &r1, Rail &r2);
bool operator != (Rail &r1, Rail &r2);

class Tile : public Square {
	public:
		Tile(idTile t = Empty, int x = -1, int y = -1, int p = -1); // type is t and idPlayer is p
		~Tile();

		void setStop(bool s = true);
		bool isStop();

		bool getAccess(Orientation o);
		bool canChange(Square *s);
		void rotate();

		bool haveRail(Rail r);

		void setType(idTile id);
		int getTurn();
		void setPlayer(int id);
		int getPlayer();

		void print();

		Tile &operator = (Tile const & t);

		friend std::ostream& operator << (std::ostream &f, Tile &t);
		friend std::istream& operator >> (std::istream &f, Tile &t);

	protected:
		bool tree;
		std::vector<Rail> ways;
		int turn;
		int idPlayer;
		bool stop;
		bool access[4];
};

#endif
