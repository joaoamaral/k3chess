#ifndef __ChessClock_h
#define __ChessClock_h

struct ChessClock
{
   int initialTime;      // ms
   int remainingTime;  // ms
   int moveIncrement;  // ms
   bool untimed;
   //
   ChessClock() : initialTime(0), remainingTime(0), moveIncrement(0), untimed(true) {}
   ChessClock(int tt, int rt, int mi) : initialTime(tt), remainingTime(rt), moveIncrement(mi), untimed(false) {}
   //
   bool operator==(const ChessClock& clock) const
   {
      return initialTime == clock.initialTime && remainingTime == clock.remainingTime &&
            moveIncrement == clock.moveIncrement && untimed == clock.untimed;
   }
   bool operator!=(const ChessClock& clock) const
   {
      return !operator==(clock);
   }
};

#endif
