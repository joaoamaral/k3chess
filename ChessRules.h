#ifndef __ChessRules_h
#define __ChessRules_h

#include "ChessPosition.h"
#include "ChessMoveMap.h"
#include "ChessMove.h"
#include "Singletons.h"


typedef unsigned ChessMoveType;
const ChessMoveType moveCapture = 1;
const ChessMoveType moveShortCastling = 2;
const ChessMoveType moveLongCastling = 4;
const ChessMoveType movePromotion = 8;


class ChessRules
{
   friend void Singletons::initialize();
   friend void Singletons::finalize();

   ChessRules();
   ~ChessRules();

public:
   // fills the move map of possible moves for the given position
   void findPossibleMoves(const ChessPosition& position, ChessMoveMap& moves) const;

   // checks if the given move is a promotion move for the current side
   bool isPromotionMove(const ChessPosition& position, const CoordPair& move) const;

   // applies move from the list of previously returned possible moves
   ChessMoveType applyMove(ChessPosition& position, const ChessMove& move) const;

   // verifies if the current player's king is checked
   bool isKingChecked(PieceColor color, const ChessPosition& position) const;

   // finds the given piece (KQRBN) attacking the given cell (if not found returns empty coords)
   ChessCoord findAttacker(ChessPiece piece, const ChessPosition &position, ChessCoord coord) const;


private:
   // you can add any necessary initialization here
};

#endif
