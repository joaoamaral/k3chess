#ifndef __ChessMoveMap_h
#define __ChessMoveMap_h

#include "ChessCoord.h"
#include <map>
#include <list>

class ChessMoveMap
{
public:
   class const_iterator
   {
      friend class ChessMoveMap;
      const_iterator(const std::multimap<unsigned, CoordPair>::const_iterator& it) : it_(it) {}
   public:
      const CoordPair& operator*() const { return it_->second; }
      const CoordPair *operator->() const { return &(it_->second); }
      bool operator==(const_iterator it) const { return it_ == it.it_; }
      bool operator!=(const_iterator it) const { return it_ != it.it_; }
      const const_iterator& operator++() { ++it_; return *this; }
   private:
      std::multimap<unsigned, CoordPair>::const_iterator it_;
   };

   typedef std::pair<const_iterator, const_iterator> const_iterator_pair;

   bool empty() const
   {
      return moves_.empty();
   }

   void clear()
   {
      moves_.clear();
   }

   bool contains(const CoordPair& move) const
   {
      const_iterator_pair range = getMovesFrom(move.from);
      for(;range.first!=range.second;++range.first)
      {
         if(range.first->to == move.to) return true;
      }
      return false;
   }

   bool getUniqueMove(ChessCoord from, CoordPair& move) const
   {
      // returns unique move for the 'from' coord (if such move exists)
      // otherwise returns false
      const_iterator_pair range = getMovesFrom(from);
      if(range.first==range.second) return false;
      const_iterator it = range.first;
      ++it;
      if(it==range.second)
      {
         move = *range.first;
         return true;
      }
      return false;
   }

   void add(const CoordPair& move)
   {
      moves_.insert(std::make_pair(hashOf(move.from), move));
   }

   bool hasMovesFrom(ChessCoord coord) const
   {
      const_iterator_pair range = getMovesFrom(coord);
      return range.first!=range.second;
   }

   const_iterator_pair getMovesFrom(const ChessCoord& coord) const
   {
      std::pair<
            std::multimap<unsigned, CoordPair>::const_iterator,
            std::multimap<unsigned, CoordPair>::const_iterator> range =
               moves_.equal_range(hashOf(coord));
      return std::make_pair(const_iterator(range.first), const_iterator(range.second));
   }

   const_iterator_pair getAllMoves() const
   {
      return std::make_pair(const_iterator(moves_.begin()),
                            const_iterator(moves_.end()));
   }

   void getMovesTo(ChessCoord coord, std::list<CoordPair>& moves) const
   {
      moves.clear();
      //
      const_iterator_pair range = getAllMoves();
      for(;range.first!=range.second;++range.first)
      {
         const CoordPair& cpair = *range.first;
         if(cpair.to==coord)
         {
            moves.push_back(cpair);
         }
      }
   }

private:
   unsigned hashOf(ChessCoord coord) const
   {
      return (unsigned(coord.col) << 8) | unsigned(coord.row);
   }

   std::multimap<unsigned, CoordPair> moves_;
};

#endif
