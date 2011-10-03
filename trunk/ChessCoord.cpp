#include "ChessCoord.h"

// @@note: these classes support arbitrary chess variants with
//         ranks 1..9 and files a..z

char colToChar(ColValue v)
{
   return (char)v+'a'-1;
}

ColValue colFromChar(char c)
{
   if(c>='a' && c<='h')
      return (ColValue)(c-'a'+1);
   else
      return 0;
}

char rowToChar(RowValue v)
{
   return (char)v + '0';
}

RowValue rowFromChar(char c)
{
   if(c>='1' && c<='9')
      return RowValue(c-'0');
   else
      return 0;
}

std::string ChessCoord::toString() const
{
   char s[2];
   s[0] = colToChar(col);
   s[1] = rowToChar(row);
   return std::string(s, 2);
}

ChessCoord ChessCoord::fromString(const std::string& s)
{
   if(s.length()==2)
   {
      ChessCoord coord;
      coord.col = colFromChar(s[0]);
      if(coord.col==0) return ChessCoord();
      coord.row = rowFromChar(s[1]);
      if(coord.row==0) return ChessCoord();
      return coord;
   }
   else
   {
      return ChessCoord();
   }
}

std::string CoordPair::toString() const
{
   std::string s;
   s.reserve(4);
   //
   s.append(from.toString());
   s.append(to.toString());
   //
   return s;
}

CoordPair CoordPair::fromString(const std::string& str)
{
   if(str.length()==4)
   {
      CoordPair cpair;
      cpair.from = ChessCoord::fromString(str.substr(0, 2));
      if(cpair.from==ChessCoord()) return CoordPair();
      cpair.to = ChessCoord::fromString(str.substr(2));
      if(cpair.to==ChessCoord()) return CoordPair();
      return cpair;
   }
   else
   {
      return CoordPair();
   }
}
