#ifndef __ChessGame_h
#define __ChessGame_h

#include "ChessPosition.h"
#include "ChessCoord.h"
#include "ChessMove.h"
#include "ChessRules.h"
#include "GameProfile.h"

#include <QDateTime>
#include <QStringList>
#include <vector>

enum ChessGameResult { resultNone, // e.g. still under way, or canceled
                       resultWhiteCheckmates, resultBlackCheckmates,
                       resultWhiteWonOnTime, resultBlackWonOnTime,
                       resultWhiteResigns, resultBlackResigns,
                       resultDrawnByAgreement, resultDrawnByRepetition,
                       resultDrawnByStalemate, resultDrawnBy50MoveRule };

class ChessPlayer;

class ChessGame : public QObject
{
   Q_OBJECT
public:
   ChessGame(const QString& whitePlayerName, const QString& blackPlayerName);

   const QString& whitePlayerName() const;
   const QString& blackPlayerName() const;

   void start(); // prepares default initial position and sets startTime
   void start(const ChessPosition& position); // setups initial position and sets startTime
   void stop(ChessGameResult result, const QString& message);

   const GameProfile& profile() const; // current profile snapshot

   ChessGameResult result() const;
   const QString& resultMessage() const;

   QDateTime startTime() const;
   QDateTime endTime() const;

   bool applyMove(const ChessMove& move); // applies move and advances position (only if move is valid)
   bool takebackOneFullMove();

   const ChessPosition& position() const; // returns current position
   const ChessMove& lastMove() const;            // returns last move
   const QString& lastSANMove() const; // last move in PGN notation

   ChessMove interpretMoveString(const std::string& move_str) const; // among moves possible in the current position

   const std::vector<ChessMove>& moves() const; // all game moves up to current position
   const QStringList& sanMoves() const; // all game moves in SAN format up to current position
   QString toPGN() const; // get all game in PGN notation

   const ChessMoveMap& possibleMoves() const; // possible moves for current position

signals:
   void checkmateDetected();
   void stalemateDetected();
   void repetitionDetected();
   void fiftyMovesDetected();

private:
   void recalcPossibleMoves();
   void appendSANMove();
   unsigned addPositionOccurrence(const std::string &fen);

private:
   QString whitePlayerName_;
   QString blackPlayerName_;

   QDateTime startTime_;
   QDateTime endTime_;
   ChessGameResult result_;
   QString resultMessage_;
   ChessPosition position_; // current position

   std::vector<std::string> positions_; // game positions in the form of fen strings
   std::vector<ChessMove> gameMoves_;   // game moves in coordinate algebraic form
   QStringList sanMoves_;  // game moves in standard algebraic notation

   ChessMoveMap possibleMoves_; // precalculated possible moves from current position
   std::map<std::string, unsigned> positionOccurrences_; // for theefold repetition detection
};

#endif
