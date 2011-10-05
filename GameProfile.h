#ifndef __GameProfile_h
#define __GameProfile_h

enum GameType { gameComputerBlack, gameComputerWhite, gameTwoPlayers, gameTwoEngines };

struct ChessClock
{
   int initialTime;      // ms
   int remainingTime;  // ms
   int moveIncrement;  // ms
   //
   ChessClock() : initialTime(0), remainingTime(0), moveIncrement(0) {}
   ChessClock(int tt, int rt, int mi) : initialTime(tt), remainingTime(rt), moveIncrement(mi) {}
   //
   bool operator==(const ChessClock& clock) const
   {
      return initialTime == clock.initialTime && remainingTime == clock.remainingTime &&
            moveIncrement == clock.moveIncrement;
   }
   bool operator!=(const ChessClock& clock) const
   {
      return !operator==(clock);
   }
};

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