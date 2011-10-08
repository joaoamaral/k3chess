#ifndef __ChessPosition_h
#define __ChessPosition_h

#include "ChessPiece.h"
#include "ChessCoord.h"

#include <vector>
#include <string>
#include <assert.h>

typedef std::vector<ChessPiece> CellRow;

// chess position according to FEN
class ChessPosition
{
public:
   ChessPosition();
   //
   ColValue maxCol() const;
   RowValue maxRow() const;
   //
   bool isEmpty() const;
   //
   bool inRange(ChessCoord c) const;
   //
   ChessPiece cell(ChessCoord c) const;
   void setCell(ChessCoord c, ChessPiece piece);
   //
   ColValue leftRookInitialCol() const;  // for chess 960
   ColValue rightRookInitialCol() const;
   //
   std::string toString() const; // returns FEN string for this position
   static ChessPosition fromString(const std::string& s); // loads position from FEN string
   static ChessPosition new960Position(); // creates a new random chess960 position
   //
   PieceColor sideToMove() const;  // piece color of next move
   ChessCoord pawnJump() const;   // target square of last pawn double move if last move was a pawn double move
   unsigned halfCount() const;     // counter of half moves since last capture or pawn move
   unsigned moveNumber() const;    // next move ordinal number, starting from 1
   //
   void increaseHalfCount();
   void resetHalfCount();
   void increaseMoveNumber();
   void setSideToMove(PieceColor color);
   //
   bool canShortCastle() const;  // for the current side to move
   bool canLongCastle() const;
   bool canCastle() const;       // at least in one direction
   //
   void prohibitShortCastling(); // for the current side to move
   void prohibitLongCastling();
   void prohibitCastling();
   //
   ChessCoord initialLeftRookCoord() const; // for the current side to move
   ChessCoord initialRightRookCoord() const;
   ChessCoord initialKingCoord() const;
   //
   bool isChess960() const;
   void setChess960(bool value); // force 960 value (true or false)
   //
   void setPawnJump(ChessCoord coord);
   //
private:
   unsigned toIndex(ChessCoord c) const;
   void adjustCastlingPossibility(ChessCoord c);
private:
   ColValue maxCol_;
   RowValue maxRow_;
   //
   PieceColor sideToMove_;
   unsigned castling_;
   ChessCoord pawnJump_;
   unsigned halfCount_;
   unsigned moveNumber_;
   //
   ColValue initialKingCol_;     // for chess 960 castling
   ColValue initialLeftRookCol_;
   ColValue initialRightRookCol_;
   //
   bool isChess960_;
   //
   std::vector<ChessPiece> cells_;
};

extern const std::string cStandardInitialFen;
extern const ChessPosition cStandardInitialPosition;

#endif
