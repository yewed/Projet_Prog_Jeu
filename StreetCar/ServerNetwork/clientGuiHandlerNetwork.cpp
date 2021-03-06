//#include "PlayedTravel.h"
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sstream>
#include "../Shared/Tile.h"
#include "../Shared/Utils.h"
#include <pthread.h>
#include "clientGuiHandlerNetwork.h"
#include "../Shared/ParamThreadClient.h"
#include "../Shared/StartTravel.h"
#include "../Shared/PlayTravel.h"
#include "../Shared/StopTravel.h"
#include "../Shared/PlayTile.h"
#include "../Shared/PileWhenTravel.h"
#include "../Shared/IWantPlay.h"
#include "../Shared/StartGame.h"
#include "../Shared/CreateGame.h"
#include "../Shared/InitGame.h"
#include "../Shared/PlayedTile.h"
#include "../Shared/PlayedTravel.h"
#include "../Shared/StartedTravel.h"
#include "../Shared/StoppedTravel.h"
#include "../Shared/Validation.h"
#include "../Shared/Won.h"
#include "../Shared/PilePlayer.h"
#include "../Shared/NewPlayerAdd.h"
#include "../Shared/Pack.h"
#include "../Shared/RefreshPlayerGame.h"
#include "../Shared/ResponsePlayerRefresh.h"
#include "../Shared/Debug.h"
#include <cerrno>
//#include "../Shared/Launch.h"
#include "../Shared/Quit.h"

using namespace std;

void *clientOutputHandlerNetwork(void* argv){

    //recover params for the thread
    ParamThread *param = (ParamThread*)argv;

    ProdCons<Pack*> *prodConsClient = param->prodConsClient;
    ProdCons<Pack*> *prodConsCommon = param->prodConsCommon;
    int newsockfd = param->sockfd;
    struct sockaddr_in serv_addr = *param->serv_addr;
    struct sockaddr_in cli_addr = *param->cli_addr;

    cout << "S: Event thread client1Handler started successful : " << pthread_self() << endl;

    socklen_t clilen;
    stringstream buffer2;
    char buffer[256];
    int n;
    bool isFinish = false;
    Pack *readPack;
    //The accept() call actually accepts an incoming connection
    clilen = sizeof(cli_addr);

    pthread_t client;
    ParamThreadInput paramInput = {prodConsCommon,newsockfd,&serv_addr,&cli_addr};

    cout << "S: sock 1 : " << newsockfd << endl;
    if (pthread_create(&client, NULL, clientInputHandlerNetwork,(void *)(&paramInput))==0){
    }else
	cout << "S: ERROR, impossible to create clientInput " << endl;


    while (!isFinish){
	cout << "readPack consume in clientOutput" << endl;
	readPack = prodConsClient->consume();
	
	stringstream ss;
	ss << *readPack;
	ss.seekg(0, ios::end);
	int size = ss.tellg(); //size contain the size (in bytes) of the string

	cout << "S: message -------------- " << ss.str() << endl;
	int g = htonl(size);
	n = write(newsockfd, (const char*)&g, sizeof(int));
	n = write(newsockfd, ss.str().c_str(), size);
	cout << "S: write on network " << endl;

	if (n < 0) 
	    cout << "S: ERROR writing from socket" << endl;

	if (readPack->idPack == QUIT){
	    cout << "S: ----------------------- I DELETE THE SOCKET " << endl;
	    delete readPack;
	    close(newsockfd);
	    pthread_cancel(client);
	    exit(0);
	}
	delete readPack;
    }

    close(newsockfd);

    return 0;
    
}

void *clientInputHandlerNetwork(void* argv){
    
    //recover params for the thread
    cout << "S: Clien input start successful : " << pthread_self() << endl;
    ParamThreadInput *param = (ParamThreadInput*)argv;
    
    ProdCons<Pack*> *prodConsCommon = param->prodConsCommon;
    int newsockfd = param->sockfd;
    struct sockaddr_in serv_addr = *param->serv_addr;
    struct sockaddr_in cli_addr = *param->cli_addr;
    
    stringstream ss;
    char buffer[MAX_PACKET_SIZE];
    int n;
    bool isFinish = false;
    
    while (!isFinish){
	Pack *pack = new Pack();
	bzero(buffer,MAX_PACKET_SIZE);
	int a ;
	cout << "SN: in client Input" << endl;
	if ((n = recv(newsockfd,(char*)&a,sizeof(int),MSG_WAITALL)) < 0){
	    cout << "Something went wrong! errno " << errno << ": ";
	    cout << strerror(errno) << endl;
	}
	a = ntohl(a);
	cout << "S: receive int a = " << a << endl;
	n = recv(newsockfd,buffer,a,MSG_WAITALL);
	if (n > 0) {
	    
	    cout << "S: reading on socket " << n << " " << buffer << endl;
	    //buffer[n] = '\0';
	    ss.str(string()); //to clear the stringstream 
	    ss.clear();
	    
	    ss << buffer;
	    
	    int i;
	    ss >> i;
	    switch((packs)i){
	    case STARTTRAVEL:
		{
		    StartTravel* tmp = new StartTravel();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case PLAYTRAVEL:
		{
		    PlayTravel* tmp = new PlayTravel();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case STOPTRAVEL:
		{
		    StopTravel* tmp = new StopTravel();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case PLAYTILE:
		{
		    PlayTile* tmp = new PlayTile();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case PILEWHENTRAVEL:
		{
		    PileWhenTravel* tmp = new PileWhenTravel();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case IWANTPLAY:
		{
		    IWantPlay* tmp = new IWantPlay();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case STARTGAME:
		{
		    StartGame* tmp = new StartGame();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case CREATEGAME:
		{
		    CreateGame* tmp = new CreateGame();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case DEBUG:
		{
		    Debug* tmp = new Debug();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case INITGAME:
		{
		    InitGame* tmp = new InitGame();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case PLAYEDTILE:
		{
		    PlayedTile* tmp = new PlayedTile();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case PLAYEDTRAVEL:
		{
		    PlayedTravel* tmp = new PlayedTravel();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case STARTEDTRAVEL:
		{
		    StartedTravel* tmp = new StartedTravel();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case STOPPEDTRAVEL:
		{
		    StoppedTravel* tmp = new StoppedTravel();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case VALIDATION:
		{
		    Validation* tmp = new Validation();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case WON:
		{
		    Won* tmp = new Won();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case PILEPLAYER:
		{
		    PilePlayer* tmp = new PilePlayer();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case NEWPLAYERADD:
		{
		    NewPlayerAdd* tmp = new NewPlayerAdd();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case QUIT:
		{
		    Quit* tmp = new Quit();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    case REFRESHPLAYERGAME:
		{
		    ResponsePlayerRefresh* tmp = new ResponsePlayerRefresh();
		    ss >> *tmp;
		    pack = tmp;
		}
		break;
	    default:
		cout << "S: deserialisable error" << endl;
		break;
	    }
	    cout << "S: this is pack : "<< *pack << endl;
	    prodConsCommon->produce(pack);
	}
	else {
	    cout << "S: ERROR reading from socket" << endl;
	    isFinish = true;
	    return 0;
	}
    }
    close(newsockfd);
    return 0;
}

/*Pack* deserialize(Pack* p, stringstream& s){
  switch(p->idPack){
  case DEBUG:
  Debug* tmp = (Debug*)p;
  s >> tmp;
  cout << "S: deserialize debug : " << tmp << endl;
  return tmp;
  break;
  default:
  cout << "S: deserialisable error" << endl;
  return NULL;
  }
  }
*/
