#include "ChessPlayer_LocalEngine.h"
#include "LocalChessGui.h"
#include "StringUtils.h"
#include "GlobalStrings.h"
#include "Settings.h"

#include <fstream>

namespace
{

enum TalkDirection { TO_ENGINE, FROM_ENGINE };

void logEngineTalk(TalkDirection direction, const std::string& msg)
{
   QTime time = QTime::currentTime();
   //
   static unsigned nCalls = 0;
   static bool noLog = false;
   if(noLog) return;
   std::ofstream out("./logs/engine_talk.log", nCalls==0 ? std::ios::trunc : std::ios::app);
   noLog = out.fail(); // will fail if there is no 'logs' folder
   if(noLog) return;
   //
   out << time.toString("HH:mm:ss.zzz").toStdString() << " "
       << (direction==TO_ENGINE ? "-->" : "<--") << " " << msg << std::endl;
   ++nCalls;
}

}

const int cUciokTimeoutMs = 5000; // milliseconds to wait for 'uciok'


ChessPlayer_LocalEngine::ChessPlayer_LocalEngine(const EngineInfo& info) :
   ChessPlayer(info.name), engineProcess_(this),
   readyRequest_(false), info_(info), inForceModeAfterTakeback_(false)
{
   QObject::connect(&engineProcess_, SIGNAL(started()),
                    this, SLOT(engineStarted()), Qt::UniqueConnection);
   QObject::connect(&engineProcess_, SIGNAL(error(QProcess::ProcessError)),
                    this, SIGNAL(engineProcessError(QProcess::ProcessError)),
                    Qt::UniqueConnection);
   QObject::connect(&engineProcess_, SIGNAL(readyRead()),
                    this, SLOT(engineHasOutput()), Qt::UniqueConnection);
   //
   QString workDir = extractFolderPath(info_.exePath);
   engineProcess_.setWorkingDirectory(workDir);
   engineProcess_.start(info_.exePath);
}

void ChessPlayer_LocalEngine::getReady()
{
   if(engineProcess_.state()==QProcess::Running && info_.type!=etDetect)
   {
      readyRequest_ = false;
      emit isReady();
   }
   else
   {
      readyRequest_ = true;
   }
}

void ChessPlayer_LocalEngine::engineStarted()
{
   switch(info_.type)
   {
      case etXBoard:
         tellEngine("xboard"); // required by some engines
         break;
      case etDetect:
         // try to detect engine type by sending the "uci" command
         // and waiting for "uciok" response
         QObject::connect(&uciokTimer_, SIGNAL(timeout()), this,
                          SLOT(uciokTimeout()), Qt::UniqueConnection);
         tellEngine("uci");
         //
         uciokTimer_.setSingleShot(true);
         uciokTimer_.start(cUciokTimeoutMs);
         break;
      default:
         break;
   }
}

void ChessPlayer_LocalEngine::uciokTimeout()
{
   QObject::disconnect(&uciokTimer_, SIGNAL(timeout()), this,
                       SLOT(uciokTimeout()));
   //
   if(info_.type!=etDetect)
      return; // type already detected, ignore this event
   //
   // uciok timed out, so consider this an XBoard-compatible engine
   //
   info_.type = etXBoard;
   tellEngine("xboard"); // required by some engines
   //
   if(readyRequest_)
   {
      readyRequest_ = false;
      emit isReady();
   }
}

std::string clockToXBoardLevel(const ChessClock& clock)
{
   std::string s;
   s.reserve(32);
   s.append("level ");
   s.append("0 ");
   if(clock.initialTime<60000)
   {
      s.append("0:");
      s.append(uintToStr(clock.initialTime/1000));
   }
   else
   {
      s.append(uintToStr(clock.initialTime/60000));
   }
   s.append(" ");
   s.append(uintToStr(clock.moveIncrement/1000));
   //
   return s;
}

void ChessPlayer_LocalEngine::beginGame(PieceColor color,
   const QString& opponentName, const ChessClock& clock)
{
   opponentName_ = opponentName;
   //
   switch(info_.type)
   {
      case etUCI:
         tellEngine("ucinewgame");
         break;
      case etXBoard:
         tellEngine("new");
         if(g_settings.canPonder())
         {
            tellEngine("hard");
         }
         else
         {
            tellEngine("easy");
         }
         tellEngine("nopost");
         tellEngine(clockToXBoardLevel(clock));
         break;
      case etDetect:
         assert(false);
         break;
   }
}

void ChessPlayer_LocalEngine::makeMove(const ChessPosition& position,
                                       const ChessMove& lastMove,
                                       const ChessClock& whiteClock,
                                       const ChessClock& blackClock)
{
   g_localChessGui.showStaticMessage(g_msg("WaitingForPlayerToMove").arg(name()));
   //
   switch(info_.type)
   {
      case etUCI:
         {
            std::string cmd;
            cmd.reserve(256);
            cmd.append("position fen ");
            cmd.append(position.toString());
            tellEngine(cmd);
            cmd.clear();
            cmd.append("go ");
            cmd.append("wtime ");
            cmd.append(uintToStr(whiteClock.remainingTime));
            cmd.append(" btime ");
            cmd.append(uintToStr(blackClock.remainingTime));
            cmd.append(" winc ");
            cmd.append(uintToStr(whiteClock.moveIncrement));
            cmd.append(" binc ");
            cmd.append(uintToStr(blackClock.moveIncrement));
            tellEngine(cmd);
         }
         break;
      case etXBoard:
         {
            int engineTime = position.sideToMove()==pcWhite ? whiteClock.remainingTime : blackClock.remainingTime;
            int opponentTime = position.sideToMove()==pcWhite ? blackClock.remainingTime : whiteClock.remainingTime;
            //
            std::string cmd;
            cmd.reserve(40);
            cmd.append("time ");
            cmd.append(uintToStr(engineTime/10));
            tellEngine(cmd);
            cmd.clear();
            cmd.append("otim ");
            cmd.append(uintToStr(opponentTime/10));
            tellEngine(cmd);
            //
            if(lastMove.assigned())
            {
               tellEngine(lastMove.toString());
               if(inForceModeAfterTakeback_)
               {
                  inForceModeAfterTakeback_ = false;
                  tellEngine("go");
               }
            }
            else
            {
               if(position.sideToMove()==pcWhite)
               {
                  tellEngine("white");
               }
               else
               {
                  tellEngine("black");
               }
               //
               tellEngine("go");
            }
         }
         break;
      case etDetect:
         assert(false);
         break;
   }
}

void ChessPlayer_LocalEngine::opponentOffersDraw()
{
   tellEngine("draw");
}

void ChessPlayer_LocalEngine::opponentAcceptsDraw()
{
   tellEngine("draw");
}

void ChessPlayer_LocalEngine::gameResult(ChessGameResult result)
{
   switch(info_.type)
   {
      case etUCI:
         tellEngine("stop"); // just in case, to avoid pondering after the game is over
         break;
      case etXBoard:
         // tell engine to forget about the previous game and prepare for a new game
         inForceModeAfterTakeback_ = false;
         tellEngine("force");
         break;
      case etDetect:
         assert(false);
         break;
   }
}

void ChessPlayer_LocalEngine::tellEngine(const std::string& str)
{
   logEngineTalk(TO_ENGINE, str);
   //
   engineProcess_.write(str.c_str(), str.length());
   engineProcess_.write("\n", 1);
}

void ChessPlayer_LocalEngine::processEngineResponse(const std::string& str)
{
   if(str.empty()) return;
   //
   logEngineTalk(FROM_ENGINE, str);
   //
   switch(info_.type)
   {
      case etUCI:
         if(str.find("bestmove ")==0)
         {
            size_t pos = str.find(' ', 9);
            if(pos==std::string::npos) pos = str.length();
            std::string move_str = str.substr(9, pos-9);
            emit playerMoves(move_str);
         }
         /* no draw/resign options for UCI engines*/
         break;
      case etXBoard:
         if(startsWith(str, "move "))
         {
            std::string move_str = trim(str.substr(5));
            emit playerMoves(move_str);
         }
         else if(str.find('{')!=std::string::npos)
         {
            // engine detected end-of-game, but it must be detected
            // by game and/org game session
            int x =0;
            ++x;
         }
         else if(startsWith(str, "offer draw"))
         {
            emit playerOffersDraw(); // engine offered a draw
         }
         else if(startsWith(str, "resign"))
         {
            emit playerResigns(); // engine resigned
         }
         break;
      case etDetect:
         if(str.find("uciok")==0)
         {
            info_.type = etUCI;
            setUCIPonderOption();
            //
            if(readyRequest_)
            {
               readyRequest_ = false;
               emit isReady();
            }
         }
         break;
   }
}

void ChessPlayer_LocalEngine::engineHasOutput()
{
   const unsigned cBufferSize = 1024;
   char buffer[cBufferSize];
   //
   std::string line;
   line.reserve(256);
   line.append(incompleteLine_);
   //
   char ignoredChar = 0x00;
   //
   while(true)
   {
      unsigned nBytesRead = engineProcess_.read(buffer, cBufferSize);
      if(nBytesRead==0) break;
      for(unsigned i=0; i<nBytesRead; ++i)
      {
         char c = buffer[i];
         if(c==ignoredChar) continue;
         switch(c)
         {
            case 0x0D:
               processEngineResponse(line);
               line.clear();
               ignoredChar = 0x0A;
               break;
            case 0x0A:
               processEngineResponse(line);
               line.clear();
               ignoredChar = 0x0D;
               break;
            default:
               line.push_back(c);
               break;
         }
      }
      if(nBytesRead<cBufferSize) break;
   }
   incompleteLine_ = line;
}

void ChessPlayer_LocalEngine::ponderingChanged()
{
   if(info_.type!=etUCI) return;
   //
   setUCIPonderOption();
}

void ChessPlayer_LocalEngine::setUCIPonderOption()
{
   if(g_settings.canPonder())
   {
      tellEngine("setoption name Ponder value true");
   }
   else
   {
      tellEngine("setoption name Ponder value false");
   }
}

void ChessPlayer_LocalEngine::opponentRequestsTakeback(bool &accept)
{
   accept = true;
   switch(info_.type)
   {
      case etUCI:
         // nothing is to be done
         // on the next engine move the previous position will be sent
         // to the engine
         break;
      case etXBoard:
         tellEngine("force");
         tellEngine("undo");
         tellEngine("undo");
         inForceModeAfterTakeback_ = true;
         break;
      default:
         break;
   }
}

void ChessPlayer_LocalEngine::opponentRequestsAbort(bool &accept)
{
   accept = true;
}
