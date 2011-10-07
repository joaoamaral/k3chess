#ifndef __ChessPlayer_h
#define __ChessPlayer_h

#include "ChessPosition.h"
#include "ChessGame.h"
#include "GameProfile.h"
#include "ChessMove.h"
#include <QObject>

// virtual chess player base class
class ChessPlayer : public QObject
{
   Q_OBJECT
public:
   ChessPlayer(const QString& a_name) : name_(a_name) {}
   virtual ~ChessPlayer() {}

   const QString& name() const { return name_; }

   // requests (must implement)
   virtual void getReady() = 0; // tells the player to get ready
   virtual void beginGame(PieceColor color, const QString& opponentName,
                          const ChessClock& clock) = 0; // tells player to begin game as white or black
   virtual void setInitialPosition(const ChessPosition& position) = 0; // tells player to set up the given initial position
   virtual void replayMove(const ChessMove& move) = 0; // after beginGame() and before the first makeMove() call,
                                                       // tells the player to replay the given move on his board
                                                       // (this can be either his move or his opponent's move depending
                                                       //  on whose turn it is)
   virtual void makeMove(const ChessPosition& position,
                         const ChessMove& lastMove,
                         const ChessClock& whiteClock,
                         const ChessClock& blackClock) = 0;  // prompts player to make a move in the given position and given last opponent move (if any)

   // notifications (optional processing)
   virtual void illegalMove() {}                          // informs the player that the move he attempted to make is an illegal move (makeMove() call will follow)
   virtual void opponentMoves(const ChessMove& move) {}   // inform player about opponent's next move
   virtual void opponentOffersDraw() {}                   // inform player that his opponent has offered a draw (send draw offer to accept, move to reject)
   virtual void opponentAcceptsDraw() {}                  // inform player that his opponent has accepted the draw offer
   virtual void opponentRejectsDraw() {}                  // inform player that his opponent has rejected the draw offer
   virtual void opponentResigns() {}                      // inform player that his opponent has resigned
   virtual void opponentSays(const QString& msg) {}       // player receives a message from opponent
   virtual void opponentRequestsTakeback(bool& accept);   // default behavior is reject
   virtual void opponentRequestsAbort(bool& accept);      // default behavior is reject
   virtual void gameResult(ChessGameResult result) {}     // notify player that the game was finished with the given result
                                                          // (resultNone means the game was aborted)
   // additional commands
   virtual bool setChess960(bool value); // must try to set mode to Chess960 if value=true, return true if supported, false otherwise, if false is passed, always return true

signals:
   void isReady();                             // player notifies that he is ready to play
   void playerMoves(const ChessMove& move);    // player makes a move
   void playerMoves(const std::string& move);  // player can send moves either in ChessMove struct or in a string (but not both!)
   void playerOffersDraw();                    // player offers a draw (or accepts the draw offer made by opponent)
   void playerResigns();                       // player resigns
   void playerSays(const QString& msg);        // player sends a message to his opponent
   void playerRequestsTakeback();
   void playerRequestsAbort();

private:
   QString name_;
};

inline
void ChessPlayer::opponentRequestsTakeback(bool& accept)
{
   accept = false;
}

inline
void ChessPlayer::opponentRequestsAbort(bool& accept)
{
   accept = false;
}

inline
bool ChessPlayer::setChess960(bool value)
{
   return !value; // there is no chess960 support by default
}

#endif
