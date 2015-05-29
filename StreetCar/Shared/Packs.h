#ifndef PACKS_H
#define PACKS_H


// enum of the differet types of packs in the protocol

enum packs {
    // CLIENT TO SERVER PACKS
    STARTTRAVEL = 0, PLAYTRAVEL, STOPTRAVEL, PLAYTILE, PILEWHENTRAVEL, IWANTPLAY, STARTGAME, CREATEGAME,DEBUG,QUIT,
    // SERVER TO CLIENT PACKS
    INITGAME, PLAYEDTILE, PLAYEDTRAVEL, STARTEDTRAVEL, STOPPEDTRAVEL, VALIDATION, WON, PILEPLAYER, NEWPLAYERADD,YOURIDPLAYER,
    // ONLY SERVER
    CIRCULARQUEUECLIENT
};

enum error_pack {
	IMPOSSIBLE_PLAY = 0, TOO_MANY_TILES, WRONG_WAY, TILE_NOT_IN_HAND, DISCONNECTED, GAMEFULL
};

#endif
