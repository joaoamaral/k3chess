#include "ChessRules.h"

#include <list>
#include <iostream>

namespace
{

typedef int MoveType;
const MoveType WALK = 1;
const MoveType ATTACK = 2;

typedef std::list<CoordPair> MoveList;

inline
bool tryAppendMove(qint8 dx, qint8 dy, MoveType moveType, const ChessPosition& position, ChessCoord fromCoord, MoveList& moves)
{
   CoordPair move(fromCoord, ChessCoord(fromCoord.col+dx, fromCoord.row+dy));
   //
   if(!position.inRange(move.to)) return false;
   //
   ChessPiece piece = position.cell(move.to);
   if(piece.type()==ptNone)
   {
      if(moveType & WALK)
      {
         moves.push_back(move);
         return true;
      }
   }
   else if(piece.color() != position.sideToMove())
   {
      if(moveType & ATTACK)
      {
         moves.push_back(move);
         return true;
      }
   }
   return false;
}

inline
void tryAppendMovesIter(qint8 dx, qint8 dy, const ChessPosition& position, ChessCoord fromCoord, MoveList& moves)
{
   CoordPair move(fromCoord, ChessCoord(fromCoord.col+dx, fromCoord.row+dy));
   //
   while(position.inRange(move.to))
   {
      ChessPiece piece = position.cell(move.to);
      if(piece.type()==ptNone)
      {
         moves.push_back(move);
      }
      else
      {
         if(piece.color()!=position.sideToMove())
         {
            moves.push_back(move);
         }
         //
         break;
      }
      //
      move.to.col += dx;
      move.to.row += dy;
   }
   //
}

inline
int min_of_three(int a, int b, int c)
{
   return qMin(qMin(a, b), c);
}

inline
int max_of_three(int a, int b, int c)
{
   return qMax(qMax(a, b), c);
}

inline
bool oneKingOneRook(const ChessPosition& position, ChessCoord lo, ChessCoord hi)
{
   assert(lo.row == hi.row);
   assert(lo.col <= hi.col);
   //
   int nkings = 0;
   int nrooks = 0;
   //
   for(;lo.col<=hi.col; ++lo.col)
   {
      switch(position.cell(lo).type())
      {
         case ptNone:  break;
         case ptKing:  if(++nkings>1) return false; else break;
         case ptRook:  if(++nrooks>1) return false; else break;
         default:
            return false;
      }
   }
   //
   return true;
}

bool isCellAttackedBy(PieceColor attackerColor, const ChessPosition& position, ChessCoord coord); // forward

inline
bool pathSafe(const ChessPosition& position, ChessCoord lo, ChessCoord hi)
{
   assert(lo.row == hi.row);
   assert(lo.col <= hi.col);
   //
   PieceColor attackerSide = getOpponent(position.sideToMove());
   //
   for(;lo.col<=hi.col;++lo.col)
   {
      if(isCellAttackedBy(attackerSide, position, lo)) return false;
   }
   //
   return true;
}

inline
void appendCastlingMove(const ChessPosition& position, ChessCoord kingCoord, bool isShort, MoveList& moves)
{
   if((isShort && !position.canShortCastle()) ||
      (!isShort && !position.canLongCastle())) return; // rook or king has moved
   //
   ChessCoord kingTargetCoord(isShort ? position.maxCol()-1 : 3, kingCoord.row);
   //
   ChessCoord rookCoord = isShort ? position.initialRightRookCoord() : position.initialLeftRookCoord();
   //
   ChessCoord lo(min_of_three(kingCoord.col, rookCoord.col, kingTargetCoord.col), kingCoord.row);
   ChessCoord hi(max_of_three(kingCoord.col, rookCoord.col, kingTargetCoord.col), kingCoord.row);
   //
   // check that there are no pieces other than one rook and one king within (lo, hi) range
   //
   if(!oneKingOneRook(position, lo, hi)) return;
   //
   // check that the cells between kingCoord and kingTargetCoord are not under attack
   //
   if((isShort && !pathSafe(position, kingCoord, kingTargetCoord)) ||
      (!isShort && !pathSafe(position, kingTargetCoord, kingCoord))) return;
   //
   // castling possible, generate move
   //
   if(position.maxCol()!=8 || position.leftRookInitialCol()!=1 || position.rightRookInitialCol() != position.maxCol())
   {
      // chess variants castling: target move square is the rook to be moved
      moves.push_back(CoordPair(kingCoord, rookCoord));
   }
   else
   {
      // standard chess castling: target move square is the resulting king square
      moves.push_back(CoordPair(kingCoord, kingTargetCoord));
   }
}

inline
void appendCastlingMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   if(!position.canCastle()) return;
   //
   appendCastlingMove(position, coord, false, moves);
   appendCastlingMove(position, coord, true, moves);
   //
}

/* NOTE:  white and black k/q/r/b moves are added separately because
          we need a certain order of move directions for quick move selection
          using keys */

inline
void appendWhiteKingMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMove(-1,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 0,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 1,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-1,  0, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 1,  0, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-1, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 0, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 1, -1, WALK | ATTACK, position, coord, moves);
   //
   appendCastlingMoves(position, coord, moves);
}

inline
void appendWhiteQueenMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMovesIter(-1,  1, position, coord, moves);
   tryAppendMovesIter( 0,  1, position, coord, moves);
   tryAppendMovesIter( 1,  1, position, coord, moves);
   tryAppendMovesIter(-1,  0, position, coord, moves);
   tryAppendMovesIter( 1,  0, position, coord, moves);
   tryAppendMovesIter(-1, -1, position, coord, moves);
   tryAppendMovesIter( 0, -1, position, coord, moves);
   tryAppendMovesIter( 1, -1, position, coord, moves);
}

inline
void appendWhiteRookMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMovesIter( 0,  1, position, coord, moves);
   tryAppendMovesIter(-1,  0, position, coord, moves);
   tryAppendMovesIter( 1,  0, position, coord, moves);
   tryAppendMovesIter( 0, -1, position, coord, moves);
}

inline
void appendWhiteBishopMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMovesIter(-1,  1, position, coord, moves);
   tryAppendMovesIter( 1,  1, position, coord, moves);
   tryAppendMovesIter(-1, -1, position, coord, moves);
   tryAppendMovesIter( 1, -1, position, coord, moves);
}

inline
void appendWhiteKnightMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMove(-1,  2, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 1,  2, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-2,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 2,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-2, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 2, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-1, -2, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 1, -2, WALK | ATTACK, position, coord, moves);
}

inline
void appendBlackKingMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMove( 1, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 0, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-1, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 1,  0, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-1,  0, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 1,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 0,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-1,  1, WALK | ATTACK, position, coord, moves);
   //
   appendCastlingMoves(position, coord, moves);
}

inline
void appendBlackQueenMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMovesIter( 1, -1, position, coord, moves);
   tryAppendMovesIter( 0, -1, position, coord, moves);
   tryAppendMovesIter(-1, -1, position, coord, moves);
   tryAppendMovesIter( 1,  0, position, coord, moves);
   tryAppendMovesIter(-1,  0, position, coord, moves);
   tryAppendMovesIter( 1,  1, position, coord, moves);
   tryAppendMovesIter( 0,  1, position, coord, moves);
   tryAppendMovesIter(-1,  1, position, coord, moves);
}

inline
void appendBlackRookMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMovesIter( 0, -1, position, coord, moves);
   tryAppendMovesIter( 1,  0, position, coord, moves);
   tryAppendMovesIter(-1,  0, position, coord, moves);
   tryAppendMovesIter( 0,  1, position, coord, moves);
}

inline
void appendBlackBishopMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMovesIter( 1, -1, position, coord, moves);
   tryAppendMovesIter(-1, -1, position, coord, moves);
   tryAppendMovesIter( 1,  1, position, coord, moves);
   tryAppendMovesIter(-1,  1, position, coord, moves);
}

inline
void appendBlackKnightMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   tryAppendMove( 1, -2, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-1, -2, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 2, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-2, -1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 2,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-2,  1, WALK | ATTACK, position, coord, moves);
   tryAppendMove( 1,  2, WALK | ATTACK, position, coord, moves);
   tryAppendMove(-1,  2, WALK | ATTACK, position, coord, moves);
}

inline
void appendKingMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   switch(position.cell(coord).color())
   {
      case pcWhite:   appendWhiteKingMoves(position, coord, moves);  break;
      case pcBlack:   appendBlackKingMoves(position, coord, moves);  break;
   }
}

inline
void appendQueenMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   switch(position.cell(coord).color())
   {
      case pcWhite:   appendWhiteQueenMoves(position, coord, moves);  break;
      case pcBlack:   appendBlackQueenMoves(position, coord, moves);  break;
   }
}

inline
void appendRookMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   switch(position.cell(coord).color())
   {
      case pcWhite:   appendWhiteRookMoves(position, coord, moves);  break;
      case pcBlack:   appendBlackRookMoves(position, coord, moves);  break;
   }
}

inline
void appendBishopMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   switch(position.cell(coord).color())
   {
      case pcWhite:   appendWhiteBishopMoves(position, coord, moves);  break;
      case pcBlack:   appendBlackBishopMoves(position, coord, moves);  break;
   }
}

inline
void appendKnightMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   switch(position.cell(coord).color())
   {
      case pcWhite:   appendWhiteKnightMoves(position, coord, moves);  break;
      case pcBlack:   appendBlackKnightMoves(position, coord, moves);  break;
   }
}

inline
void appendWhiteEnpassantCapture(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   if(position.pawnJump().row==coord.row &&
      (position.pawnJump().col==coord.col-1 ||
       position.pawnJump().col==coord.col+1) &&
      position.cell(position.pawnJump()).color() == pcBlack)
   {
      moves.push_back(CoordPair(coord, ChessCoord(position.pawnJump().col, coord.row+1)));
   }
}

inline
void appendBlackEnpassantCapture(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   if(position.pawnJump().row==coord.row &&
      (position.pawnJump().col==coord.col-1 ||
       position.pawnJump().col==coord.col+1) &&
      position.cell(position.pawnJump()).color() == pcWhite)
   {
      moves.push_back(CoordPair(coord, ChessCoord(position.pawnJump().col, coord.row-1)));
   }
}

inline
void appendWhitePawnMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   if(tryAppendMove(0, 1, WALK, position, coord, moves))
   {
      if(coord.row==2)
         tryAppendMove(0, 2, WALK, position, coord, moves);
   }
   //
   tryAppendMove(-1, 1, ATTACK, position, coord, moves);
   tryAppendMove( 1, 1, ATTACK, position, coord, moves);
   //
   appendWhiteEnpassantCapture(position, coord, moves);
}

inline
void appendBlackPawnMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   if(tryAppendMove(0, -1, WALK, position, coord, moves))
   {
      if(coord.row==position.maxRow()-1)
         tryAppendMove(0, -2, WALK, position, coord, moves);
   }
   //
   tryAppendMove(-1, -1, ATTACK, position, coord, moves);
   tryAppendMove( 1, -1, ATTACK, position, coord, moves);
   //
   appendBlackEnpassantCapture(position, coord, moves);
}

inline
void appendPawnMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   switch(position.sideToMove())
   {
      case pcWhite: appendWhitePawnMoves(position, coord, moves); break;
      case pcBlack: appendBlackPawnMoves(position, coord, moves); break;
   }
}

inline
void appendCellMoves(const ChessPosition& position, ChessCoord coord, MoveList& moves)
{
   ChessPiece piece = position.cell(coord);
   //
   if(piece.type()==ptNone) return;
   //
   if(piece.color()!=position.sideToMove()) return;
   //
   switch(piece.type())
   {
      case ptKing:   appendKingMoves(position, coord, moves); break;
      case ptQueen:  appendQueenMoves(position, coord, moves); break;
      case ptRook:   appendRookMoves(position, coord, moves); break;
      case ptBishop: appendBishopMoves(position, coord, moves); break;
      case ptKnight: appendKnightMoves(position, coord, moves); break;
      case ptPawn:   appendPawnMoves(position, coord, moves); break;
   }
}

inline
bool probeAttack(qint8 dx, qint8 dy, ChessPiece piece, const ChessPosition& position, ChessCoord coord)
{
   ChessCoord probeCoord(coord.col+dx, coord.row+dy);
   if(position.inRange(probeCoord))
   {
      return position.cell(probeCoord)==piece;
   }
   else
   {
      return false;
   }
}

inline
bool jumpToPiece(ChessCoord& result, qint8 dx, qint8 dy, ChessPiece piece, const ChessPosition& position, ChessCoord coord)
{
   ChessCoord probeCoord(coord.col+dx, coord.row+dy);
   if(position.inRange(probeCoord))
   {
      if(position.cell(probeCoord)==piece)
      {
         result = probeCoord;
         return true;
      }
   }
   return false;
}

inline
bool slideToPiece(ChessCoord& result, qint8 dx, qint8 dy, ChessPiece piece, const ChessPosition& position, ChessCoord coord)
{
   ChessCoord probeCoord(coord.col+dx, coord.row+dy);
   //
   while(position.inRange(probeCoord))
   {
      ChessPiece probePiece = position.cell(probeCoord);
      if(probePiece.type()!=ptNone)
      {
         if(probePiece == piece)
         {
             result = probeCoord;
             return true;
         }
         else
         {
             return false;
         }
      }
      probeCoord.col += dx;
      probeCoord.row += dy;
   }
   //
   return false;
}

inline
ChessCoord scanBackKingAttack(const ChessPiece& piece, const ChessPosition& position, ChessCoord from)
{
   ChessCoord coord;
   //
   if(jumpToPiece(coord, -1, -1, piece, position, from) ||
      jumpToPiece(coord, -1,  0, piece, position, from) ||
      jumpToPiece(coord, -1,  1, piece, position, from) ||
      jumpToPiece(coord,  0, -1, piece, position, from) ||
      jumpToPiece(coord,  0,  1, piece, position, from) ||
      jumpToPiece(coord,  1, -1, piece, position, from) ||
      jumpToPiece(coord,  1,  0, piece, position, from) ||
      jumpToPiece(coord,  1,  1, piece, position, from))
      return coord;
   else
      return ChessCoord();
}

inline
ChessCoord scanBackQueenAttack(ChessPiece piece, const ChessPosition& position, ChessCoord from)
{
   ChessCoord coord;
   //
   if(slideToPiece(coord, -1, -1, piece, position, from) ||
      slideToPiece(coord, -1,  0, piece, position, from) ||
      slideToPiece(coord, -1,  1, piece, position, from) ||
      slideToPiece(coord,  0, -1, piece, position, from) ||
      slideToPiece(coord,  0,  1, piece, position, from) ||
      slideToPiece(coord,  1, -1, piece, position, from) ||
      slideToPiece(coord,  1,  0, piece, position, from) ||
      slideToPiece(coord,  1,  1, piece, position, from))
      return coord;
   else
      return ChessCoord();
}

inline
ChessCoord scanBackRookAttack(ChessPiece piece, const ChessPosition& position, ChessCoord from)
{
   ChessCoord coord;
   //
   if(slideToPiece(coord, -1,  0, piece, position, from) ||
      slideToPiece(coord,  0, -1, piece, position, from) ||
      slideToPiece(coord,  0,  1, piece, position, from) ||
      slideToPiece(coord,  1,  0, piece, position, from))
      return coord;
   else
      return ChessCoord();
}

inline
ChessCoord scanBackBishopAttack(ChessPiece piece, const ChessPosition& position, ChessCoord from)
{
   ChessCoord coord;
   //
   if(slideToPiece(coord, -1, -1, piece, position, from) ||
      slideToPiece(coord, -1,  1, piece, position, from) ||
      slideToPiece(coord,  1, -1, piece, position, from) ||
      slideToPiece(coord,  1,  1, piece, position, from))
      return coord;
   else
      return ChessCoord();
}

inline
ChessCoord scanBackKnightAttack(ChessPiece piece, const ChessPosition& position, ChessCoord from)
{
   ChessCoord coord;
   //
   if(jumpToPiece(coord, -2, -1, piece, position, from) ||
      jumpToPiece(coord, -2,  1, piece, position, from) ||
      jumpToPiece(coord, -1,  2, piece, position, from) ||
      jumpToPiece(coord,  1,  2, piece, position, from) ||
      jumpToPiece(coord,  2,  1, piece, position, from) ||
      jumpToPiece(coord,  2, -1, piece, position, from) ||
      jumpToPiece(coord,  1, -2, piece, position, from) ||
      jumpToPiece(coord, -1, -2, piece, position, from))
      return coord;
   else
      return ChessCoord();
}

inline
ChessCoord scanBackPawnAttack(ChessPiece piece, const ChessPosition& position, ChessCoord from)
{
   ChessCoord coord;
   //
   if(piece.color()==pcWhite)
   {
      if(jumpToPiece(coord, -1, -1, piece, position, from) ||
         jumpToPiece(coord,  1, -1, piece, position, from))
         return coord;
   }
   else if(piece.color()==pcWhite)
   {
      if(jumpToPiece(coord, -1,  1, piece, position, from) ||
         jumpToPiece(coord,  1,  1, piece, position, from))
         return coord;
   }
   return ChessCoord();
}

inline
bool probeAttackIter(qint8 dx, qint8 dy, ChessPiece piece1, ChessPiece piece2, const ChessPosition& position, ChessCoord coord)
{
   ChessCoord probeCoord(coord.col+dx, coord.row+dy);
   //
   while(position.inRange(probeCoord))
   {
      ChessPiece probePiece = position.cell(probeCoord);
      if(probePiece.type()!=ptNone)
      {
         return (probePiece == piece1 || probePiece == piece2);
      }
      probeCoord.col += dx;
      probeCoord.row += dy;
   }
   //
   return false;
}

inline
bool isCellAttackedByKing(PieceColor attackerColor, const ChessPosition& position, ChessCoord coord)
{
   ChessPiece attacker(ptKing | attackerColor);
   //
   return probeAttack(-1, -1, attacker, position, coord) ||
          probeAttack(-1,  0, attacker, position, coord) ||
          probeAttack(-1,  1, attacker, position, coord) ||
          probeAttack( 0, -1, attacker, position, coord) ||
          probeAttack( 0,  1, attacker, position, coord) ||
          probeAttack( 1, -1, attacker, position, coord) ||
          probeAttack( 1,  0, attacker, position, coord) ||
          probeAttack( 1,  1, attacker, position, coord);
}

inline
bool isCellAttackedBySlider(PieceColor attackerColor, const ChessPosition& position, ChessCoord coord)
{
   ChessPiece queen(ptQueen  | attackerColor);
   ChessPiece bishop(ptBishop | attackerColor);
   ChessPiece rook(ptRook | attackerColor);
   //
   return probeAttackIter(-1, -1, bishop, queen, position, coord) ||
          probeAttackIter(-1,  1, bishop, queen, position, coord) ||
          probeAttackIter( 1, -1, bishop, queen, position, coord) ||
          probeAttackIter( 1,  1, bishop, queen, position, coord) ||
          probeAttackIter(-1,  0, rook, queen, position, coord) ||
          probeAttackIter( 0, -1, rook, queen, position, coord) ||
          probeAttackIter( 0,  1, rook, queen, position, coord) ||
          probeAttackIter( 1,  0, rook, queen, position, coord);
}

inline
bool isCellAttackedByKnight(PieceColor attackerColor, const ChessPosition& position, ChessCoord coord)
{
   ChessPiece knight(ptKnight | attackerColor);
   //
   return probeAttack(-2, -1, knight, position, coord) ||
          probeAttack(-2,  1, knight, position, coord) ||
          probeAttack(-1,  2, knight, position, coord) ||
          probeAttack( 1,  2, knight, position, coord) ||
          probeAttack( 2,  1, knight, position, coord) ||
          probeAttack( 2, -1, knight, position, coord) ||
          probeAttack( 1, -2, knight, position, coord) ||
          probeAttack(-1, -2, knight, position, coord);
}

inline
bool isCellAttackedByWhitePawn(const ChessPosition& position, ChessCoord coord)
{
   ChessPiece pawn(ptPawn|pcWhite);
   ChessCoord left(coord.col-1, coord.row-1);
   //
   if(position.inRange(left) && position.cell(left)==pawn) return true;
   //
   ChessCoord right(coord.col+1, coord.row-1);
   if(position.inRange(right) && position.cell(right)==pawn) return true;
   //
   if(position.sideToMove()==pcWhite &&
      position.pawnJump()==coord)
   {
      ChessCoord left(coord.col-1, coord.row);
      if(position.inRange(left) && position.cell(left)==pawn) return true;
      //
      ChessCoord right(coord.col+1, coord.row);
      if(position.inRange(right) && position.cell(right)==pawn) return true;
   }
   //
   return false;
}

inline
bool isCellAttackedByBlackPawn(const ChessPosition& position, ChessCoord coord)
{
   ChessPiece pawn(ptPawn|pcBlack);
   ChessCoord left(coord.col-1, coord.row+1);
   //
   if(position.inRange(left) && position.cell(left)==pawn) return true;
   //
   ChessCoord right(coord.col+1, coord.row+1);
   if(position.inRange(right) && position.cell(right)==pawn) return true;
   //
   if(position.sideToMove()==pcBlack &&
      position.pawnJump()==coord)
   {
      ChessCoord left(coord.col-1, coord.row);
      if(position.inRange(left) && position.cell(left)==pawn) return true;
      //
      ChessCoord right(coord.col+1, coord.row);
      if(position.inRange(right) && position.cell(right)==pawn) return true;
   }
   //
   return false;
}

inline
bool isCellAttackedByPawn(PieceColor attackerColor, const ChessPosition& position, ChessCoord coord)
{
   switch(attackerColor)
   {
      case pcWhite:  return isCellAttackedByWhitePawn(position, coord); break;
      case pcBlack:  return isCellAttackedByBlackPawn(position, coord); break;
   }
   return false; // dummy line to suppress compiler warning
}

inline
bool isCellAttackedBy(PieceColor attackerColor, const ChessPosition& position, ChessCoord coord)
{
   return isCellAttackedBySlider(attackerColor, position, coord) ||
          isCellAttackedByKnight(attackerColor, position, coord) ||
          isCellAttackedByKing(attackerColor, position, coord) ||
          isCellAttackedByPawn(attackerColor, position, coord);
}

inline
ChessCoord findPiecePosition(ChessPiece piece, const ChessPosition& position)
{
   ChessCoord coord;
   //
   RowValue maxRow = position.maxRow();
   ColValue maxCol = position.maxCol();
   //
   for(coord.row=1; coord.row<=maxRow; ++coord.row)
   {
      for(coord.col=1; coord.col<=maxCol; ++coord.col)
      {
         if(position.cell(coord)==piece) return coord;
      }
   }
   return ChessCoord();
}

inline
void clearCellRange(ChessPosition& position, ChessCoord lo, ChessCoord hi)
{
   assert(lo.row==hi.row);
   assert(lo.col<=hi.col);
   //
   for(ChessCoord coord = lo; coord.col<=hi.col;++coord.col)
   {
      position.setCell(coord, ChessPiece());
   }
}

inline
void applyCastlingMove(ChessPosition& position, bool isShort, ChessCoord kingCoord)
{
   ChessCoord kingTargetCoord(isShort ? position.maxCol()-1 : 3, kingCoord.row);
   //
   ChessCoord rookCoord(isShort ? position.rightRookInitialCol() : position.leftRookInitialCol(), kingCoord.row);
   ChessCoord rookTargetCoord(isShort ? kingTargetCoord.col-1 : kingTargetCoord.col+1, kingTargetCoord.row);
   //
   ChessCoord lo(min_of_three(kingCoord.col, rookCoord.col, kingTargetCoord.col), kingCoord.row);
   ChessCoord hi(max_of_three(kingCoord.col, rookCoord.col, kingTargetCoord.col), kingCoord.row);
   //
   clearCellRange(position, lo, hi);
   //
   position.setCell(kingTargetCoord, ChessPiece(ptKing|position.sideToMove()));
   position.setCell(rookTargetCoord, ChessPiece(ptRook|position.sideToMove()));
   //
}

inline
ChessMoveType applyCastlingMove(ChessPosition& position, const CoordPair& move)
{
   if(position.cell(move.from) == ChessPiece(ptKing|position.sideToMove()) &&
      position.canCastle() && move.from.row == move.to.row)
   {
      if(position.maxCol()!=8 ||
            position.leftRookInitialCol()!=1 ||
            position.rightRookInitialCol() != position.maxCol())
      {
         // chess variants castling: target move square is the rook to be moved
         if(move.to.col==position.leftRookInitialCol() &&
            position.canLongCastle())
         {
            applyCastlingMove(position, false, move.from);
            position.prohibitCastling();
            return moveLongCastling;
         }
         else if(move.to.col==position.rightRookInitialCol() &&
                 position.canShortCastle())
         {
            applyCastlingMove(position, true, move.from);
            position.prohibitCastling();
            return moveShortCastling;
         }
      }
      else
      {
         // standard chess castling: target move square is target king square
         if(move.to.col==3)
         {
            applyCastlingMove(position, false, move.from);
            position.prohibitCastling();
            return moveLongCastling;
         }
         else if(move.to.col==position.maxCol()-1)
         {
            applyCastlingMove(position, true, move.from);
            position.prohibitCastling();
            return moveShortCastling;
         }
      }
   }
   //
   return 0;
}

inline
ChessMoveType applyEnpassantCapture(ChessPosition& position, const CoordPair& move)
{
   //
   // @note: only 0 or moveCapture is returned
   //
   if(position.pawnJump()==ChessCoord() ||
      position.cell(position.pawnJump()).color()==position.sideToMove())
   {
      return 0;
   }
   ChessPiece movedPiece = position.cell(move.from);
   //
   if(movedPiece.type()!=ptPawn)
   {
      return 0;
   }
   //
   if(position.pawnJump().row == move.from.row &&
      position.pawnJump().col == move.to.col)
   {
      position.setCell(position.pawnJump(), ChessPiece());
      return moveCapture;
   }
   else
   {
      return 0;
   }
}

inline
void adjustCastlingPossibility(ChessPosition& position, const CoordPair& move)
{
   if(!position.canCastle()) return;
   //
   if(move.from==position.initialKingCoord())
   {
      position.prohibitCastling();
   }
   else if(position.canLongCastle() &&
           (move.from==position.initialLeftRookCoord() ||
            move.to==position.initialLeftRookCoord()))
   {
      position.prohibitLongCastling();
   }
   else if (position.canShortCastle() &&
           (move.from==position.initialRightRookCoord() ||
            move.to==position.initialRightRookCoord()))
   {
      position.prohibitShortCastling();
   }
}

inline
MoveType applyConventionalMove(ChessPosition& position, const CoordPair& move)
{
   // @@note: only 0 or moveCapture is returned
   //
   ChessPiece movedPiece = position.cell(move.from);
   ChessPiece targetPiece = position.cell(move.to);
   //
   adjustCastlingPossibility(position, move);
   //
   if(movedPiece.type()==ptNone || movedPiece.color()!=position.sideToMove())
   {
      assert(false);
   }
   //
   position.setCell(move.from, ChessPiece());
   position.setCell(move.to, movedPiece);
   //
   return targetPiece.type()==ptNone ? 0 : moveCapture;
}

inline
void increaseMoveCounts(ChessPosition& position, const CoordPair& move)
{
   if(position.cell(move.from).type()==ptPawn || position.cell(move.to).type()!=ptNone)
   {
      position.resetHalfCount();
   }
   else
   {
      position.increaseHalfCount();
   }
   //
   if(position.sideToMove()==pcBlack)
   {
      position.increaseMoveNumber();
   }
}

bool isPawnDoubleMove(const ChessPosition& position, const CoordPair& move)
{
   switch(position.sideToMove())
   {
   case pcWhite: return (position.cell(move.from) == (ptPawn | pcWhite) &&
                         move.from.row==2 && move.to.row==4);
   case pcBlack: return (position.cell(move.from) == (ptPawn | pcBlack) &&
                         move.from.row==position.maxRow()-1 && move.to.row==position.maxRow()-3);
   }
}

}

ChessRules::ChessRules()
{
   // @@note: in this version chess rules require no initialization
}

ChessRules::~ChessRules()
{
}

void ChessRules::findPossibleMoves(const ChessPosition &position, ChessMoveMap &moves) const
{
   //
   MoveList moveCandidates;
   //
   ChessCoord coord;
   //
   RowValue maxRow = position.maxRow();
   ColValue maxCol = position.maxCol();
   //
   for(coord.row=1; coord.row<=maxRow; ++coord.row)
   {
      for(coord.col=1; coord.col<=maxCol; ++coord.col)
      {
         appendCellMoves(position, coord, moveCandidates);
      }
   }
   //
   // pick only those moves that do not result in own king being checked
   //
   moves.clear();
   //
   MoveList::const_iterator it = moveCandidates.begin(), itEnd = moveCandidates.end();
   for(;it!=itEnd;++it)
   {
      ChessPosition testPosition(position);
      applyMove(testPosition, *it);
      if(!isKingChecked(position.sideToMove(), testPosition))
      {
         moves.add(*it);
      }
   }
}

ChessMoveType ChessRules::applyMove(ChessPosition &position, const ChessMove& move) const
{
   increaseMoveCounts(position, move);
   //
   ChessMoveType moveType = 0;
   //
   moveType |= applyCastlingMove(position, move);
   //
   if(!moveType)
   {
      bool isPromotion = isPromotionMove(position, move);
      bool isPawnDouble = isPawnDoubleMove(position, move);
      //
      moveType |= applyEnpassantCapture(position, move); // only removes the captured pawn
      //
      moveType |= applyConventionalMove(position, move); // performs standard move operations
      //
      if(isPawnDouble)
      {
         position.setPawnJump(move.to);
      }
      else
      {
         position.setPawnJump(ChessCoord());
      }
      //
      if(isPromotion)
      {
         moveType |= movePromotion;
         position.setCell(move.to, ChessPiece(position.sideToMove() | move.promotion));
      }
   }
   position.setSideToMove(getOpponent(position.sideToMove()));
   //
   return moveType;
}

bool ChessRules::isKingChecked(PieceColor color, const ChessPosition& position) const
{
   ChessCoord kingCoord = findPiecePosition(ChessPiece(ptKing|color), position);
   if(kingCoord == ChessCoord())
   {
      assert(false);
      return false;
   }
   else
   {
      return isCellAttackedBy(getOpponent(color), position, kingCoord);
   }
}

bool ChessRules::isPromotionMove(const ChessPosition& position, const CoordPair& move) const
{
   if(!position.inRange(move.from) || !position.inRange(move.to))
   {
      assert(false);
      return false;
   }
   switch(position.sideToMove())
   {
      case pcWhite:  return position.cell(move.from) == (ptPawn | pcWhite) && move.to.row == position.maxRow();  break;
      case pcBlack:  return position.cell(move.from) == (ptPawn | pcBlack) && move.to.row == 1;  break;
   }
   return false; // dummy line to suppress compiler warning
}

ChessCoord ChessRules::findAttacker(ChessPiece piece, const ChessPosition &position, ChessCoord coord) const
{
   switch(piece.type())
   {
      case ptKing:   return scanBackKingAttack(piece, position, coord);   break;
      case ptQueen:  return scanBackQueenAttack(piece, position, coord);  break;
      case ptRook:   return scanBackRookAttack(piece, position, coord);   break;
      case ptBishop: return scanBackBishopAttack(piece, position, coord); break;
      case ptKnight: return scanBackKnightAttack(piece, position, coord); break;
      case ptPawn:   return scanBackPawnAttack(piece, position, coord);   break;
   }
   return ChessCoord(); // dummy line to suppress compiler warning
}
