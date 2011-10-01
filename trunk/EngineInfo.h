#ifndef __EngineInfo_h
#define __EngineInfo_h

#include <QString>

enum EngineType { etDetect, etUCI, etXBoard };

struct EngineInfo
{
   QString name;
   QString exePath;
   EngineType type;
   //
   EngineInfo() : type(etDetect) {}
};


#endif
