#include "InitGame.h"

using namespace std;

InitGame::InitGame() : Pack(INITGAME) {} 

InitGame::InitGame(vector<vector<Tile> > h, int firstP) : Pack(INITGAME){
	hands = h;
	idFirstPlayer = firstP;
}

void InitGame::print(ostream& f){
	Pack::print(f);
	f << hands.size() << " ";
	for (unsigned int i = 0; i<hands.size(); i++)
		for (int j = 0; j<5; j++)
			f << hands[i][j] << " ";
	f << idFirstPlayer;
}

void InitGame::read(istream& f){
	int sizeOfHands;
	f >> sizeOfHands;

	hands.clear();
	for (int i = 0; i<sizeOfHands; i++){
		vector<Tile> h;
		h.clear();
		for (int j = 0; j<5; j++){
			Tile tile;
			f >> tile;
			h.push_back(tile);
		}
		hands.push_back(h);
	}
	f >> idFirstPlayer;
}

