#ifndef __EngineInfo_h
#define __EngineInfo_h

#include <QStringList>

enum EngineType { etDetect, etUCI, etXBoard };

struct EngineInfo
{
   QString name;
   QString exePath;
   EngineType type;
   std::map<QString, QStringList> startupCommands; // for various profiles
   QStringList profileNames;
   QStringList cleanUpMasks;  // which files to clean up between sessions
   QString commandStandard; // command to switch engine from chess variant to standard chess
   QString command960; // command to switch the engine into playing chess 960
   //
   EngineInfo() : type(etDetect) {}
   bool supports960() const { return !command960.isEmpty(); }
};


#endif
