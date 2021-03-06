#ifndef PLAYEDTILE_H
#define PLAYEDTILE_H

#include "Pack.h"
#include "Utils.h"
#include "Tile.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <vector>

class PlayedTile : public Pack{

	public:
		//    std::vector<Tile*> tiles;
		//    std::vector<int> idxTiles;
		Tile *tiles[NBR_TILE_MAX];
		int idxHand[NBR_TILE_MAX];

		PlayedTile();
		//PlayedTile(std::vector<Tile*> tilesPlayed, std::vector<int> idxT);
		PlayedTile(Tile *t[], int idx[]);

		void print(std::ostream& f);
		void read(std::istream& f);

		friend std::ostream& operator << (std::ostream &f, PlayedTile &t){
			t.print(f);
			return f;
		};
		friend std::istream& operator >> (std::istream &f, PlayedTile &t){
			t.read(f);
			return f;
		};

};

#endif

