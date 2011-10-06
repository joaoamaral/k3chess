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
   bool touched(ChessCoord c) const; // warning! touched since position setup (if position is not start game position)
   //
   ColValue leftRookInitialCol() const;  // for chess 960
   ColValue rightRookInitialCol() const;
   //
   std::string toString() const; // returns FEN string for this position
   static ChessPosition fromString(const std::string& s); // loads position from FEN string
   //
   PieceColor sideToMove() const;  // piece color of next move
   ChessCoord enpassant() const;   // target square of last pawn double move
   unsigned halfCount() const;     // counter of half moves since last capture or pawn move
   unsigned moveNumber() const;    // next move ordinal number, starting from 1
   //
   void increaseHalfCount();
   void resetHalfCount();
   void increaseMoveNumber();
   void setSideToMove(PieceColor color);
   //
   bool canShortCastle(PieceColor color) const;
   bool canLongCastle(PieceColor color) const;
   bool canCastle(PieceColor color) const;       // at least in one direction
   //
   void prohibitShortCastling(PieceColor color);
   void prohibitLongCastling(PieceColor color);
   void prohibitCastling(PieceColor color);
   //
   void setEnpassant(ChessCoord coord);
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
   ChessCoord enpassant_;
   unsigned halfCount_;
   unsigned moveNumber_;
   //
   ColValue leftRookInitialCol_; // for chess 960 castling
   ColValue rightRookInitialCol_;
   std::vector<ChessPiece> cells_;
   std::vector<bool> touched_; // @@todo: check how vector<bool> behaves!
                               // touched_ keeps track of cell touches since initial setup
                               // it may not correspond to actual touches if the initial setup
                               // was not for move 1
};

extern const std::string cStandardInitialFen;
extern const ChessPosition cStandardInitialPosition;

#endif
