#include "ChessPlayer_LocalHuman.h"
#include "LocalChessGui.h"
#include "CommandOptionDefs.h"

/* Local Human Player
   ==================
   Played by user, using the local GUI.
*/

ChessPlayer_LocalHuman::ChessPlayer_LocalHuman(const QString& name)
   : ChessPlayer(name), showingIngameOptions_(false)
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
   position_ = position;
   //
   QObject::connect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this,
                    SLOT(userMoves(const CoordPair&)), Qt::UniqueConnection);
   // connect to keyboard events to receive MENU key press
   // (for the in-game menu)
   QObject::connect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key)), this,
                    SLOT(keyPressed(Qt::Key)), Qt::UniqueConnection);
   //
   g_localChessGui.beginMoveSelection();
}

void ChessPlayer_LocalHuman::offerChoice(const CommandOptions& options)
{
   QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)));
   QObject::connect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)), Qt::UniqueConnection);
   g_localChessGui.offerChoice(options);
}

void ChessPlayer_LocalHuman::userChoice(int id)
{
   showingIngameOptions_ = false;
   QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)));
   QObject::disconnect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this, SLOT(userMoves(const CoordPair&)));
   //
   switch(id)
   {
      case cmd_Promotion_Queen:   emit playerMoves(ChessMove(promotionMove_, ptQueen));   break;
      case cmd_Promotion_Rook:    emit playerMoves(ChessMove(promotionMove_, ptRook));    break;
      case cmd_Promotion_Bishop:  emit playerMoves(ChessMove(promotionMove_, ptBishop));  break;
      case cmd_Promotion_Knight:  emit playerMoves(ChessMove(promotionMove_, ptKnight));  break;
      //
      case cmd_InGame_Takeback:   emit playerRequestsTakeback(); break;
      case cmd_InGame_OfferDraw:  emit playerOffersDraw();
         // resume move selection
         QObject::connect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this,
                          SLOT(userMoves(const CoordPair&)), Qt::UniqueConnection);
         break;
      case cmd_InGame_Resign:     emit playerResigns();  break;
      case cmd_InGame_Abort:      emit playerRequestsAbort(); break;
   }
}

void ChessPlayer_LocalHuman::userMoves(const CoordPair &move)
{
   QObject::disconnect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this, SLOT(userMoves(const CoordPair&)));
   //
   if(g_chessRules.isPromotionMove(position_, move))
   {
      promotionMove_ = move;
      offerChoice(g_commandOptionDefs.promotionOptions());
   }
   else
   {
      QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)));
      QObject::disconnect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key)), this, SLOT(keyPressed(Qt::Key)));
      //
      emit playerMoves(ChessMove(move));
   }
}

void ChessPlayer_LocalHuman::opponentOffersDraw()
{
   // @@note: to accept draw send a draw offer to your opponent
   // instead of making a move
}

void ChessPlayer_LocalHuman::keyPressed(Qt::Key key)
{
   switch(key)
   {
      case Qt::Key_Cancel:
      case Qt::Key_Escape:
      case Qt::Key_Menu:
         if(showingIngameOptions_)
         {
            showingIngameOptions_ = false;
            g_localChessGui.beginMoveSelection();
         }
         else
         {
            showingIngameOptions_ = true;
            offerChoice(g_commandOptionDefs.inGameOptions());
         }
         break;
      default:
         break;
   }
}

void ChessPlayer_LocalHuman::gameResult(ChessGameResult result)
{
   showingIngameOptions_ = false;
   //
   QObject::disconnect(&g_localChessGui, SIGNAL(userMoves(const CoordPair&)), this, SLOT(userMoves(const CoordPair&)));
   QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)));
   QObject::disconnect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key)), this, SLOT(keyPressed(Qt::Key)));
}

void ChessPlayer_LocalHuman::opponentRequestsTakeback(bool &accept)
{
   accept = true;
}

void ChessPlayer_LocalHuman::opponentRequestsAbort(bool &accept)
{
   accept = true;
}

void ChessPlayer_LocalHuman::illegalMove(const std::string &move_str)
{
}

