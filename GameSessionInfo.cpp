#include "GameSessionInfo.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

namespace
{

bool readIntLine(QTextStream& in, int& value)
{
   QString line = in.readLine();
   bool ok = false;
   value = line.toInt(&ok);
   return ok;
}

}

bool GameSessionInfo::saveToFile(const QString& fileName) const
{
   QFile file(fileName);
   //
   if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
   {
      // could not create game session file
      return false;
   }
   //
   QTextStream out(&file);
   out.setCodec(QTextCodec::codecForName("UTF-8"));
   //
   out << (unsigned)profile.type << "\n";
   //
   out << (profile.whiteClock.untimed ? '0' : '1')
       << (profile.blackClock.untimed ? '0' : '1')
       << "\n";
   //
   out << profile.whiteClock.initialTime << "\n";
   out << profile.whiteClock.remainingTime << "\n";
   out << profile.whiteClock.moveIncrement << "\n";
   out << profile.blackClock.initialTime << "\n";
   out << profile.blackClock.remainingTime << "\n";
   out << profile.blackClock.moveIncrement << "\n";
   out << initialPosition.toString().c_str() << "\n";
   //
   if(!moves.empty())
   {
      for(unsigned i=0; i<moves.size(); ++i)
      {
         out << moves[i].toString().c_str() << "\n";
      }
   }
   //
   return true;
}

bool GameSessionInfo::loadFromFile(const QString& fileName)
{
   QFile file(fileName);
   //
   if(!file.open(QIODevice::ReadOnly)) return false;
   //
   QTextStream in(&file);
   in.setCodec(QTextCodec::codecForName("UTF-8"));
   //
   int stype;
   if(!readIntLine(in, stype)) return false;
   profile.type = (GameType)stype;
   //
   QString s = in.readLine();
   //
   profile.whiteClock.untimed = s.length()<1 || s[0]!='1';
   profile.blackClock.untimed = s.length()<2 || s[1]!='1';
   //
   if(!readIntLine(in, profile.whiteClock.initialTime)) return false;
   if(!readIntLine(in, profile.whiteClock.remainingTime)) return false;
   if(!readIntLine(in, profile.whiteClock.moveIncrement)) return false;
   if(!readIntLine(in, profile.blackClock.initialTime)) return false;
   if(!readIntLine(in, profile.blackClock.remainingTime)) return false;
   if(!readIntLine(in, profile.blackClock.moveIncrement)) return false;
   //
   QString fen = in.readLine();
   //
   initialPosition = ChessPosition::fromString(fen.toStdString());
   if(initialPosition.isEmpty()) return false;
   //
   initialPosition.setChess960(fen!=cStandardInitialFen);
   //
   moves.clear();
   //
   while(true)
   {
      QString move_str = in.readLine();
      if(move_str.isEmpty()) break;
      moves.push_back(ChessMove::fromString(move_str.toStdString()));
   }
   //
   return true;
}
