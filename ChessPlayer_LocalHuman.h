#ifndef __ChessPlayer_LocalHuman_h
#define __ChessPlayer_LocalHuman_h

#include "ChessPlayer.h"
#include "CommandOptions.h"

class ChessPlayer_LocalHuman : public ChessPlayer
{
   Q_OBJECT
public:
   ChessPlayer_LocalHuman(const QString& name);

   virtual void getReady();
   virtual void beginGame(PieceColor color, const QString& opponentName,
                          const ChessClock& clock);
   virtual void makeMove(const ChessPosition& position,
                         const ChessMove& lastMove,
                         const ChessClock& whiteClock,
                         const ChessClock& blackClock);

   virtual void illegalMove(const std::string &move_str);
   virtual void opponentOffersDraw();
   virtual void opponentRequestsTakeback(bool& accept);
   virtual void opponentRequestsAbort(bool& accept);
   virtual void gameResult(ChessGameResult result);

private slots:
   void userMoves(const CoordPair& move); // user entered a move via GUI
   void userChoice(int id);
   void keyPressed(Qt::Key key);

private:
   void offerChoice(const CommandOptions& options);

private:
   QString opponentName_;
   ChessPosition position_;
   CoordPair promotionMove_;
   bool showingIngameOptions_;
};

#endif
