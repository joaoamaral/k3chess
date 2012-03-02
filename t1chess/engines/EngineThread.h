#ifndef ENGINETHREAD_H
#define ENGINETHREAD_H

#include <QThread>

class EngineThread : public QThread
{
   Q_OBJECT
public:
   EngineThread(QObject *parent) : QThread(parent) {}
   virtual void run();
};

#endif // ENGINETHREAD_H
