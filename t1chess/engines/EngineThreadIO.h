#ifndef ENGINETHREADIO_H
#define ENGINETHREADIO_H

#include <QObject>
#include <QEvent>
#include <QReadWriteLock>

const QEvent::Type EngineRequest = (QEvent::Type)(QEvent::User+1);
const QEvent::Type EngineReponse = (QEvent::Type)(QEvent::User+2);

class EngineRequestEvent : public QEvent
{
public:
   EngineRequestEvent() : QEvent(EngineRequest) {}
};

class EngineResponseEvent : public QEvent
{
public:
   EngineResponseEvent() : QEvent(EngineReponse) {}
};

class EngineThreadIO : public QObject
{
   Q_OBJECT
public:
   EngineThreadIO(QObject *parent=0) : QObject(parent),
      inputListener_(0), outputListener_(0) {}
   //
   void setInputListener(QObject*);
   void setOutputListener(QObject*);
   //
   bool has_input(); // no wait
   std::string get_input(); // no wait, returns empty string if no input
   std::string get_output(); // no wait, returns empty string if no output
   void send_input(std::string str); // send moves and commands to engine thread
   void send_output(std::string str); // sends engine output to main thread
private:
   std::list<std::string> input_queue;
   std::list<std::string> output_queue;
   //
   QReadWriteLock input_guard;
   QReadWriteLock output_guard;
   //
   QObject *inputListener_;
   QObject *outputListener_;
};

extern EngineThreadIO g_engineIO;

#endif // ENGINETHREADIO_H
