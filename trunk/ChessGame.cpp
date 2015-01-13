#include "ChessGame.h"
#include "Settings.h"
#include "StringUtils.h"

#include <fstream>

namespace
{

void logString(const std::string& str)
{
   static unsigned nCalls = 0;
   static bool noLog = false;
   if(noLog) return;
   std::ofstream out("./logs/pos_moves.log", nCalls==0 ? std::ios::trunc : std::ios::app);
   noLog = out.fail();
   if(noLog) return; // will fail if there is no 'logs' folder
   out << str << std::endl;
   ++nCalls;
}

void logIllegalMove(const ChessMove& move)
{
   logString(std::string("Attempt to make an illegal move: ") + move.toString());
}

void logPositionAndMoves(const ChessPosition& position, const ChessMoveMap& moves)
{
   std::string str;
   str.reserve(200);
   str.append("Position: ");
   str.append(position.toString());
   str.append("\n");
   str.append("Available moves:");
   //
   ChessMoveMap::const_iterator_pair range =
         moves.getAllMoves();
   //
   if(range.first==range.second)
   {
      str.append(" -");
   }
   else
   {
      for(;range.first!=range.second;++range.first)
      {
         str.append(" ");
         str.append(range.first->toString());
      }
   }
   //
   str.append("\n");
   //
   logString(str);
}

QString toSANMove(const ChessPosition& position,
                  const ChessMove& move,
                  ChessMoveType moveType,
                  const ChessMoveMap& moves,
                  bool isCheck,
                  bool isCheckmate)
{
   // @@note: call this function before move is applied to position
   //
   QString s;
   //
   s.reserve(8);
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
   }
   //
   if(isCheckmate)
   {
      s.append('#');
   }
   else if(isCheck)
   {
      s.append('+');
   }
   //
   return s;
}

QString toRuMove(const ChessPosition& position,
                 const ChessMove& move,
                 ChessMoveType moveType,
                 bool isCheck,
                 bool isCheckmate)
{
   QString s;
   //
   s.reserve(8);
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
      switch(piece.type())
      {
         case ptKing:   s.append(QChar(0x041A));
                        s.append(QChar(0x0440));
                        break;
         case ptQueen:  s.append(QChar(0x0424)); break;
         case ptRook:   s.append(QChar(0x041B)); break;
         case ptBishop: s.append(QChar(0x0421)); break;
         case ptKnight: s.append(QChar(0x041A)); break;
         default:
            break;
      }
      //
      s.append(move.from.toString().c_str());
      if(moveType & moveCapture)
         s.append(QChar(0x003A));
      else
         s.append(QChar(0x2014));
      //
      s.append(move.to.toString().c_str());
      //
      if(moveType & movePromotion)
      {
         switch(piece.type())
         {
            case ptQueen:  s.append(QChar(0x0424)); break;
            case ptRook:   s.append(QChar(0x041B)); break;
            case ptBishop: s.append(QChar(0x0421)); break;
            case ptKnight: s.append(QChar(0x041A)); break;
            default:
               break;
         }
      }
   }
   //
   if(isCheckmate)
   {
      s.append(QChar(0x2A09));
   }
   else if(isCheck)
   {
      s.append(QChar(0x002B));
   }
   //
   s.append(' ');
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

ChessMove findSANMove(const ChessMoveMap& moves, const ChessPosition& position,
                      const std::string& move_str)

{
   if(move_str.length()<2 || move_str.length()>7) return CoordPair();
   //
   if(startsWith(move_str, "O-O-O"))
   {
      if(position.isChess960())
      {
         ChessCoord targetKingCoord(position.initialLeftRookCoord().col,
                                    position.initialKingCoord().row);
         return CoordPair(position.initialKingCoord(), targetKingCoord);
      }
      else
      {
         RowValue row = position.initialKingCoord().row;
         return CoordPair(ChessCoord(5, row), ChessCoord(3, row));
      }
   }
   else if(startsWith(move_str, "O-O"))
   {
      if(position.isChess960())
      {
         ChessCoord targetKingCoord(position.initialRightRookCoord().col,
                                    position.initialKingCoord().row);
         return CoordPair(position.initialKingCoord(), targetKingCoord);
      }
      else
      {
         RowValue row = position.initialKingCoord().row;
         return CoordPair(ChessCoord(5, row), ChessCoord(7, row));
      }
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
            PieceType pt = ChessPiece::fromChar(c).type();
            if(pt!=ptNone) promotion = pt;
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

ChessMove interpretDashedMove(const std::string& move_str)
{
   // interprets moves in the format containing two coordinates
   // separated with a dash, and possibly preceded by a piece symbol
   size_t pos = move_str.find('-');
   if(pos==std::string::npos || pos < 2 || move_str.length()-pos < 3) return ChessMove();
   ChessMove move;
   move.from = ChessCoord::fromString(move_str.substr(pos-2, 2));
   if(move.from==ChessCoord()) return ChessMove();
   move.to = ChessCoord::fromString(move_str.substr(pos+1, 2));
   if(move.from==ChessCoord()) return ChessMove();
   if(move_str.length()-pos>3)
   {
      ChessPiece piece = ChessPiece::fromChar(move_str[pos+3]);
      switch(piece.type())
      {
         case ptQueen:
         case ptRook:
         case ptBishop:
         case ptKnight:
            move.promotion = piece.type();
            break;
         default:
            break;
      }
   }
   return move;
}


QString getPgnScore(ChessGameResult result)
{
   switch(result)
   {
      case resultWhiteCheckmates:
      case resultBlackResigns:
      case resultWhiteWonOnTime:
         return "1-0";
         break;
      case resultBlackCheckmates:
      case resultWhiteResigns:
      case resultBlackWonOnTime:
         return "0-1";
         break;
      case resultDrawnByAgreement:
      case resultDrawnByStalemate:
      case resultDrawnByRepetition:
      case resultDrawnBy50MoveRule:
         return "1/2-1/2";
         break;
      default:
         return QString();
   }
}

QString getPgnResultMessage(ChessGameResult result)
{
   switch(result)
   {
   case resultWhiteCheckmates: return "White won by checkmate";
   case resultBlackCheckmates: return "Black won by checkmate";
   case resultWhiteWonOnTime: return "White won on time";
   case resultBlackWonOnTime: return "Black won on time";
   case resultWhiteResigns: return "Black won by resignation";
   case resultBlackResigns: return "White won by resignation";
   case resultDrawnByAgreement: return "Game drawn by agreement";
   case resultDrawnByRepetition: return "Game drawn by threefold repetition";
   case resultDrawnByStalemate: return "Game drawn by stalemate";
   case resultDrawnBy50MoveRule: return "Game drawn by 50 move rule";
   default:
      return QString();
   }
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
   start(cStandardInitialPosition);
}

void ChessGame::start(const ChessPosition& position)
{
   if(!startTime_.isNull())
   {
      assert(false);  // attempt to start a running game
      return;
   }
   //
   positions_.clear();
   positions_.push_back(position.toString());
   //
   addPositionOccurrence(positions_.back());
   //
   position_ = position;
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
      logIllegalMove(move);
      return false; // attempt to make an illegal move
   }
   //
   ChessPosition prevPosition(position_);
   //
   gameMoves_.push_back(move);
   //
   ChessMoveType moveType = g_chessRules.applyMove(position_, move);
   positions_.push_back(position_.toString());
   //
   ChessMoveMap prevPossibleMoves = possibleMoves_;
   //
   recalcPossibleMoves();
   //
   bool isCheck = g_chessRules.isKingChecked(position_.sideToMove(), position_);
   bool isCheckmate = possibleMoves_.empty() && isCheck;
   //
   QString sanMove = toSANMove(prevPosition, move, moveType, prevPossibleMoves, isCheck, isCheckmate);
   QString ruMove = toRuMove(prevPosition, move, moveType, isCheck, isCheckmate);
   //
   sanMoves_.push_back(sanMove);
   ruMoves_.push_back(ruMove);
   //
   if(addPositionOccurrence(positions_.back())>=3)
   {
      emit repetitionDetected();
   }
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

const QString& ChessGame::lastRuMove() const
{
   if(ruMoves_.empty())
   {
      static const QString dummy;
      return dummy;
   }
   else
   {
      return ruMoves_.back();
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
   pgn.append("[Event \"K3Chess game\"]\r\n");
   pgn.append("[Site \"?\"]\r\n");
   pgn.append("[Date \"");
   pgn.append(startTime_.date().toString("yyyy.MM.dd"));
   pgn.append("\"]\r\n");
   pgn.append("[Round \"?\"]\r\n");
   //
   pgn.append("[White \"");
   pgn.append(whitePlayerName_);
   pgn.append("\"]\r\n");
   //
   pgn.append("[Black \"");
   pgn.append(blackPlayerName_);
   pgn.append("\"]\r\n");
   //
   pgn.append("[Result \"");
   //
   QString score = getPgnScore(result_);
   pgn.append(score.isEmpty() ? QString("*") : score);
   pgn.append("\"]\r\n");
   //
   pgn.append("[Termination \"");
   pgn.append(getPgnResultMessage(result_));
   pgn.append("\"]\r\n");
   //
   if(position_.isChess960())
   {
      pgn.append("[Variant \"Chess960\"]\r\n");
   }
   //
   if(positions_[0]!=cStandardInitialFen)
   {
      pgn.append("[FEN \"");
      pgn.append(QString::fromStdString(positions_[0]));
      pgn.append("\"]\r\n");
   }
   //
   for(unsigned i=0; i<(unsigned)sanMoves_.size(); ++i)
   {
      if((i%10)==0)
      {
         pgn.append("\r\n");
      }
      //
      if(i>0)
      {
         pgn.append(" ");
      }
      //
      if((i%2)==0)
      {
         pgn.append(QString::number(i/2+1));
         pgn.append(". ");
      }
      //
      pgn.append(sanMoves_[i]);
   }
   //
   pgn.append(" ");
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
   bool isChess960 = position_.isChess960();
   //
   positions_.pop_back();
   positions_.pop_back();
   position_ = ChessPosition::fromString(positions_.back());
   position_.setChess960(isChess960);
   //
   gameMoves_.pop_back();
   gameMoves_.pop_back();
   //
   sanMoves_.pop_back();
   sanMoves_.pop_back();
   //
   ruMoves_.pop_back();
   ruMoves_.pop_back();
   //
   recalcPossibleMoves();
   //
   return true;
}

ChessMove ChessGame::interpretMoveString(const std::string &move_str) const
{
   if(possibleMoves_.empty()) return ChessMove();
   //
   ChessMove move = ChessMove::fromString(move_str);
   if(move==ChessMove())
   {
      move = findSANMove(possibleMoves_, position_, move_str);
      if(move==ChessMove())
      {
         move = interpretDashedMove(move_str);
      }
   }
   //
   return move;
}

unsigned ChessGame::addPositionOccurrence(const std::string& fen)
{
   std::string::size_type pos = fen.find_last_of(' ');
   pos = fen.find_last_of(' ', pos-1);
   std::string f = fen.substr(0, pos);
   std::map<std::string, unsigned>::iterator it =
         positionOccurrences_.find(f);
   if(it==positionOccurrences_.end())
   {
      positionOccurrences_.insert(it, std::make_pair(f, 1));
      return 1;
   }
   else
   {
      ++it->second;
      return it->second;
   }
}

const QStringList & ChessGame::sanMoves() const
{
   return sanMoves_;
}

const QStringList & ChessGame::ruMoves() const
{
   return ruMoves_;
}
