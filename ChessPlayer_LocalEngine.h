#ifndef __ChessPlayer_LocalEngine_h
#define __ChessPlayer_LocalEngine_h

#include "ChessPlayer.h"
#include "EngineInfo.h"

#include <QProcess>
#include <QTimer>

class ChessPlayer_LocalEngine : public ChessPlayer
{
   Q_OBJECT

public:
   ChessPlayer_LocalEngine(const EngineInfo& info);
   virtual ~ChessPlayer_LocalEngine();

   virtual void getReady();
   virtual void beginGame(PieceColor color, const QString& opponentName,
                          const ChessClock& clock);
   virtual void makeMove(const ChessPosition& position,
                         const ChessMove& lastMove,
                         const ChessClock& whiteClock,
                         const ChessClock& blackClock);

   virtual void setInitialPosition(const ChessPosition& position);
   virtual void replayMove(const ChessMove& move);

   virtual void illegalMove();
   virtual void opponentRequestsTakeback(bool& accept);
   virtual void opponentRequestsAbort(bool& accept);

   virtual void opponentOffersDraw();
   virtual void opponentAcceptsDraw();

   virtual void gameResult(ChessGameResult result);

   virtual bool setChess960(bool value);

   const EngineInfo& info() const;

   void ponderingChanged(); // can be called manually to set/clear pondering mode
                            // when the corresponding GUI option changes
signals:
   void engineProcessError(QProcess::ProcessError error);

private slots:
   void engineStarted();
   void engineHasOutput();
   void uciokTimeout();

private:
   void tellEngine(const std::string& str);
   void processEngineResponse(const std::string& str);
   void setUCIPonderOption(); // sets UCI engine Ponder option according to current GUI preference
   void engineTypeDetected(bool updateEngineIni=false);
   void performCleanup();

private:
   EngineInfo info_;
   QString opponentName_;
   QProcess engineProcess_;
   bool readyRequest_;
   EngineType type_;
   QTimer uciokTimer_;
   std::string incompleteLine_;
   bool inForceMode_;   // (force mode is defined for XBoard engines only)
};

#endif
