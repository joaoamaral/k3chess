#ifndef __EngineInfo_h
#define __EngineInfo_h

#include <QStringList>

enum EngineType { etDetect, etUCI, etXBoard };

struct EngineInfo
{
   QString name;
   QString exePath;
   EngineType type;
   QStringList startupCommands;
   QStringList cleanUpMasks;  // which files to clean up between sessions
   //
   EngineInfo() : type(etDetect) {}
};


#endif
