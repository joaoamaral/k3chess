#include "ChessPosition.h"
#include "StringUtils.h"

// must be in ChessRules, but for convenience is placed here
const std::string cStandardInitialFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const ChessPosition cStandardInitialPosition = ChessPosition::fromString(cStandardInitialFen);

namespace
{
const unsigned cWhiteCanShortCastle = 1;
const unsigned cWhiteCanLongCastle = 2;
const unsigned cBlackCanShortCastle = 4;
const unsigned cBlackCanLongCastle = 8;
}

ChessPosition::ChessPosition() :
   maxCol_(8), maxRow_(8), sideToMove_(pcWhite),
   castling_(0), halfCount_(0), moveNumber_(0),
   leftRookInitialCol_(0), rightRookInitialCol_(0)
{
   cells_.resize(maxCol_*maxRow_);
   touched_.resize(cells_.size());
}

ColValue ChessPosition::maxCol() const
{
   return maxCol_;
}

RowValue ChessPosition::maxRow() const
{
   return maxRow_;
}

bool ChessPosition::isEmpty() const
{
   return cells_.empty();
}

bool ChessPosition::inRange(ChessCoord c) const
{
   return c.col >= 1 && c.col <= maxCol_ &&
          c.row >= 1 && c.row <= maxRow_;
}

unsigned ChessPosition::toIndex(ChessCoord c) const
{
   assert(inRange(c));
   return (c.row-1) * maxCol_ + (c.col-1);
}

ChessPiece ChessPosition::cell(ChessCoord c) const
{
   return cells_[toIndex(c)];
}

bool ChessPosition::touched(ChessCoord c) const
{
   return touched_[toIndex(c)];
}

ColValue ChessPosition::leftRookInitialCol() const
{
   return leftRookInitialCol_;
}

ColValue ChessPosition::rightRookInitialCol() const
{
   return rightRookInitialCol_;
}

PieceColor ChessPosition::sideToMove() const
{
   return sideToMove_;
}

bool ChessPosition::canCastle(PieceColor color) const
{
   switch(color)
   {
      case pcWhite:  return (castling_ & (cWhiteCanShortCastle|cWhiteCanLongCastle))!=0; break;
      case pcBlack:  return (castling_ & (cBlackCanShortCastle|cBlackCanLongCastle))!=0; break;
   }
   return false;
}

bool ChessPosition::canShortCastle(PieceColor color) const
{
   switch(color)
   {
      case pcWhite:  return (castling_ & cWhiteCanShortCastle)!=0; break;
      case pcBlack:  return (castling_ & cBlackCanShortCastle)!=0; break;
   }
   return false;
}

bool ChessPosition::canLongCastle(PieceColor color) const
{
   switch(color)
   {
      case pcWhite:  return (castling_ & cWhiteCanLongCastle)!=0; break;
      case pcBlack:  return (castling_ & cBlackCanLongCastle)!=0; break;
   }
   return false;
}

ChessCoord ChessPosition::pawnJump() const
{
   return pawnJump_;
}

unsigned ChessPosition::halfCount() const
{
   return halfCount_;
}

unsigned ChessPosition::moveNumber() const
{
   return moveNumber_;
}

unsigned countFenCols(const std::string& s, unsigned pos)
{
   unsigned count = 0;
   //
   while(pos<s.length())
   {
      char c = s[pos];
      if(c>='1' && c<='9')
      {
         count += c-'0';
      }
      else if(c=='/' || c==' ' || c=='\t')
      {
         break;
      }
      else
      {
         ChessPiece piece = ChessPiece::fromChar(c);
         if(piece.type()!=ptNone)
         {
            ++count;
         }
         else
         {
            return 0;
         }
      }
      ++pos;
   }
   //
   return count;
}

bool fillFenRow(std::vector<ChessPiece>& pieces, unsigned i, const std::string& s, unsigned pos, unsigned ncols)
{
   unsigned count = 0;
   //
   while(pos<s.length() && count < ncols)
   {
      char c = s[pos];
      if(c>='1' && c<='9')
      {
         int k = c-'0';
         count += k;
         while(k--)
            pieces[i++] = ptNone;
      }
      else if(c=='/' || c==' ' || c=='\t')
      {
         return false;
      }
      else
      {
         ChessPiece piece = ChessPiece::fromChar(c);
         if(piece.type()!=ptNone)
         {
            if(count>=ncols) return false;
            pieces[i] = piece;
            ++i;
            ++count;
         }
         else
         {
            return false;
         }
      }
      ++pos;
   }
   //
   return true;
}

std::string ChessPosition::toString() const
{
   std::string fen;
   fen.reserve(128);
   //
   int emptyCellCount = 0;
   //
   ChessCoord coord;
   for(coord.row=maxRow();coord.row>=1;--coord.row)
   {
      if(coord.row<maxRow())
      {
         fen.push_back('/');
      }
      //
      for(coord.col=1;coord.col<=maxCol();++coord.col)
      {
         ChessPiece piece = cell(coord);
         if(piece.type()==ptNone)
         {
            ++emptyCellCount;
         }
         else
         {
            if(emptyCellCount)
            {
               fen.push_back('0'+emptyCellCount);
               emptyCellCount = 0;
            }
            fen.push_back(piece.toChar());
         }
      }
      //
      if(emptyCellCount)
      {
         fen.push_back('0'+emptyCellCount);
         emptyCellCount = 0;
      }
   }
   //
   fen.push_back(' ');
   fen.push_back(sideToMove_==pcWhite ? 'w' : 'b');
   //
   fen.push_back(' ');
   if(castling_ & cWhiteCanShortCastle) fen.push_back('K');
   if(castling_ & cWhiteCanLongCastle)  fen.push_back('Q');
   if(castling_ & cBlackCanShortCastle) fen.push_back('k');
   if(castling_ & cBlackCanLongCastle)  fen.push_back('q');
   //
   if(castling_==0)
   {
      fen.push_back('-');
   }
   //
   fen.push_back(' ');
   if(pawnJump_!=ChessCoord())
   {
      if(sideToMove_==pcWhite)
         fen.append(ChessCoord(pawnJump_.col, pawnJump_.row+1).toString());
      else
         fen.append(ChessCoord(pawnJump_.col, pawnJump_.row-1).toString());
   }
   else
   {
      fen.push_back('-');
   }
   //
   fen.push_back(' ');
   fen.append(uintToStr(halfCount_));
   //
   fen.push_back(' ');
   fen.append(uintToStr(moveNumber_));
   //
   return std::string(fen); // strip extra reserved characters
}

ChessPosition ChessPosition::fromString(const std::string& s)
{
   // ChessPosition() is returned if parse fails
   //
   // count number of rows and prepare rows_ vector
   ChessPosition position;
   //
   position.maxRow_ = countChars(s, '/')+1;
   //
   unsigned pos = 0;
   skipSpace(s, pos);
   //
   position.maxCol_ = countFenCols(s, pos);
   //
   if(position.maxCol_==0) return ChessPosition();
   //
   position.cells_.resize(position.maxRow_*position.maxCol_);
   position.touched_.resize(position.cells_.size(), false);
   // scan FEN rows one by one (starting from top row)
   for(ColValue i=position.maxRow_-1; i>=0; --i)
   {
      unsigned pos2 = s.find_first_of(" \t/", pos);
      if(pos2==std::string::npos) return ChessPosition();
      //
      if(!fillFenRow(position.cells_, i*position.maxCol_, s, pos, position.maxCol_)) return ChessPosition();
      //
      pos = pos2+1;
   }
   //
   skipSpace(s, pos);
   //
   if(pos>=s.length()) return ChessPosition();
   //
   switch(s[pos])
   {
   case 'w':
   case 'W': position.sideToMove_ = pcWhite; break;
   case 'b':
   case 'B': position.sideToMove_ = pcBlack; break;
   default:
      return ChessPosition();
   }
   ++pos;
   //
   skipSpace(s, pos);
   //
   if(pos>=s.length()) return ChessPosition();
   //
   position.castling_ = 0;
   //
   while(pos<s.length())
   {
      bool break_loop = false;
      switch(s[pos])
      {
         case 'K':  position.castling_ |= cWhiteCanShortCastle; break;
         case 'Q':  position.castling_ |= cWhiteCanLongCastle; break;
         case 'k':  position.castling_ |= cBlackCanShortCastle; break;
         case 'q':  position.castling_ |= cBlackCanLongCastle; break;
         case '-':  ++pos; break_loop = true; break;
         default :  break_loop = true; break;
      }
      if(break_loop) break;
      ++pos;
   }
   //
   if(position.castling_ & (cWhiteCanLongCastle | cWhiteCanShortCastle |
                             cBlackCanLongCastle | cBlackCanLongCastle))
   {
      // find left and right rook initial coords (needed for chess 960)
      {
         ChessCoord coord;
         ChessPiece rook;
         if(position.canLongCastle(pcWhite)) { coord = ChessCoord(1, 1); rook = ptRook | pcWhite; }
         else if(position.canLongCastle(pcBlack)) { coord = ChessCoord(1, position.maxRow_); rook = ptRook | pcBlack; }
         if(coord!=ChessCoord())
         {
            for(; coord.col<=position.maxCol_; ++coord.col)
            {
               if(position.cell(coord)==rook)
               {
                  position.leftRookInitialCol_ = coord.col;
                  break;
               }
            }
         }
      }
      //
      {
         ChessCoord coord;
         ChessPiece rook;
         if(position.canShortCastle(pcWhite)) { coord = ChessCoord(position.maxCol_, 1); rook = ptRook | pcWhite; }
         else if(position.canShortCastle(pcBlack)) { coord = ChessCoord(position.maxCol_, position.maxRow_); rook = ptRook | pcBlack; }
         if(coord!=ChessCoord())
         {
            for(; coord.col>=1; --coord.col)
            {
               if(position.cell(coord)==rook)
               {
                  position.rightRookInitialCol_ = coord.col;
                  break;
               }
            }
         }
      }
   }
   //
   skipSpace(s, pos);
   //
   if(pos>=s.length()) return ChessPosition();
   //
   if(s[pos]=='-')
   {
      position.pawnJump_ = ChessCoord();
      ++pos;
   }
   else if(pos+1<s.length())
   {
      ChessCoord enpassantTargetSquare = ChessCoord::fromString(s.substr(pos, 2));
      if(enpassantTargetSquare==ChessCoord()) return ChessPosition();
      if(position.sideToMove_==pcWhite)
         position.pawnJump_ = ChessCoord(enpassantTargetSquare.col, enpassantTargetSquare.row-1);
      else
         position.pawnJump_ = ChessCoord(enpassantTargetSquare.col, enpassantTargetSquare.row+1);
      pos += 2;
   }
   else
   {
      return ChessPosition();
   }
   //
   skipSpace(s, pos);
   //
   if(pos>=s.length()) return ChessPosition();
   //
   unsigned pos_parsed;
   position.halfCount_ = strToUint(s, pos, pos_parsed);
   if(pos_parsed==pos) return ChessPosition();
   pos = pos_parsed;
   //
   skipSpace(s, pos);
   //
   if(pos>=s.length()) return ChessPosition();
   //
   position.moveNumber_ = strToUint(s, pos, pos_parsed);
   if(pos_parsed==pos) return ChessPosition();
   //
   pos = pos_parsed;
   //
   skipSpace(s, pos);
   //
   if(pos<s.length()) return ChessPosition(); // extra chars after fen string
   //
   return position;
}

void ChessPosition::prohibitShortCastling(PieceColor color)
{
   switch(color)
   {
      case pcWhite:  castling_ &= ~cWhiteCanShortCastle; break;
      case pcBlack:  castling_ &= ~cBlackCanShortCastle; break;
   }
}

void ChessPosition::prohibitLongCastling(PieceColor color)
{
   switch(color)
   {
      case pcWhite:  castling_ &= ~cWhiteCanLongCastle; break;
      case pcBlack:  castling_ &= ~cBlackCanLongCastle; break;
   }
}

void ChessPosition::prohibitCastling(PieceColor color)
{
   switch(color)
   {
      case pcWhite:  castling_ &= ~(cWhiteCanShortCastle|cWhiteCanLongCastle); break;
      case pcBlack:  castling_ &= ~(cBlackCanShortCastle|cBlackCanLongCastle); break;
   }
}

void ChessPosition::setCell(ChessCoord c, ChessPiece piece)
{
   unsigned idx = toIndex(c);
   ChessPiece prevPiece = cells_[idx];
   cells_[idx] = piece;
   touched_[idx] = true;
}

void ChessPosition::increaseHalfCount()
{
   ++halfCount_;
}

void ChessPosition::resetHalfCount()
{
   halfCount_ = 0;
}

void ChessPosition::increaseMoveNumber()
{
   ++moveNumber_;
}

void ChessPosition::setSideToMove(PieceColor color)
{
   sideToMove_ = color;
}

void ChessPosition::setPawnJump(ChessCoord coord)
{
   pawnJump_ = coord;
}
