#ifndef __chesscoord_h
#define __chesscoord_h

#include <QtGlobal>
#include <string>

typedef qint8 ColValue; // @@note: these types must be signed
typedef qint8 RowValue;

char colToChar(ColValue v);
char rowToChar(RowValue v);
ColValue colFromChar(char c);
RowValue rowFromChar(char c);

struct ChessCoord
{
   ColValue col;
   RowValue row;
   //
   ChessCoord() : col(0), row(0) {}
   ChessCoord(ColValue c, RowValue r) : col(c), row(r) {}
   //
   std::string toString() const;
   static ChessCoord fromString(const std::string& s);
   //
   bool operator==(ChessCoord coord) const
   {
      return col == coord.col && row == coord.row;
   }
   bool operator!=(ChessCoord coord) const
   {
      return !(operator==(coord));
   }
};

struct CoordPair
{
   ChessCoord from;
   ChessCoord to;
   //
   CoordPair() {}
   CoordPair(ChessCoord f, ChessCoord t) : from(f), to(t) {}
   //
   bool operator==(const CoordPair& pair) const
   {
      return from == pair.from && to == pair.to;
   }
   //
   std::string toString() const;
   static CoordPair fromString(const std::string& str);
};

#endif
