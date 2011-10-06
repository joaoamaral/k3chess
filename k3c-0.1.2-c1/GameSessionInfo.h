#ifndef __GameSessionInfo_h
#define __GameSessionInfo_h

#include "ChessPosition.h"
#include "GameProfile.h"
#include "ChessMove.h"

struct GameSessionInfo
{
   ChessPosition initialPosition;
   GameProfile profile;
   std::vector<ChessMove> moves;
   //
   bool saveToFile(const QString& fileName) const;
   bool loadFromFile(const QString& fileName);
};

#endif
