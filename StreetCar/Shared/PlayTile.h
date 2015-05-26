#ifndef PLAYTILE_H
#define PLAYTILE_H

#include "Pack.h"
#include "Utils.h"
#include "Tile.h"
#include <iostream>
#include <vector>
#include <fstream>

class PlayTile : public Pack{

 public:

    int idPlayer;
    Tile tiles[2];
    int idxHand[2];

    PlayTile();
    PlayTile(int idP, Tile myHand[5], int idTilePlay[2]);

    void print(std::ostream& f);
    void read(std::istream& f);
    
};

#endif
