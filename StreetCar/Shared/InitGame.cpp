#include "InitGame.h"

using namespace std;

InitGame::InitGame() : Pack(INITGAME) {} 

InitGame::InitGame(vector<vector<Tile> > h, Pile p, int firstP, GoalPlayer goalP) : Pack(INITGAME){
	hands = h;
	pile = p;
	idFirstPlayer = firstP;
	goalPlayer = goalP;
}

void InitGame::print(ostream& f){
	Pack::print(f);
	f << hands.size() << " ";
	for (unsigned int i = 0; i<hands.size(); i++)
		for (int j = 0; j<5; j++)
			f << hands[i][j] << " ";
	f << pile << " "; //TODO redefinition of ostream & istream in Pile
	f << idFirstPlayer << " ";

	f << goalPlayer.stop << " ";

	f << goalPlayer.line << " ";
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
	Pile p;
	f >> p;
	pile = p;

	f >> idFirstPlayer;
	Card card;
	f >> card;
	int l;
	f >> l;
	GoalPlayer gp;
	gp.stop = card;
	gp.line = l;
	goalPlayer = gp;

}

