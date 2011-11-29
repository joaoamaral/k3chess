#ifndef __GameProfile_h
#define __GameProfile_h

#include "ChessClock.h"

enum GameType { gameComputerBlack, gameComputerWhite, gameTwoPlayers, gameTwoEngines };

struct GameProfile
{
   GameType type;
   ChessClock whiteClock;
   ChessClock blackClock;
   //
   GameProfile() : type(gameTwoPlayers) {}
   GameProfile(GameType t, const ChessClock& w, const ChessClock& b) : type(t),
      whiteClock(w), blackClock(b) {}
};

#endif
