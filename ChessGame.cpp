#include "ChessGame.h"
#include "Settings.h"

#include <fstream>

namespace
{

void logPositionAndMoves(const ChessPosition& position, const ChessMoveMap& moves)
{
   static unsigned nCalls = 0;
   static bool noLog = false;
   if(noLog) return;
   std::ofstream out("./logs/pos_moves.log", nCalls==0 ? std::ios::trunc : std::ios::app);
   noLog = out.fail();
   if(noLog) return; // will fail if there is no 'logs' folder
   out << "Position: " << position.toString() << std::endl;
   out << "Available moves:";
   //
   ChessMoveMap::const_iterator_pair range =
         moves.getAllMoves();
   //
   if(range.first==range.second)
   {
      out << " -";
   }
   else
   {
      for(;range.first!=range.second;++range.first)
      {
         out << " " << range.first->toString();
      }
   }
   //
   out << std::endl << std::endl;
   //
   ++nCalls;
}

QString toSANMove(const ChessPosition& position, const ChessMoveMap& moves, const ChessMove& move)
{
   // @@note: call this function before move is applied to position
   //
   QString s;
   //
   s.reserve(8);
   //
   ChessPosition nextPosition(position);
   //
   ChessMoveType moveType = g_chessRules.applyMove(nextPosition, move);
   //
   if(moveType & moveShortCastling)
   {
      s.append("O-O");
   }
   else if(moveType & moveLongCastling)
   {
      s.append("O-O-O");
   }
   else
   {
      ChessPiece piece = position.cell(move.from);
      //
      if(piece.type()==ptPawn)
      {
         if(moveType & moveCapture)
         {
            s.append(colToChar(move.from.col));
            s.append('x');
         }
      }
      else
      {
         s.append(ChessPiece(piece.type()|pcWhite).toChar());
         // check for ambiguous move
         ChessPosition testPosition(position);
         testPosition.setCell(move.from, ChessPiece(ptPawn|getOpponent(piece.color())));
         //
         std::list<CoordPair> movesTo;
         moves.getMovesTo(move.to, movesTo);
         //
         ChessCoord another;
         bool needFullCoord = false; // if 3 or more options are available
         std::list<CoordPair>::const_iterator it=movesTo.begin(), itEnd=movesTo.end();
         for(;it!=itEnd;++it)
         {
            if(it->from==move.from) continue;
            if(position.cell(it->from)!=piece) continue;
            // found another
            if(another==ChessCoord())
            {
               another = it->from;
            }
            else
            {
               needFullCoord = true;
               break;
            }
         }
         //
         if(needFullCoord)
         {
            s.append(colToChar(move.from.col));
            s.append(rowToChar(move.from.row));
         }
         else if(another!=ChessCoord())
         {
            // disambiguate
            if(another.col!=move.from.col)
            {
               s.append(colToChar(move.from.col));
            }
            else
            {
               s.append(rowToChar(move.from.row));
            }
         }
         if(moveType & moveCapture)
         {
            s.append('x');
         }
      }
      //
      s.append(colToChar(move.to.col));
      s.append(rowToChar(move.to.row));
      //
      if(move.promotion!=ptNone)
      {
         s.append(ChessPiece(move.promotion|pcBlack).toChar());
      }
      //
      ChessMoveMap moves;
      //
      g_chessRules.findPossibleMoves(nextPosition, moves);
      //
      if(g_chessRules.isKingChecked(nextPosition.sideToMove(), nextPosition))
      {
         if(moves.empty())
         {
            s.append('#');
         }
         else
         {
            s.append('+');
         }
      }
   }
   //
   return s;
}

// helper function for converting SAN moves to coordinate algebraic form
CoordPair findPieceMove(const ChessMoveMap& possibleMoves,
                        const ChessPosition& position,
                        PieceType pt, ChessCoord from,
                        ChessCoord to)
{
   if(from.col&&from.row)
   {
      ChessMoveMap::const_iterator_pair range = possibleMoves.getMovesFrom(from);
      //
      for(;range.first!=range.second;++range.first)
      {
         if(range.first->to==to) return CoordPair(from, to);
      }
   }
   else
   {
      std::list<CoordPair> moves;
      possibleMoves.getMovesTo(to, moves);
      std::list<CoordPair>::const_iterator it = moves.begin(), itEnd = moves.end();
      for(;it!=itEnd;++it)
      {
         if(!from.col && !from.row &&
            position.cell(it->from).type()==pt)
         {
            return *it;
         }
         else if(from.col && it->from.col==from.col &&
            position.cell(it->from).type()==pt)
         {
            return *it;
         }
         else if(from.row && it->from.row==from.row &&
                 position.cell(it->from).type()==pt)
         {
            return *it;
         }
      }
   }
   return CoordPair(); // no such move!
}

CoordPair findSANMove(const ChessMoveMap& moves, const ChessPosition& position,
                      const std::string& move_str)

{
   if(move_str.length()<2 || move_str.length()>7) return CoordPair();
   //
   if(move_str=="O-O")
   {
      RowValue row = position.sideToMove()==pcWhite ? 1 : 8;
      return CoordPair(ChessCoord(5, row), ChessCoord(7, row));
   }
   else if(move_str=="O-O-O")
   {
      RowValue row = position.sideToMove()==pcWhite ? 1 : 8;
      return CoordPair(ChessCoord(5, row), ChessCoord(3, row));
   }
   //
   unsigned i=0;
   //
   PieceType pt(ptNone);
   if(colFromChar(move_str[0])==0)
      pt = ChessPiece::fromChar(move_str[0]).type();
   //
   if(pt==ptNone) pt = ptPawn;
   else ++i;
   //
   std::vector<ColValue> cols;
   std::vector<RowValue> rows;
   cols.reserve(2);
   rows.reserve(2);
   //
   PieceType promotion(ptNone);
   //
   for(; i<move_str.size(); ++i)
   {
      char c = move_str[i];
      ColValue col = colFromChar(c);
      if(col)
         cols.push_back(col);
      else
      {
         RowValue row = rowFromChar(c);
         if(row)
            rows.push_back(row);
         else
         {
            promotion = ChessPiece::fromChar(c).type();
         }
      }
   }
   //
   ChessCoord from, to;
   //
   if(cols.empty() || cols.size()>2) return ChessMove();
   //
   if(cols.size()==2)
   {
      from.col = cols[0];
      to.col = cols[1];
   }
   else
   {
      to.col = cols[0];
   }
   //
   if(rows.empty() || rows.size()>2) return ChessMove();
   //
   if(rows.size()==2)
   {
      from.row = rows[0];
      to.row = rows[1];
   }
   else
   {
      to.row = rows[0];
   }
   //
   if(!position.inRange(to)) return ChessMove();
   //
   switch(promotion)
   {
      case ptNone:
      case ptQueen:
      case ptKnight:
      case ptBishop:
         break;
      default:
         return CoordPair();  // illegal promotion piece type
   }
   //
   ChessMove move = findPieceMove(moves, position, pt, from, to);
   move.promotion = promotion;
   return move;
}

}

ChessGame::ChessGame(const QString& whitePlayerName,
                     const QString& blackPlayerName) :
   whitePlayerName_(whitePlayerName),
   blackPlayerName_(blackPlayerName),
   result_(resultNone)
{
   positions_.reserve(100);
   gameMoves_.reserve(100);
}

const QString& ChessGame::whitePlayerName() const
{
   return whitePlayerName_;
}

const QString& ChessGame::blackPlayerName() const
{
   return blackPlayerName_;
}

void ChessGame::start()
{
   start(g_settings.defaultBoardSetup().toStdString());
}

void ChessGame::start(const std::string &fen)
{
   if(!startTime_.isNull())
   {
      assert(false);  // attempt to start a running game
      return;
   }
   //
   positions_.clear();
   positions_.push_back(fen);
   //
   position_ = ChessPosition::fromString(fen);
   //
   gameMoves_.clear();
   sanMoves_.clear();
   //
   startTime_ = QDateTime::currentDateTime();
   //
   recalcPossibleMoves();
}

void ChessGame::stop(ChessGameResult result, const QString& message)
{
   endTime_ = QDateTime::currentDateTime();
   //
   result_ = result;
   resultMessage_ = message;
}

ChessGameResult ChessGame::result() const
{
   return result_;
}

const QString& ChessGame::resultMessage() const
{
   return resultMessage_;
}

QDateTime ChessGame::startTime() const
{
   return startTime_;
}

QDateTime ChessGame::endTime() const
{
   return endTime_;
}

const std::vector<ChessMove>& ChessGame::moves() const
{
   return gameMoves_;
}

const ChessMoveMap& ChessGame::possibleMoves() const
{
   return possibleMoves_;
}

bool ChessGame::applyMove(const ChessMove& move)
{
   if(!possibleMoves_.contains(move))
   {
      return false; // attempt to make an illegal move
   }
   //
   QString sanMove = toSANMove(position_, possibleMoves_, move);
   sanMoves_.push_back(sanMove);
   //
   gameMoves_.push_back(move);
   //
   g_chessRules.applyMove(position_, move);
   positions_.push_back(position_.toString());
   //
   recalcPossibleMoves();
   //
   if(position_.halfCount()>=100)
   {
      emit fiftyMovesDetected();
   }
   //
   return true;
}

const ChessPosition& ChessGame::position() const
{
   return position_;
}

const ChessMove& ChessGame::lastMove() const
{
   if(gameMoves_.empty())
   {
      static const ChessMove dummy;
      return dummy;
   }
   else
   {
      return gameMoves_.back();
   }
}

const QString& ChessGame::lastSANMove() const
{
   if(sanMoves_.empty())
   {
      static const QString dummy;
      return dummy;
   }
   else
   {
      return sanMoves_.back();
   }
}

void ChessGame::recalcPossibleMoves()
{
   g_chessRules.findPossibleMoves(position_, possibleMoves_);
   //
   logPositionAndMoves(position_, possibleMoves_);
   //
   if(possibleMoves_.empty())
   {
      if(g_chessRules.isKingChecked(position_.sideToMove(), position_))
      {
         emit checkmateDetected();
      }
      else
      {
         emit stalemateDetected();
      }
   }
}

QString ChessGame::toPGN() const
{
   QString pgn;
   pgn.reserve(2048);
   //
   pgn.append("[Event \"K3Chess game\"]\n");
   pgn.append("[Site \"?\"]");
   pgn.append("[Date \"");
   pgn.append(startTime_.date().toString("yyyy.MM.dd"));
   pgn.append("\"]\n");
   pgn.append("[Round \"?\"]");
   //
   pgn.append("[White \"");
   pgn.append(whitePlayerName_);
   pgn.append("\"]\n");
   //
   pgn.append("[Black \"");
   pgn.append(blackPlayerName_);
   pgn.append("\"]\n");
   //
   pgn.append("[Result \"");
   //
   QString score;
   score.reserve(8);
   //
   switch(result_)
   {
      case resultWhiteCheckmates:
      case resultBlackResigns:
      case resultWhiteWonOnTime:
         score.append("1-0");
         break;
      case resultBlackCheckmates:
      case resultWhiteResigns:
      case resultBlackWonOnTime:
         score.append("0-1");
         break;
      case resultDrawnByAgreement:
      case resultDrawnByStalemate:
      case resultDrawnByRepetition:
      case resultDrawnBy50MoveRule:
         score.append("1/2-1/2");
         break;
      default:
         break;
   }
   //
   pgn.append(" ");
   pgn.append(score.isEmpty() ? QString("*") : score);
   pgn.append("\"]\n");
   //
   pgn.append("[Termination \"");
   pgn.append(resultMessage_);
   pgn.append("\"]\n");
   //
   for(unsigned i=0; i<sanMoves_.size(); ++i)
   {
      if((i%10)==0)
      {
         pgn.append('\n');
      }
      //
      if(i>0)
      {
         pgn.append(" ");
      }
      //
      if((i%2)==0)
      {
         pgn.append(QString::number(i+1));
         pgn.append(". ");
      }
      //
      pgn.append(sanMoves_[i]);
   }
   //
   pgn.append(score);
   //
   pgn.append("\n\n\n");
   //
   return QString(pgn);
}

bool ChessGame::takebackOneFullMove()
{
   if(positions_.size()<=2) return false; // nothing to take back
   //
   positions_.pop_back();
   positions_.pop_back();
   position_ = ChessPosition::fromString(positions_.back());
   //
   gameMoves_.pop_back();
   gameMoves_.pop_back();
   //
   sanMoves_.pop_back();
   sanMoves_.pop_back();
   //
   recalcPossibleMoves();
   //
   return true;
}

ChessMove ChessGame::interpretSANMove(const std::string &san)
{
   if(possibleMoves_.empty()) return ChessMove();
   //
   return findSANMove(possibleMoves_, position_, san);
}

