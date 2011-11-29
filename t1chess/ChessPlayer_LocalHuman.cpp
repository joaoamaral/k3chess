#include "ChessPlayer_LocalHuman.h"
#include "LocalChessGui.h"
#include "CommandOptionDefs.h"

/* Local Human Player
   ==================
   Played by user, using the local GUI.
*/

ChessPlayer_LocalHuman::ChessPlayer_LocalHuman(const QString& name)
   : ChessPlayer(name), mode_(modeStandby)
{
}

void ChessPlayer_LocalHuman::getReady()
{
   emit isReady(); // local human player is considered ready
}

void ChessPlayer_LocalHuman::beginGame(PieceColor color,
   const QString& opponentName, const ChessClock& clock)
{
   opponentName_ = opponentName;
   position_ = ChessPosition();
}

void ChessPlayer_LocalHuman::makeMove(const ChessPosition& position,
                                      const ChessMove& lastMove,
                                      const ChessClock& whiteClock,
                                      const ChessClock& blackClock)
{
   mode_ = modeSelectingMove;
   //
   position_ = position;
   //
   QObject::connect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this,
                    SLOT(userMoves(const CoordPair&)), Qt::UniqueConnection);
   // connect to keyboard events to receive MENU key press
   // (for the in-game menu)
   QObject::connect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key, Qt::KeyboardModifiers)), this,
                    SLOT(keyPressed(Qt::Key, Qt::KeyboardModifiers)), Qt::UniqueConnection);
   //
   g_localChessGui.beginMoveSelection();
}

void ChessPlayer_LocalHuman::offerChoice(const CommandOptions& options)
{
   QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)));
   QObject::connect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)), Qt::UniqueConnection);
   g_localChessGui.switchToCommandView();
   g_localChessGui.offerChoice(options);
}

void ChessPlayer_LocalHuman::userChoice(int id)
{
   QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)));
   QObject::disconnect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this, SLOT(userMoves(const CoordPair&)));
   //
   mode_ = modeStandby;
   //
   switch(id)
   {
      case cmd_Promotion_Queen:  emit playerMoves(ChessMove(promotionMove_, ptQueen)); break;
      case cmd_Promotion_Rook:   emit playerMoves(ChessMove(promotionMove_, ptRook)); break;
      case cmd_Promotion_Bishop: emit playerMoves(ChessMove(promotionMove_, ptBishop)); break;
      case cmd_Promotion_Knight: emit playerMoves(ChessMove(promotionMove_, ptKnight)); break;
      case cmd_InGame_Takeback:  emit playerRequestsTakeback(); break;
      case cmd_InGame_OfferDraw: emit playerOffersDraw();
         // resume move selection
         mode_ = modeSelectingMove;
         QObject::connect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this,
                          SLOT(userMoves(const CoordPair&)), Qt::UniqueConnection);
         g_localChessGui.beginMoveSelection();
         break;
      case cmd_InGame_Resign:     emit playerResigns();  break;
      case cmd_InGame_Abort:      emit playerRequestsAbort();  break;
   }
}

void ChessPlayer_LocalHuman::userMoves(const CoordPair &move)
{
   QObject::disconnect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this, SLOT(userMoves(const CoordPair&)));
   //
   if(g_chessRules.isPromotionMove(position_, move))
   {
      mode_ = modeSelectingPromotion;
      promotionMove_ = move;
      offerChoice(g_commandOptionDefs.promotionOptions());
   }
   else
   {
      mode_ = modeStandby;
      QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)));
      QObject::disconnect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key, Qt::KeyboardModifiers)),
                                                   this, SLOT(keyPressed(Qt::Key, Qt::KeyboardModifiers)));
      //
      g_localChessGui.switchToClockView();
      //
      emit playerMoves(ChessMove(move));
   }
}

void ChessPlayer_LocalHuman::opponentOffersDraw()
{
   // @@note: to accept draw send a draw offer to your opponent
   // instead of making a move
}

void ChessPlayer_LocalHuman::keyPressed(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
   switch(key)
   {
      case Qt::Key_Cancel:
      case Qt::Key_Escape:
      case Qt::Key_Menu:
         switch(mode_)
         {
            case modeShowingInGameOptions:
               mode_ = modeSelectingMove;
               g_localChessGui.beginMoveSelection();
               break;
            case modeSelectingMove:
               mode_ = modeShowingInGameOptions;
               offerChoice(g_commandOptionDefs.inGameOptions());
               break;
            default:
               break;
         }
         break;
      default:
         break;
   }
}

void ChessPlayer_LocalHuman::gameResult(ChessGameResult result)
{
   mode_ = modeStandby;
   //
   QObject::disconnect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this, SLOT(userMoves(const CoordPair&)));
   QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)));
   QObject::disconnect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key, Qt::KeyboardModifiers)),
                       this, SLOT(keyPressed(Qt::Key, Qt::KeyboardModifiers)));
}

void ChessPlayer_LocalHuman::opponentRequestsTakeback(bool &accept)
{
   accept = true;
}

void ChessPlayer_LocalHuman::opponentRequestsAbort(bool &accept)
{
   accept = true;
}

bool ChessPlayer_LocalHuman::setChess960(bool value)
{
   return true; // human player is assumed to support chess 960 games
}

