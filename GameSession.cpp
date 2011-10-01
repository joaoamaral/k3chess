#include "GameSession.h"
#include "LocalChessGui.h"
#include "GlobalStrings.h"
#include "CommandOptionDefs.h"

namespace
{

const int cGetReadyTimeout = 5000; // ms
// if a player does not send isReady() signal within this timeout
// he/she/it is considered not ready and the game session aborts

const int cClockUpdateInterval = 100; // ms


QString gameResultToMessage(ChessGameResult result)
{
   switch(result)
   {
      case resultDrawnByAgreement:  return g_msg("GameDrawnByAgreement");
      case resultDrawnByStalemate:  return g_msg("GameDrawnByStalemate");
      case resultDrawnByRepetition: return g_msg("GameDrawnByRepetition");
      case resultDrawnBy50MoveRule: return g_msg("GameDrawnBy50MoveRule");
      case resultWhiteCheckmates:   return g_msg("WhiteCheckmates");
      case resultBlackCheckmates:   return g_msg("BlackCheckmates");
      case resultWhiteWonOnTime:    return g_msg("WhiteWonOnTime");
      case resultBlackWonOnTime:    return g_msg("BlackWonOnTime");
      case resultWhiteResigns:      return g_msg("WhiteResigns");
      case resultBlackResigns:      return g_msg("BlackResigns");
      case resultNone:              return QString();
   }
   return QString(); // dummy line to suppress compiler warning
}

}

GameSession::GameSession(ChessPlayer *whitePlayer, ChessPlayer *blackPlayer,
                         const GameProfile& profile) :
   whitePlayer_(whitePlayer), blackPlayer_(blackPlayer),
   game_(whitePlayer->name(), blackPlayer->name()),
   whitePlayerReady_(false), blackPlayerReady_(false), profile_(profile),
   whiteDrawOfferActive_(false), blackDrawOfferActive_(false),
   canDrawByRepetition_(false)
{
   QObject::connect(&getReadyTimer_, SIGNAL(timeout()), this, SLOT(getReadyTimeout()), Qt::UniqueConnection);
   QObject::connect(&clockUpdateTimer_, SIGNAL(timeout()), this, SLOT(clockUpdateTimer()), Qt::UniqueConnection);
   //
   connectPlayers();
   connectGame();
}

void GameSession::begin()
{
   g_commandOptionDefs.inGameOptions().disable(cmd_InGame_Takeback);
   g_commandOptionDefs.inGameOptions().disable(cmd_InGame_OfferDraw);
   g_commandOptionDefs.inGameOptions().disable(cmd_InGame_Resign);
   //
   whitePlayer_->getReady();
   blackPlayer_->getReady();
   //
   getReadyTimer_.setSingleShot(true);
   getReadyTimer_.start(cGetReadyTimeout);
}

void GameSession::getReadyTimeout()
{
   if(!whitePlayerReady_ && !blackPlayerReady_)
   {
      QString msg;
      //
      msg.reserve(200);
      msg.append(g_msg("PlayerIsNotReady").arg(whitePlayer_->name()));
      msg.append(", ");
      msg.append(g_msg("PlayerIsNotReady").arg(blackPlayer_->name()));
      //
      g_localChessGui.showSessionMessage(msg);
      //
      emit end(reasonBothPlayersIsNotReady, resultNone, msg);
   }
   else if(!whitePlayerReady_)
   {
      QString msg = g_msg("PlayerIsNotReady").arg(whitePlayer_->name());
      //
      g_localChessGui.showSessionMessage(msg);
      //
      emit end(reasonWhitePlayerIsNotReady, resultNone, msg);
   }
   else if(!blackPlayerReady_)
   {
      QString msg = g_msg("PlayerIsNotReady").arg(blackPlayer_->name());
      //
      g_localChessGui.showSessionMessage(msg);
      //
      emit end(reasonBlackPlayerIsNotReady, resultNone, msg);
   }
   else
   {
      // players are ready, ignore this event
   }
}

void GameSession::requestMove(ChessPlayer *player)
{
   player->makeMove(game_.position(), game_.lastMove(), profile_.whiteClock, profile_.blackClock);
}

void GameSession::white_isReady()
{
   whitePlayerReady_ = true;
   verifyBothPlayersReady();
}

void GameSession::white_moves(const ChessMove& move)
{
   if(game_.position().sideToMove()!=pcWhite)
   {
      assert(false);
      // report error: attempt to move out of turn
      return;
   }
   //
   if(!game_.applyMove(move))
   {
      g_localChessGui.showSessionMessage(g_msg("IllegalMove").arg(move.toString().c_str()));
      requestMove(whitePlayer_);
   }
   else if(checkRepetition())
   {
      whiteDrawOfferActive_ = false;
      blackDrawOfferActive_ = false;
      //
      endGame(reasonGameFinished, resultDrawnByRepetition);
   }
   else
   {
      if(blackDrawOfferActive_)
      {
         blackPlayer_->opponentRejectsDraw();
      }
      //
      whiteDrawOfferActive_ = false;
      blackDrawOfferActive_ = false;
      //
      profile_.whiteClock.remainingTime += profile_.whiteClock.moveIncrement;
      //
      outputLastMove();
      g_localChessGui.updatePosition(game_.position(), game_.lastMove(),
                                     game_.possibleMoves());
      //
      blackPlayer_->opponentMoves(move);
      if(game_.result()==resultNone && profile_.blackClock.remainingTime > 0)
      {
         requestMove(blackPlayer_);
      }
   }
}

void GameSession::white_moves(const std::string& move)
{
   white_moves(game_.interpretSANMove(move));
}

void GameSession::white_offersDraw()
{
   if(blackDrawOfferActive_)
   {
      blackDrawOfferActive_ = false;
      blackPlayer_->opponentAcceptsDraw();
      //
      g_localChessGui.showSessionMessage(
            g_msg("PlayerAcceptsDraw").arg(whitePlayer_->name()));
      //
      endGame(reasonGameFinished, resultDrawnByAgreement);
   }
   else if(canDrawByRepetition_)
   {
      endGame(reasonGameFinished, resultDrawnByRepetition);
   }
   else
   {
      whiteDrawOfferActive_ = true;
      blackPlayer_->opponentOffersDraw();
      //
      g_localChessGui.showSessionMessage(
            g_msg("PlayerOffersDraw").arg(whitePlayer_->name()));
   }
}

void GameSession::white_resigns()
{
   blackPlayer_->opponentResigns();
   endGame(reasonGameFinished, resultWhiteResigns);
}

void GameSession::white_says(const QString& msg)
{
   g_localChessGui.showPlayerMessage(whitePlayer_->name(), msg);
   blackPlayer_->opponentSays(msg);
}

void GameSession::black_isReady()
{
   blackPlayerReady_ = true;
   verifyBothPlayersReady();
}

void GameSession::black_moves(const ChessMove& move)
{
   if(game_.position().sideToMove()!=pcBlack)
   {
      assert(false);
      // report error: attempt to move out of turn
      return;
   }
   //
   if(!game_.applyMove(move))
   {
      g_localChessGui.showSessionMessage(g_msg("IllegalMove").arg(move.toString().c_str()));
      requestMove(blackPlayer_);
   }
   else if(checkRepetition())
   {
      whiteDrawOfferActive_ = false;
      blackDrawOfferActive_ = false;
      //
      endGame(reasonGameFinished, resultDrawnByRepetition);
   }
   else
   {
      if(whiteDrawOfferActive_)
      {
         whitePlayer_->opponentRejectsDraw();
      }
      //
      if(game_.position().moveNumber()==2)
      {
         g_commandOptionDefs.inGameOptions().enable(cmd_InGame_Takeback);
         g_commandOptionDefs.inGameOptions().enable(cmd_InGame_OfferDraw);
         g_commandOptionDefs.inGameOptions().enable(cmd_InGame_Resign);
      }
      //
      blackDrawOfferActive_ = false;
      whiteDrawOfferActive_ = false;
      //
      profile_.blackClock.remainingTime += profile_.blackClock.moveIncrement;
      //
      outputLastMove();
      g_localChessGui.updatePosition(game_.position(), game_.lastMove(),
                                     game_.possibleMoves());
      //
      whitePlayer_->opponentMoves(move);
      if(game_.result()==resultNone && profile_.whiteClock.remainingTime > 0)
      {
         requestMove(whitePlayer_);
      }
   }
}

void GameSession::black_moves(const std::string& move)
{
   black_moves(game_.interpretSANMove(move));
}

void GameSession::black_offersDraw()
{
   if(whiteDrawOfferActive_)
   {
      whiteDrawOfferActive_ = false;
      whitePlayer_->opponentAcceptsDraw();
      //
      g_localChessGui.showSessionMessage(
            g_msg("PlayerAcceptsDraw").arg(blackPlayer_->name()));
      //
      endGame(reasonGameFinished, resultDrawnByAgreement);
   }
   else if(canDrawByRepetition_)
   {
      endGame(reasonGameFinished, resultDrawnByRepetition);
   }
   else
   {
      blackDrawOfferActive_ = true;
      whitePlayer_->opponentOffersDraw();
      //
      g_localChessGui.showSessionMessage(
            g_msg("PlayerOffersDraw").arg(blackPlayer_->name()));
   }
}

void GameSession::black_resigns()
{
   whitePlayer_->opponentResigns();
   endGame(reasonGameFinished, resultWhiteResigns);
}

void GameSession::black_says(const QString& msg)
{
   g_localChessGui.showPlayerMessage(blackPlayer_->name(), msg);
   whitePlayer_->opponentSays(msg);
}

void GameSession::checkmateDetected()
{
   endGame(reasonGameFinished,
           game_.position().sideToMove()==pcWhite ?
           resultBlackCheckmates : resultWhiteCheckmates);
}

void GameSession::stalemateDetected()
{
   endGame(reasonGameFinished, resultDrawnByStalemate);
}

void GameSession::fiftyMovesDetected()
{
   endGame(reasonGameFinished, resultDrawnBy50MoveRule);
}

void GameSession::repetitionDetected()
{
   canDrawByRepetition_ = true;
}

void GameSession::connectPlayers()
{
   QObject::connect(whitePlayer_, SIGNAL(isReady()), this, SLOT(white_isReady()), Qt::UniqueConnection);
   QObject::connect(whitePlayer_, SIGNAL(playerMoves(ChessMove)), this, SLOT(white_moves(ChessMove)), Qt::UniqueConnection);
   QObject::connect(whitePlayer_, SIGNAL(playerMoves(const std::string&)), this, SLOT(white_moves(const std::string&)), Qt::UniqueConnection);
   QObject::connect(whitePlayer_, SIGNAL(playerOffersDraw()), this, SLOT(white_offersDraw()), Qt::UniqueConnection);
   QObject::connect(whitePlayer_, SIGNAL(playerResigns()), this, SLOT(white_resigns()), Qt::UniqueConnection);
   QObject::connect(whitePlayer_, SIGNAL(playerSays(const QString&)), this, SLOT(white_says(const QString&)), Qt::UniqueConnection);
   QObject::connect(whitePlayer_, SIGNAL(playerRequestsTakeback()), this, SLOT(white_requestsTakeback()), Qt::UniqueConnection);
   QObject::connect(whitePlayer_, SIGNAL(playerRequestsAbort()), this, SLOT(white_requestsAbort()), Qt::UniqueConnection);
   //
   QObject::connect(blackPlayer_, SIGNAL(isReady()), this, SLOT(black_isReady()), Qt::UniqueConnection);
   QObject::connect(blackPlayer_, SIGNAL(playerMoves(ChessMove)), this, SLOT(black_moves(ChessMove)), Qt::UniqueConnection);
   QObject::connect(blackPlayer_, SIGNAL(playerMoves(const std::string&)), this, SLOT(black_moves(const std::string&)), Qt::UniqueConnection);
   QObject::connect(blackPlayer_, SIGNAL(playerOffersDraw()), this, SLOT(black_offersDraw()), Qt::UniqueConnection);
   QObject::connect(blackPlayer_, SIGNAL(playerResigns()), this, SLOT(black_resigns()), Qt::UniqueConnection);
   QObject::connect(blackPlayer_, SIGNAL(playerSays(const QString&)), this, SLOT(black_says(const QString&)), Qt::UniqueConnection);
   QObject::connect(blackPlayer_, SIGNAL(playerRequestsTakeback()), this, SLOT(black_requestsTakeback()), Qt::UniqueConnection);
   QObject::connect(blackPlayer_, SIGNAL(playerRequestsAbort()), this, SLOT(black_requestsAbort()), Qt::UniqueConnection);
}

void GameSession::disconnectPlayers()
{
   QObject::connect(whitePlayer_, SIGNAL(isReady()), this, SLOT(white_isReady()));
   QObject::connect(whitePlayer_, SIGNAL(playerMoves(ChessMove)), this, SLOT(white_moves(ChessMove)));
   QObject::connect(whitePlayer_, SIGNAL(playerMoves(const std::string&)), this, SLOT(white_moves(const std::string&)));
   QObject::connect(whitePlayer_, SIGNAL(playerOffersDraw()), this, SLOT(white_offersDraw()));
   QObject::connect(whitePlayer_, SIGNAL(playerResigns()), this, SLOT(white_resigns()));
   QObject::connect(whitePlayer_, SIGNAL(playerSays(const QString&)), this, SLOT(white_says(const QString&)));
   QObject::connect(whitePlayer_, SIGNAL(playerRequestsTakeback()), this, SLOT(white_requestsTakeback()));
   QObject::connect(whitePlayer_, SIGNAL(playerRequestsAbort()), this, SLOT(white_requestsAbort()));
   //
   QObject::connect(blackPlayer_, SIGNAL(isReady()), this, SLOT(black_isReady()));
   QObject::connect(blackPlayer_, SIGNAL(playerMoves(ChessMove)), this, SLOT(black_moves(ChessMove)));
   QObject::connect(blackPlayer_, SIGNAL(playerMoves(const std::string&)), this, SLOT(black_moves(const std::string&)));
   QObject::connect(blackPlayer_, SIGNAL(playerOffersDraw()), this, SLOT(black_offersDraw()));
   QObject::connect(blackPlayer_, SIGNAL(playerResigns()), this, SLOT(black_resigns()));
   QObject::connect(blackPlayer_, SIGNAL(playerSays(const QString&)), this, SLOT(black_says(const QString&)));
   QObject::connect(blackPlayer_, SIGNAL(playerRequestsTakeback()), this, SLOT(black_requestsTakeback()));
   QObject::connect(blackPlayer_, SIGNAL(playerRequestsAbort()), this, SLOT(black_requestsAbort()));
}

void GameSession::connectGame()
{
   QObject::connect(&game_, SIGNAL(checkmateDetected()), this, SLOT(checkmateDetected()), Qt::UniqueConnection);
   QObject::connect(&game_, SIGNAL(stalemateDetected()), this, SLOT(stalemateDetected()), Qt::UniqueConnection);
   QObject::connect(&game_, SIGNAL(fiftyMovesDetected()), this, SLOT(fiftyMovesDetected()), Qt::UniqueConnection);
   QObject::connect(&game_, SIGNAL(repetitionDetected()), this, SLOT(repetitionDetected()), Qt::UniqueConnection);
}

void GameSession::disconnectGame()
{
   QObject::connect(&game_, SIGNAL(checkmateDetected()), this, SLOT(checkmateDetected()));
   QObject::connect(&game_, SIGNAL(stalemateDetected()), this, SLOT(stalemateDetected()));
   QObject::connect(&game_, SIGNAL(fiftyMovesDetected()), this, SLOT(fiftyMovesDetected()));
   QObject::connect(&game_, SIGNAL(repetitionDetected()), this, SLOT(repetitionDetected()));
}

void GameSession::verifyBothPlayersReady()
{
   if(whitePlayerReady_ && blackPlayerReady_)
   {
      getReadyTimer_.stop(); // @@todo: check if we need to use blockSignals(true) instead
      startGame();
   }
}

void GameSession::startGame()
{
   g_localChessGui.beginNewGame(whitePlayer_->name(), blackPlayer_->name(), profile_);
   //
   whitePlayer_->beginGame(pcWhite, blackPlayer_->name(), profile_.whiteClock);
   blackPlayer_->beginGame(pcBlack, whitePlayer_->name(), profile_.blackClock);
   //
   game_.start();
   //
   addPositionOccurrence(game_.position()); // this is redundant if starting position is default
                                             // but important if game is started from a custom position
   //
   counter_.restart();
   //
   clockUpdateTimer_.setInterval(cClockUpdateInterval);
   clockUpdateTimer_.setSingleShot(false);
   clockUpdateTimer_.start();
   //
   g_localChessGui.updatePosition(game_.position(), game_.lastMove(), game_.possibleMoves());
   //
   requestMove(whitePlayer_);
   //
}

QString GameSession::getResultMessage(GameSessionEndReason reason, ChessGameResult result)
{
    switch(reason)
    {
       case reasonWhitePlayerIsNotReady:
          return g_msg("PlayerIsNotReady").arg(whitePlayer_->name());
       case reasonBlackPlayerIsNotReady:
          return g_msg("PlayerIsNotReady").arg(blackPlayer_->name());
       case reasonBothPlayersIsNotReady:
          return g_msg("PlayerIsNotReady").arg(whitePlayer_->name()) + QString(", ");
          return g_msg("PlayerIsNotReady").arg(blackPlayer_->name());
       case reasonGameAborted:
          return g_msg("GameAborted");
       case reasonGameFinished:
          return gameResultToMessage(result);
    }
    return QString();
}

void GameSession::endGame(GameSessionEndReason reason, ChessGameResult result)
{
   clockUpdateTimer_.stop();
   //
   if(reason==reasonGameFinished)
   {
      g_localChessGui.showSessionMessage(g_msg("GameFinished"));
   }
   //
   QString resultMsg = getResultMessage(reason, result);
   //
   game_.stop(result, resultMsg);
   //
   whitePlayer_->gameResult(result);
   blackPlayer_->gameResult(result);
   //
   disconnectPlayers();
   disconnectGame();
   //
   if(!resultMsg.isEmpty()) g_localChessGui.showSessionMessage(resultMsg);
   //
   emit end(reason, result, resultMsg);
}

void GameSession::outputLastMove()
{
   g_localChessGui.appendToMoveList(game_.lastSANMove());
}

void GameSession::clockUpdateTimer()
{
   if(game_.position().sideToMove()==pcWhite)
   {
      profile_.whiteClock.remainingTime -= counter_.elapsed();
      if(profile_.whiteClock.remainingTime<=0)
      {
         endGame(reasonGameFinished, resultBlackWonOnTime);
      }
   }
   else
   {
      profile_.blackClock.remainingTime -= counter_.elapsed();
      if(profile_.blackClock.remainingTime<=0)
      {
         endGame(reasonGameFinished, resultWhiteWonOnTime);
      }
   }
   //
   counter_.restart();
}

QString GameSession::pgn() const
{
   return game_.toPGN();
}

unsigned GameSession::addPositionOccurrence(const ChessPosition& position)
{
   std::string pos_str = position.toString();
   std::map<std::string, unsigned>::iterator it = positionOccurrences_.find(pos_str);
   if(it==positionOccurrences_.end())
   {
      positionOccurrences_.insert(it, std::make_pair(pos_str, 1));
      return 1;
   }
   else
   {
      ++it->second;
      return it->second;
   }
}

bool GameSession::checkRepetition()
{
   return addPositionOccurrence(game_.position())>=3;
}

void GameSession::white_requestsTakeback()
{
   if(game_.position().moveNumber()>1)
   {
      ChessCoord prevCursorPos;
      unsigned nmoves = game_.moves().size();
      if(nmoves>=2)
         prevCursorPos = game_.moves()[nmoves-2].from;
      //
      bool accept = false;
      blackPlayer_->opponentRequestsTakeback(accept);
      //
      if(accept)
      {
         game_.takebackOneFullMove();
         g_localChessGui.dropLastFullMove();
         g_localChessGui.setInitialMoveCursorPos(prevCursorPos);
         g_localChessGui.updatePosition(game_.position(),
            game_.lastMove(), game_.possibleMoves());
      }
   }
   requestMove(whitePlayer_);
}

void GameSession::white_requestsAbort()
{
   bool accepted = true;
   if(game_.position().moveNumber()>1)
      blackPlayer_->opponentRequestsAbort(accepted);
   if(accepted)
   {
      endGame(reasonGameAborted, resultNone);
   }
   else
   {
      requestMove(whitePlayer_);
   }
}

void GameSession::black_requestsTakeback()
{
   if(game_.position().moveNumber()>1)
   {
      ChessCoord prevCursorPos;
      unsigned nmoves = game_.moves().size();
      if(nmoves>=2)
         prevCursorPos = game_.moves()[nmoves-2].from;
      //
      bool accept = false;
      whitePlayer_->opponentRequestsTakeback(accept);
      //
      if(accept)
      {
         game_.takebackOneFullMove();
         g_localChessGui.dropLastFullMove();
         g_localChessGui.setInitialMoveCursorPos(prevCursorPos);
         g_localChessGui.updatePosition(game_.position(),
            game_.lastMove(), game_.possibleMoves());
      }
   }
   requestMove(blackPlayer_);
}

void GameSession::black_requestsAbort()
{
   bool accepted = true;
   if(game_.position().moveNumber()>1)
      whitePlayer_->opponentRequestsAbort(accepted);
   if(accepted)
   {
      endGame(reasonGameAborted, resultNone);
   }
   else
   {
      requestMove(blackPlayer_);
   }
}
