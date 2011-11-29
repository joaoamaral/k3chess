#include "PieceCoordMap.h"

PieceCoordMap::PieceCoordMap() : itMove_(possibleMoves_.end())
{
}

void PieceCoordMap::loadFrom(const ChessPosition& position, const ChessMoveMap& possibleMoves)
{
   possibleMoves_ = possibleMoves;
   loadPiecePositions(position);
}

void PieceCoordMap::loadPiecePositions(const ChessPosition& position)
{
   piecesOnRow_.clear();
   piecesOnCol_.clear();
   //
   piecesOnRow_.resize(position.maxRow()+1);  // item[0] is not used
   piecesOnCol_.resize(position.maxCol()+1);  // item[0] is not used
   //
   PieceCoord pc;
   for(pc.col=1;pc.col<=position.maxCol();++pc.col)
   {
      for(pc.row=1;pc.row<=position.maxRow();++pc.row)
      {
         ChessPiece piece = position.cell(coord);
         if(piece.type()==ptNone || piece.color()!=position.sideToMove()) continue;
         pc.value = piece.value;
         piecesOnRow_[pc.row].push_back(pc);
         piecesOnCol_[pc.col].push_back(pc);
      }
   }
}

PieceCoord getNextPieceByRow(int row);
PieceCoord getNextPieceByCol(int col);
PieceCoord getNextPieceByType(PieceType pt);
