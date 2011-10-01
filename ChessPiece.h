#ifndef __chesspiece_h
#define __chesspiece_h

typedef unsigned char PieceFlags;

typedef PieceFlags PieceType;
const PieceType ptNone = 0;
const PieceType ptKing = 1;
const PieceType ptQueen = 2;
const PieceType ptRook = 3;
const PieceType ptBishop = 4;
const PieceType ptKnight = 5;
const PieceType ptPawn = 6;
const PieceFlags ptMask = 7;

const PieceFlags ptFirst = ptKing; // for iterations
const PieceFlags ptLast = ptPawn;

typedef PieceFlags PieceColor;
const PieceColor pcWhite = 0;
const PieceColor pcBlack = 128;
const PieceFlags pcMask = 128;

struct ChessPiece
{
   PieceFlags value;
   //
   ChessPiece() : value(ptNone | pcWhite) {}
   ChessPiece(PieceFlags val) : value(val) {}
   //
   char toChar() const;
   static ChessPiece fromChar(char c);
   //
   bool operator==(ChessPiece piece) const
   {
      return value == piece.value;
   }
   bool operator!=(ChessPiece piece) const
   {
      return value != piece.value;
   }
   bool operator<(ChessPiece piece) const
   {
      return value < piece.value;
   }
   //
   unsigned char type() const { return value & ptMask; }
   unsigned char color() const { return value & pcMask; }
};

inline
PieceColor getOpponent(PieceColor pc)
{
   return (pc & pcBlack) ? pcWhite : pcBlack;
}

#endif
