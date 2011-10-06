#ifndef __ChessMove_h
#define __ChessMove_h

#include "ChessPiece.h"
#include "ChessCoord.h"
#include <string>

struct ChessMove : public CoordPair
{
   PieceType promotion;
   //
   ChessMove() : promotion(ptNone) {}
   ChessMove(const CoordPair& pair, PieceType pt = ptNone) :
         CoordPair(pair), promotion(pt) {}
   //
   bool assigned() const { return !(from==ChessCoord()||to==ChessCoord()); }
   //
   std::string toString() const; // to coordinate algebraic string
   static ChessMove fromString(const std::string& str); // from coordinate algebraic string
};

#endif
