#include "ChessMove.h"
#include "ChessPiece.h"
#include <assert.h>

std::string ChessMove::toString() const
{
   if(promotion==ptNone)
   {
      return CoordPair::toString();
   }
   else
   {
      std::string s;
      s.reserve(5);
      s.append(CoordPair::toString());
      s.push_back(ChessPiece(promotion|pcBlack).toChar());
      return s;
   }
}

ChessMove ChessMove::fromString(const std::string& str)
{
   if(str.length()<4)
   {
      return ChessMove();
   }
   PieceType promotion = ptNone;
   if(str.length()>4)
   {
      ChessPiece p = ChessPiece::fromChar(str[4]);
      switch(p.type())
      {
         case ptQueen:
         case ptRook:
         case ptBishop:
         case ptKnight:
            promotion = p.type();
            break;
      }
   }
   CoordPair cpair = CoordPair::fromString(str.substr(0, 4));
   if(cpair==CoordPair()) return ChessMove();
   return ChessMove(cpair, promotion);
}
