#ifndef __GameSession_h
#define __GameSession_h

#include "ChessPlayer.h"
#include "GameSessionInfo.h"

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
               const GameSessionInfo& initialInfo);
   // @@note: chess player objects are not owned by game session
   // @@note: initialInfo can contain a saved position

   void begin();

   ChessClock whiteClock() const { return whiteClock_; } // instant white clock value
   ChessClock blackClock() const { return blackClock_; } // instant black clock value

   const ChessPlayer *whitePlayer() const { return whitePlayer_; }
   const ChessPlayer *blackPlayer() const { return blackPlayer_; }

   const ChessGame& game() const { return game_; }
   const GameSessionInfo& sessionInfo() const { return sessionInfo_; }

   GameType type() const { return sessionInfo_.profile.type; }

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
   QString getResultMessage(GameSessionEndReason reason, ChessGameResult result);

   void requestMove(ChessPlayer *player);
   void outputLastMove();

   void enableInGameCommands();
   void updateSessionInfo();

private:
   ChessPlayer *whitePlayer_;
   ChessPlayer *blackPlayer_;
   ChessGame game_;
   bool whitePlayerReady_;
   bool blackPlayerReady_;
   //
   ChessClock whiteClock_;       // most current clocks
   ChessClock blackClock_;
   //
   GameSessionInfo sessionInfo_; // saveable data (can be used for restoring the game)
                                 // updated once per move
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
