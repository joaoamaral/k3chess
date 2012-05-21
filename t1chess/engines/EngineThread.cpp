#include "EngineThread.h"
#include "toga2/src/toga2_main.h"

void EngineThread::run()
{
    toga2_main();
}

void EngineThread::setEngineDir(const QString& engineDir)
{
    g_toga2EngineDir = engineDir;
}
