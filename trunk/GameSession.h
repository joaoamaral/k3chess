#ifndef __GameSession_h
#define __GameSession_h

#include "ChessPlayer.h"
#include "ChessGame.h"
#include "GameProfile.h"

#include <QObject>
#include <QTimer>

#include <map>
#include <string>

enum GameSessionEndReason { reasonWhitePlayerIsNotReady,
                            reasonBlackPlayerIsNotReady,
                            reasonBothPlayersIsNotReady,
                            reasonGameAborted,
                            reasonGameFinished };

// GameSession initiates and controls a specific game session

class GameSession : public QObject
{
   Q_OBJECT
public:
   GameSession(ChessPlayer *whitePlayer, ChessPlayer *blackPlayer,
               const GameProfile& profile);
   // @@note: chess player objects are not owned by game session

   void begin();

   ChessClock whiteClock() const { return profile_.whiteClock; } // instant white clock value
   ChessClock blackClock() const { return profile_.blackClock; } // instant black clock value

   const ChessPlayer *whitePlayer() const { return whitePlayer_; }
   const ChessPlayer *blackPlayer() const { return blackPlayer_; }

   QString pgn() const;

   GameType type() const { return profile_.type; }

signals:
   void end(GameSessionEndReason reason, ChessGameResult result,
            const QString& message);

private slots:
   void white_isReady();
   void white_moves(const ChessMove& move);
   void white_moves(const std::string& move_str);
   void white_offersDraw();
   void white_resigns();
   void white_says(const QString& msg);
   void white_requestsTakeback();
   void white_requestsAbort();

   void black_isReady();
   void black_moves(const ChessMove& move);
   void black_moves(const std::string& move_str);
   void black_offersDraw();
   void black_resigns();
   void black_says(const QString& msg);
   void black_requestsTakeback();
   void black_requestsAbort();

   void getReadyTimeout();

   void clockUpdateTimer();

   void checkmateDetected();
   void stalemateDetected();
   void fiftyMovesDetected();
   void repetitionDetected();

private:
   void connectPlayers();
   void disconnectPlayers();
   void connectGame();
   void disconnectGame();

   void verifyBothPlayersReady();
   void startGame();
   void endGame(GameSessionEndReason reason, ChessGameResult result);

   unsigned addPositionOccurrence(const ChessPosition& position);
   bool checkRepetition();

   void requestMove(ChessPlayer *player);
   void outputLastMove();

   QString getResultMessage(GameSessionEndReason reason, ChessGameResult result);

private:
   ChessPlayer *whitePlayer_;
   ChessPlayer *blackPlayer_;
   ChessGame game_;
   bool whitePlayerReady_;
   bool blackPlayerReady_;
   //
   QTime counter_;
   GameProfile profile_;
   //
   QTimer getReadyTimer_;
   QTimer clockUpdateTimer_;
   //
   bool whiteDrawOfferActive_;
   bool blackDrawOfferActive_;
   bool canDrawByRepetition_;
   //
   std::map<std::string, unsigned> positionOccurrences_; // for theefold repetition detection
};

#endif
