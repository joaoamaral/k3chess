#include "ChessPiece.h"
#include <assert.h>

char ChessPiece::toChar() const
{
   switch(value)
   {
      case ptNone  | pcWhite:  return '.';
      case ptKing  | pcWhite:  return 'K';
      case ptQueen | pcWhite:  return 'Q';
      case ptRook  | pcWhite:  return 'R';
      case ptBishop| pcWhite:  return 'B';
      case ptKnight| pcWhite:  return 'N';
      case ptPawn  | pcWhite:  return 'P';
      case ptNone  | pcBlack:  return '.';
      case ptKing  | pcBlack:  return 'k';
      case ptQueen | pcBlack:  return 'q';
      case ptRook  | pcBlack:  return 'r';
      case ptBishop| pcBlack:  return 'b';
      case ptKnight| pcBlack:  return 'n';
      case ptPawn  | pcBlack:  return 'p';
   default:
      assert(false); // illegal chess piece value
      return '?';
   }
}

ChessPiece ChessPiece::fromChar(char c)
{
   switch(c)
   {
      case 'K': return ChessPiece(ptKing  | pcWhite);
      case 'Q': return ChessPiece(ptQueen | pcWhite);
      case 'R': return ChessPiece(ptRook  | pcWhite);
      case 'B': return ChessPiece(ptBishop| pcWhite);
      case 'N': return ChessPiece(ptKnight| pcWhite);
      case 'P': return ChessPiece(ptPawn  | pcWhite);
      case 'k': return ChessPiece(ptKing  | pcBlack);
      case 'q': return ChessPiece(ptQueen | pcBlack);
      case 'r': return ChessPiece(ptRook  | pcBlack);
      case 'b': return ChessPiece(ptBishop| pcBlack);
      case 'n': return ChessPiece(ptKnight| pcBlack);
      case 'p': return ChessPiece(ptPawn  | pcBlack);
   default:
      return ChessPiece(ptNone | pcWhite);
   }
}
