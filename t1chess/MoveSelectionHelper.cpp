#include "MoveSelectionHelper.h"

namespace
{

inline bool advanceRow(RowValue& row, const ChessPosition& position)
{
   if(position.sideToMove()==pcWhite)
   {
      if(++row > position.maxRow())
      {
         row -= position.maxRow();
         return true;
      }
   }
   else
   {
      if(--row < 1)
      {
         row += position.maxRow();
         return true;
      }
   }
   return false;
}

inline bool advanceCol(ColValue& col, const ChessPosition& position)
{
   if(position.sideToMove()==pcWhite)
   {
      if(++col > position.maxCol())
      {
         col -= position.maxCol();
         return true;
      }
   }
   else
   {
      if(--col < 1)
      {
         col += position.maxCol();
         return true;
      }
   }
   return false;
}

inline bool advanceCoord(ChessCoord& coord, const ChessPosition& position)
{
   if(advanceCol(coord.col, position))
      return advanceRow(coord.row, position);
   else
      return false;
}

ChessMoveMap::const_iterator findMove(
   const std::pair<ChessMoveMap::const_iterator,
                   ChessMoveMap::const_iterator>& range,
   const CoordPair& cpair )
{
   if(range.first==range.second) return range.second;
   //
   for(ChessMoveMap::const_iterator it=range.first;it!=range.second;++it)
   {
      if(*it==cpair) return it;
   }
   return range.second;
}

template <class TargetCondition>
ChessMoveMap::const_iterator findMoveWithCondition(
   const std::pair<ChessMoveMap::const_iterator,
                 ChessMoveMap::const_iterator>& range,
   ChessMoveMap::const_iterator itStart,
   TargetCondition cond)
{
   if(range.first==range.second) return range.second;
   //
   if(itStart==range.second) itStart = range.first;
   ChessMoveMap::const_iterator it = itStart;
   //
   while(true)
   {
      if(cond(it->to)) return it;
      if(++it==range.second) it = range.first;
      if(it==itStart) break;
   }
   return range.second;
}

template <class TargetCondition>
unsigned countMovesWithCondition(
   const std::pair<ChessMoveMap::const_iterator,
                   ChessMoveMap::const_iterator>& range,
   TargetCondition cond)
{
   if(range.first==range.second) return 0;
   //
   unsigned count = 0;
   //
   for(ChessMoveMap::const_iterator it=range.first;it!=range.second;++it)
   {
      if(cond(it->to)) ++count;
   }
   return count;
}

class TargetColMatches
{
public:
   TargetColMatches(ColValue col) : col_(col) {}
   bool operator()(ChessCoord c) const
   {
      return c.col==col_;
   }
private:
   ColValue col_;
};

class TargetRowMatches
{
public:
   TargetRowMatches(RowValue row) : row_(row) {}
   bool operator()(ChessCoord c) const
   {
      return c.row==row_;
   }
private:
   RowValue row_;
};

class TargetPieceMatches
{
public:
   TargetPieceMatches(const ChessPosition& position, PieceType pt) :
      pos_(&position), piece_(pt|getOpponent(position.sideToMove())) {}
   bool operator()(ChessCoord c) const
   {
      assert(pos_);
      return pos_->cell(c)==piece_;
   }
private:
   const ChessPosition *pos_;
   ChessPiece piece_;
};

}

namespace MoveSelectionHelper
{

ChessCoord getNextPieceByCol(const ChessPosition& position,
                             const ChessMoveMap& moves,
                             ChessCoord coord, int col,
                             bool preferSamePieceType)
{
   assert(col>=1 && col<=position.maxCol());
   //
   ChessPiece p = preferSamePieceType ? position.cell(coord) : ChessPiece();
   //
   ChessCoord c(coord);
   //
   if(c.col==col)
      advanceRow(c.row, position);
   else
      c.col = col;
   //
   int ntrav = 0;
   while(true)
   {
      if(ntrav==position.maxRow())
      {
         if(p!=ChessPiece())
         {
            p = ChessPiece();
            ntrav = 0;
         }
         else
            break;
      }
      //
      ChessPiece p2 = position.cell(c);
      if(moves.hasMovesFrom(c) &&
         ((p!=ChessPiece() && p2==p) ||
          (p==ChessPiece() && p2.type()!=ptNone &&
           p2.color()==position.sideToMove())))
      {
         return c;
      }
      //
      advanceRow(c.row, position);
      //
      ++ntrav;
   }
   //
   return coord;
}

ChessCoord getNextPieceByType(const ChessPosition& position,
                              const ChessMoveMap& moves,
                              ChessCoord coord, PieceType pt)
{
   assert(position.inRange(coord));
   //
   if(pt==ptNone) return coord;
   //
   ChessPiece p(pt|position.sideToMove());
   //
   ChessCoord c(coord);
   //
   if(position.cell(coord)==p)
      advanceCoord(c, position);
   //
   unsigned ntrav = 0, ntravmax = position.maxRow()*position.maxCol();
   while(true)
   {
      if(ntrav==ntravmax)
      {
         if(p!=ChessPiece())
         {
            p = ChessPiece();
            ntrav = 0;
         }
         else
            break;
      }
      //
      ChessPiece p2 = position.cell(c);
      if(moves.hasMovesFrom(c) &&
         ((p!=ChessPiece() && p2==p) ||
          (p==ChessPiece() && p2.type()!=ptNone &&
           p2.color()==position.sideToMove())))
      {
         return c;
      }
      //
      advanceCoord(c, position);
      //
      ++ntrav;
   }
   //
   return coord;
}

template <class TargetCondition>
ChessCoord getNextTargetByCondition(const ChessPosition& position, const ChessMoveMap& moves,
                                    const CoordPair& cpair, TargetCondition cond,
                                    bool& isUnique)
{
   isUnique = false;
   //
   ChessMoveMap::const_iterator_pair range =
      moves.getMovesFrom(cpair.from);
   //
   if(range.first==range.second) return cpair.to; // no moves for this piece - stay on the same target cell
   //
   ChessMoveMap::const_iterator itMove = findMove(range, cpair);
   //
   if(itMove==range.second)
   {
      itMove = range.first;
   }
   else
   {
      ++itMove;
   }
   //
   itMove = findMoveWithCondition(range, itMove, cond);
   if(itMove==range.second)
   {
      return cpair.to; // no moves with target column=col
   }
   else
   {
      isUnique = countMovesWithCondition(range, cond)==1;
      return itMove->to;
   }
}

ChessCoord getNextTargetByCol(const ChessPosition& position, const ChessMoveMap& moves,
                              const CoordPair& cpair, int col, bool& isUnique)
{
   return getNextTargetByCondition(position, moves, cpair, TargetColMatches(col), isUnique);
}

ChessCoord getNextTargetByRow(const ChessPosition& position, const ChessMoveMap& moves,
                              const CoordPair& cpair, int row, bool& isUnique)
{
   return getNextTargetByCondition(position, moves, cpair, TargetRowMatches(row), isUnique);
}

ChessCoord getNextTargetByPiece(const ChessPosition& position, const ChessMoveMap& moves,
                                const CoordPair& cpair, PieceType pt, bool& isUnique)
{
   return getNextTargetByCondition(position, moves, cpair, TargetPieceMatches(position, pt), isUnique);
}

}
