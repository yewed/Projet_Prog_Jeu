#include "clientGuiHandler.h"
#include "../Shared/ParamThreadClient.h"
#include "../Shared/Packs.h"
#include "../Shared/Pack.h"
#include "../Shared/Board.h"
#include "../Shared/StartTravel.h"
#include "../Shared/PlayTravel.h"
#include "../Shared/StopTravel.h"
#include "../Shared/PlayTile.h"
#include "../Shared/Pile.h"
#include "../Shared/PileWhenTravel.h"
#include "../Shared/PileTarget.h"
#include "../Shared/Card.h"
#include "../Shared/InitGame.h"
#include "../Shared/PlayedTile.h"
#include "../Shared/CreateGame.h"
#include "../Shared/IWantPlay.h"
#include "../Shared/NewPlayerAdd.h"
#include "../Shared/StartGame.h"
#include "../Shared/YourIdPlayer.h"
#include "CircularQueueClient.h"
#include "../Shared/Debug.h"
#include "../Shared/PilePlayer.h"
#include "GameState.h"
#include "../Shared/Validation.h"
#include "../Shared/StoppedTravel.h"
#include "../Shared/PlayedTravel.h"
#include "PlayerServer.h"
#include "../Shared/Connexion.h"

using namespace std;


#define PULLPLAYER 6

// sends an error pack to the specified error with the error descriptor
void sendError(int player, error_pack error, GameState *gameState){
	// Here we send the error to the player
	Validation *validation = new Validation(error);
	gameState->getPlayer(player)->circularQueue->produce(validation);
	cout << "S: error " << error  << endl;
}

// handling of a STARTTRAVEL pack
void travelStarted(StartTravel *readPack, GameState *gameState){

	PlayerServer* currentP = gameState->getPlayer(readPack->idPlayer); 
	if (readPack->idPlayer != gameState->getCurrentPlayer())
		sendError(readPack->idPlayer, WRONG_PLAYER, gameState);
	// now we will check if there isn't too many tiles
	else if (readPack->travel.size() > (gameState->getLastTravelLength()+1))
		sendError(readPack->idPlayer, TOO_MANY_TILES, gameState);
	else{
		currentP->setTravelling(true);
		// here we need to check if there is no stop on the way or if there is actualy a way from the current place to the next
		Tile* prevTile;
		int i;
		bool done = false;
		Orientation orientation;
		while ((i < readPack->travel.size()) && !done){
			if (i==0)
				// we need to check if the first tile is accessible from the current Tile
				prevTile = currentP->getTravel()->curTile;
			Tile* currentTile = (Tile*) gameState->gameBoard->get(readPack->travel[i].getCoordinates().x, readPack->travel[i].getCoordinates().y);
			if((prevTile->getCoordinates().x == currentTile->getCoordinates().x)&&(prevTile->getCoordinates().y == currentTile->getCoordinates().y + 1))
				orientation = NORTH;
			else if((prevTile->getCoordinates().x == currentTile->getCoordinates().x)&&(prevTile->getCoordinates().y == currentTile->getCoordinates().y - 1))
				orientation  = SOUTH;
			else if((prevTile->getCoordinates().x == currentTile->getCoordinates().x+1)&&(prevTile->getCoordinates().y == currentTile->getCoordinates().y))
				orientation  = EAST;
			else if((prevTile->getCoordinates().x == currentTile->getCoordinates().x-1)&&(prevTile->getCoordinates().y == currentTile->getCoordinates().y))
				orientation = WEST;
			else { 
				sendError(readPack->idPlayer, WRONG_WAY, gameState);
				return;
			}

			if ((currentTile->getType() == Empty)||(!gameState->gameBoard->adjacentPossible(currentTile, prevTile, orientation))){
				sendError(readPack->idPlayer, WRONG_WAY, gameState);
				return;
			} else if (currentTile->isStop())
				done = true;
			i++;
		}
		currentP->getTravel()->prevTile = prevTile;
		currentP->getTravel()->curTile = (Tile*) gameState->gameBoard->get(readPack->travel[i].getCoordinates().x, readPack->travel[i].getCoordinates().x);
		currentP->getTravel()->origin = orientation;
		gameState->setLastTravelLength(i+1);
		gameState->setCurrentPlayer((gameState->getCurrentPlayer() + 1) % gameState->getPlayers().size());

		for (int i = 0; i < gameState->getPlayers().size(); i++){
			PlayedTravel* playedTravel = new PlayedTravel(gameState->getCurrentPlayer(), *currentP->getTravel());
			gameState->getPlayer(i)->circularQueue->produce(playedTravel);

		}



	}
} 

// handling of a PLAYTRAVEL pack
void travelPlayed(PlayTravel *readPack, GameState *gameState){

	PlayerServer* currentP = gameState->getPlayer(readPack->idPlayer); 
	// checking if right player then if his travel started
	if (readPack->idPlayer != gameState->getCurrentPlayer())
		sendError(readPack->idPlayer, WRONG_PLAYER, gameState);
	else if (!currentP->getTravelling())
		sendError(readPack->idPlayer, TRAVEL_NOT_STARTED, gameState);
	// now we will check if there isn't too many tiles
	else if (readPack->travel.size() > (gameState->getLastTravelLength()+1))
		sendError(readPack->idPlayer, TOO_MANY_TILES, gameState);
	else{
		// here we need to check if there is no stop on the way or if there is actualy a way from the current place to the next
		Tile* prevTile;
		int i;
		bool done = false;
		Orientation orientation;
		while ((i < readPack->travel.size()) && !done){
			if (i==0)
				// we need to check if the first tile is accessible from the current Tile
				prevTile = currentP->getTravel()->curTile;
			Tile* currentTile = (Tile*) gameState->gameBoard->get(readPack->travel[i].x, readPack->travel[i].y);
			if((prevTile->getCoordinates().x == currentTile->getCoordinates().x)&&(prevTile->getCoordinates().y == currentTile->getCoordinates().y + 1))
				orientation = NORTH;
			else if((prevTile->getCoordinates().x == currentTile->getCoordinates().x)&&(prevTile->getCoordinates().y == currentTile->getCoordinates().y - 1))
				orientation  = SOUTH;
			else if((prevTile->getCoordinates().x == currentTile->getCoordinates().x+1)&&(prevTile->getCoordinates().y == currentTile->getCoordinates().y))
				orientation  = EAST;
			else if((prevTile->getCoordinates().x == currentTile->getCoordinates().x-1)&&(prevTile->getCoordinates().y == currentTile->getCoordinates().y))
				orientation = WEST;
			else { 
				sendError(readPack->idPlayer, WRONG_WAY, gameState);
				return;
			}

			if ((currentTile->getType() == Empty)||(!gameState->gameBoard->adjacentPossible(currentTile, prevTile, orientation))){
				sendError(readPack->idPlayer, WRONG_WAY, gameState);
				return;
			} else if (currentTile->isStop())
				done = true;
			i++;
		}
		currentP->getTravel()->prevTile = prevTile;
		currentP->getTravel()->curTile = (Tile*) gameState->gameBoard->get(readPack->travel[i].x, readPack->travel[i].x);
		currentP->getTravel()->origin = orientation;
		gameState->setLastTravelLength(i+1);
		gameState->setCurrentPlayer((gameState->getCurrentPlayer() + 1) % gameState->getPlayers().size());
		PlayedTravel* playedTravel = new PlayedTravel(gameState->getCurrentPlayer(), *currentP->getTravel());
		for (int i = 0; i < gameState->getPlayers().size(); i++){
			gameState->getPlayer(i)->circularQueue->produce(playedTravel);

		}

	}
}

// handling of a STOPTRAVEL pack
void travelStopped(StopTravel *readPack, GameState *gameState){

	// TO-DO checking validation
	if (readPack->idPlayer != gameState->getCurrentPlayer())
		sendError(readPack->idPlayer, WRONG_PLAYER, gameState);
	else if (!gameState->getPlayer(readPack->idPlayer)->getTravelling())
		sendError(readPack->idPlayer, TRAVEL_NOT_STARTED, gameState);
	else {
		// TO-DO throw validation and update of the board
		gameState->getPlayer(readPack->idPlayer)->setTravelling(false);

		for (int i = 0; i < gameState->getPlayers().size(); i++){
			StoppedTravel* stoppedTravel = new StoppedTravel((readPack->idPlayer + 1 % gameState->getPlayers().size()));
			gameState->getPlayer(i)->circularQueue->produce(stoppedTravel);
		}

	}
}

// handling of a PLAYTILE pack
void tilePlayed(PlayTile *readPack, GameState *gameState){

	vector<Tile*> tilePlay;
	cout << "S: Player in readPack : " << endl; //*readPack << endl;
	cout << "S: current Player : " << gameState->getCurrentPlayer() << endl;


	// test if it is the good player to play
	if (readPack->idPlayer != gameState->getCurrentPlayer()){
		sendError(readPack->idPlayer, WRONG_PLAYER, gameState);
		gameState->setTakePile(true);
		return;
	}

	// take the index of the two square play
	for(int i = 0; i < NBR_TILE_MAX; i++){
		gameState->idxhand[i] = readPack->idxHand[i];
	}
	cout << "S: Player hand ------------------------";
	for (int i = 0; i < 5; i ++)
		cout << gameState->getPlayer(gameState->getCurrentPlayer())->getHand()[i]->getType() << " ";
	cout << endl;

	// shortcut of the hand of player
	Tile **playersHand = gameState->getPlayer(readPack->idPlayer)->getHand();

	cout << "xxxxxxxxxxxxxxxx next Player : " << (gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer() << endl;
	cout << "hand :     ";
	for(int i = 0; i< HAND_SIZE; i++){
		cout << "add : " <<gameState->getPlayer((gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer())->getHand(i) << " "  << gameState->getPlayer((gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer())->getHand(i)->getType() <<" ";
	}
	cout << endl;


	for (int i = 0; i < NBR_TILE_MAX; i++){

		Tile* currentSquare = playersHand[gameState->idxhand[i]];
		Tile* futurSquare = readPack->tiles[i];
		Square *boardSquare = gameState->gameBoard->get(futurSquare->getCoordinates());

		// checking if tile actualy in hand
		if (currentSquare->getType() != futurSquare->getType()){
			cout << "S: Error, the square is not the same" << endl;
			cout << "S: players hand : " << currentSquare->getType() << endl;
			cout << "S: played tile : " << futurSquare->getType() << endl;
			sendError(gameState->getCurrentPlayer(), TILE_NOT_IN_HAND, gameState);
			if (i > 0)
				gameState->gameBoard->undoStroke();
			gameState->setTakePile(false);
			return;
		}

		while (currentSquare->getTurn() != futurSquare->getTurn())
			currentSquare->rotate();

		// We check if it is a replace move
		if (boardSquare->isEmpty()){
			// this is not a replace move
			cout << "S: test to put the tile" << endl;
			if (!gameState->gameBoard->putPossible(futurSquare->getCoordinates(), currentSquare)){
				sendError(gameState->getCurrentPlayer(), IMPOSSIBLE_PLAY, gameState);
				cout << "S: can't put tile " << currentSquare->getType() << " here" << endl;
				cout << "S: x : " << futurSquare->getCoordinates().x << "y : " << futurSquare->getCoordinates().y << endl;
				if (i > 0)
					gameState->gameBoard->printConsole();
				gameState->gameBoard->undoStroke();
				gameState->setTakePile(false);
				return;
			} else {
				// use new "put" function
				currentSquare->setCoordinates(futurSquare->getCoordinates());
				cout << "-+-+-+-+-+-+-+-+-+current Square "<< i << " : " << currentSquare << endl;
				cout << currentSquare->getPlayer() << endl;
				gameState->gameBoard->put(currentSquare);
				//*boardSquare = *currentSquare;
				//currentSquare->setType(Empty);

			}
		} else {
			// this is a replace move, we check if you can put the card here
			cout << "S: test to change the tile" << endl;
			if (!gameState->gameBoard->changePossible((Tile*)boardSquare, currentSquare)){
				sendError(gameState->getCurrentPlayer(), IMPOSSIBLE_PLAY, gameState);
				cout << "S: can't put tile " << *currentSquare << " here" << endl;
				if (i > 0)
					gameState->gameBoard->undoStroke();
				gameState->setTakePile(false);
				return;
			} else {
				// use new "change" function
				currentSquare->setCoordinates(futurSquare->getCoordinates());
				gameState->gameBoard->change((Tile*)boardSquare,currentSquare);
			}
		}

	}

	cout << "xxxxxxxxxxxxxxxx next Player3 : " << (gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer() << endl;
	cout << "hand :     ";
	for(int i = 0; i< HAND_SIZE; i++){
		cout << "add : " <<gameState->getPlayer((gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer())->getHand(i) << " "  << gameState->getPlayer((gameState->getCurrentPlayer() + 1)% gameState->getNbrPlayer())->getHand(i)->getType() <<" ";
	}
	cout << endl;

	cout << "xxxxxxxxxxxxxxxx current Player : " << gameState->getCurrentPlayer() << endl;
	cout << "hand :     ";
	for(int i = 0; i< HAND_SIZE; i++){
		cout << "add : " <<gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i) << " "  << gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i)->getType() <<" ";
	}
	cout << endl;

	cout << "S: All verification is OK" << endl;

	// if we are here, both tiles were validated

	// creation of a responce pack
	vector<int> idxT;
	idxT.push_back(readPack->idxHand[0]);
	idxT.push_back(readPack->idxHand[1]);

	// send to all
	for (unsigned int i = 0; i<gameState->getCircularQueueClient().size(); i++){
		//PlayedTile *playedTile = new PlayedTile(tilePlay,idxT);
		PlayedTile *playedTile = new PlayedTile(readPack->tiles, readPack->idxHand);
		gameState->getCircularQueueClient()[i]->produce(playedTile);

	}
	// if the travel started, we wait for a new pack from the player, PILEWHENTRAVEL pack
	if (gameState->getTravelStarted()){
		gameState->setPileWhenTravel(true);
	}
	gameState->setTakePile(true);
}

// handling of a PILEWHENTRAVEL pack
void pilewhentravel(PileWhenTravel *readPack, GameState *gameState){

	// the player sent the tiles he wanted to take from the hand of a player who started his travel
	// we first check if it is the right player and if it is the time to play
	if (!gameState->getPileWhenTravel())
		sendError(gameState->getCurrentPlayer(), TIME_FOR_REGULAR_PILE, gameState);
	// if the player doesn't want to take a card form the other players, it is a regularPile
	else if (readPack->idHands.size() > 2)
		sendError(gameState->getCurrentPlayer(), TOO_MANY_TILES, gameState);
	else {
		vector<int> idHandCurrentP;
		vector<Tile*> tilePile;
		PlayerServer* currentP = gameState->getPlayer(gameState->getCurrentPlayer());
		tilePile.clear();
		idHandCurrentP.clear();

		for (int i = 0; i < NBR_TILE_MAX; i++) {
			if (currentP->getHand(i)->getType() == Empty)
				idHandCurrentP.push_back(i);
		}
		if (idHandCurrentP.size() < readPack->idHands.size())	
			sendError(gameState->getCurrentPlayer(), TOO_MANY_TILES, gameState);
		else {	    

			for (int i = 0; i < idHandCurrentP.size(); i++) {
				int idPlayer = readPack->idPlayers.back();
				int idxHand = readPack->idHands.back();
				PlayerServer* playerPile = gameState->getPlayer(readPack->idPlayers[i]);
				currentP->setHand(playerPile->getHand(idxHand), idHandCurrentP[i]);
				readPack->idHands.pop_back();
				readPack->idPlayers.pop_back();
			}
			// we take what is left from the pile 
			while( tilePile.size() < 2 && !gameState->getPileTile()->isEmpty()){
				currentP->setHand(gameState->getPileTile()->take(), idHandCurrentP[tilePile.size()]);
				currentP->getHand(idHandCurrentP[tilePile.size()])->setPlayer(currentP->getMyIdPlayer());
				tilePile.push_back(currentP->getHand(idHandCurrentP[tilePile.size()]));
				tilePile.back()->setPlayer(gameState->getCurrentPlayer());
			}


			/*
			   PilePlayer pilePlayer = PilePlayer(gameState->getCurrentPlayer(), (gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer(), tilePile, idHandCurrentP);
			// we change the next player
			gameState->setCurrentPlayer((gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer());

			for (int i = 0; i < gameState->getNbrPlayer(); i++){
			gameState->getPlayer(i)->circularQueue->produce(&pilePlayer);
			}
			 */

			int currentIdPlayer = gameState->getCurrentPlayer();
			gameState->setCurrentPlayer((gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer());

			for (unsigned int i = 0; i<gameState->getCircularQueueClient().size(); i++){
				PilePlayer *pilePlayer = new PilePlayer(currentIdPlayer, gameState->getCurrentPlayer(), tilePile, idHandCurrentP);
				gameState->getCircularQueueClient()[i]->produce(pilePlayer);
			}
		}
	}
}

void regularPile(GameState* gameState){
	vector<Tile*> tilePile;
	vector<int> idxT;
	cout << "S: regularPile in" << endl;
	idxT.clear();
	int currentIdPlayer = gameState->getCurrentPlayer();  // kill-me
	cout << "xxxxxxxxxxxxxxxx Player who takes the card : " << gameState->getCurrentPlayer() << endl;
	cout << "old hand :     ";
	for(int i = 0; i< HAND_SIZE; i++){
		cout << "add : " <<gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i) << " "  << gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i)->getType() <<" ";
	}
	cout << endl;


/*
	for (int i = 0; i<HAND_SIZE; i++){
		if (gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i)->isEmpty()){

			gameState->getPlayer(gameState->getCurrentPlayer())->setHand(gameState->getPileTile()->take(),i);
			tilePile.push_back(gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i));
			tilePile.back()->setPlayer(gameState->getCurrentPlayer());
			idxT.push_back(i);
			cout << "S: Take card at " << i << endl;
			//gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i)->print();
		}

	}
	*/

	for (int i = 0; i<NBR_TILE_MAX; i++){
		idxT.push_back(gameState->idxhand[i]);
		if (gameState->getPlayer(gameState->getCurrentPlayer())->getHand(gameState->idxhand[i])->isEmpty()){

			gameState->getPlayer(gameState->getCurrentPlayer())->setHand(gameState->getPileTile()->take(), gameState->idxhand[i]);
			cout << "S: Take card at " << gameState->idxhand[i] << endl;
			//gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i)->print();
		}
		tilePile.push_back(gameState->getPlayer(gameState->getCurrentPlayer())->getHand(gameState->idxhand[i]));

	}

	cout << "here 0" << endl;
	gameState->setCurrentPlayer((gameState->getCurrentPlayer()+1) % gameState->getNbrPlayer());
	cout << "here 1" << endl;
	for (unsigned int i = 0; i<gameState->getCircularQueueClient().size(); i++){
		cout << i << endl;
		PilePlayer *pilePlayer = new PilePlayer(currentIdPlayer, gameState->getCurrentPlayer(), tilePile, idxT);
		gameState->getCircularQueueClient()[i]->produce(pilePlayer);
	}

	cout << "here 2" << endl;
}


int main(int argc, char **argv){



	GameState *gameState;
	int cardsInHand[2];

	//    while(!start){
	// must fill players with a vector of PlayerServer containing the list of Player + a ProdCons associated to him

	// wait for connexions, the first in is the host then new players for online game, else the gui for local games with all human players then the computers connect one by one
	// when the host (online game) or the gui (local game) sends the message to start, set start to true and this is the end of the initialization.
	//    Connexion connexion = Connexion();

	//    gameState = GameState(connexion);


	gameState = new GameState();
	gameState->initThread();
	gameState->initialization();

	///////////////////////////////
	// Game initialisation
	///////////////////////////////

	gameState->gameInit();

	///////////////////////////////
	// here starts the referee
	///////////////////////////////

	int readPlayer;

	while(!gameState->getWon()){

		cout << "xxxxxxxxxxxxxxxx Player who plays : " << gameState->getCurrentPlayer() << endl;
		cout << "his hand :     ";
		for(int i = 0; i< HAND_SIZE; i++){
			cout << "add : " <<gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i) << " "  << gameState->getPlayer(gameState->getCurrentPlayer())->getHand(i)->getType() <<" ";
		}
		cout << endl;


		Pack* readPack = gameState->prodConsCommon->consume();//getPlayer(gameState->getCurrentPlayer())->circularQueue->consume();
		if (!gameState->getPileWhenTravel()){
			// if the pack was sent by the current player we call the appropriate function to validate or not the move, else we do nothing and wait for the write player to communicate.
			switch (readPack->idPack) {
				case STARTTRAVEL :
					travelStarted((StartTravel*)&readPack, gameState);
					break;
				case PLAYTRAVEL :
					travelPlayed((PlayTravel*)&readPack, gameState);
					break;
				case STOPTRAVEL :
					travelStopped((StopTravel*)&readPack, gameState);
					break;
				case PLAYTILE :
					tilePlayed((PlayTile*)readPack, gameState);
					cout << "PileWhenTravel : " << gameState->getPileWhenTravel() << endl;
					cout << "takePile : " << gameState->getTakePile() << endl;
					if (!gameState->getPileWhenTravel() && gameState->getTakePile())
						regularPile(gameState);
					cout << "S: new player hand ------------------------";
					for (int i = 0; i < 5; i ++)
						cout << gameState->getPlayer(gameState->getCurrentPlayer())->getHand()[i]->getType() << " ";
					cout << endl;
					break;
				case IWANTPLAY:
					gameState->getCircularQueueClient().back()->produce(new Validation(GAME_FULL));
					gameState->getCircularQueueClient().back()->produce(new Quit());
					break;

				case QUIT:
					{
						cout << "S:  ---------------------- I WILL QUIT THE SOCKET " << endl;
						for (unsigned int i = 0; i<gameState->getCircularQueueClient().size(); i++){
							Quit *q = new Quit();
							cout << "S: Envoi quit aux thread" << endl;
							gameState->getCircularQueueClient()[i]->produce(q);

						}
						// for (unsigned int i = 0; i<circularQueueClient.size(); i++){
						//     pthread_join(client[i], NULL);
						// }

						// for (int i = 0; i<PULLPLAYER; i++){
						//     pthread_cancel(client[i]);
						//     pthread_join(client[i], NULL);
						// } 
						close(gameState->connexion->sockfd);
						sleep(3);
						exit(0);
						break;
					}
				default :   //error, we do nothing
					break;
			}
		} else {
			switch (readPack->idPack) {
				case PILEWHENTRAVEL :
					gameState->setCurrentPlayer(gameState->getCurrentPlayer()+1);
					pilewhentravel((PileWhenTravel*)&readPack, gameState);
					gameState->setPileWhenTravel(false);
					break;
				default :   //error, we do nothing
					break;
			}
		}
		close(gameState->connexion->sockfd);
	}

	for (int i = 0; i<PULLPLAYER; i++)
		pthread_join(gameState->client[i], NULL);

	delete gameState->prodConsCommon;
	for (int i=0; i<PULLPLAYER; i++)
		delete gameState->prodConsOutputClient[i];

	return 0;
}
