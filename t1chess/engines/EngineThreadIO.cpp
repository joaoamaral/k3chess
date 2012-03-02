#include "EngineThreadIO.h"
#include <QApplication>

EngineThreadIO g_engineIO;

bool EngineThreadIO::has_input()
{
   bool result = false;
   input_guard.lockForRead();
   result = !input_queue.empty();
   input_guard.unlock();
   return result;
}

std::string EngineThreadIO::get_input()
{
   input_guard.lockForWrite();
   std::string s;
   if(!input_queue.empty())
   {
      s = input_queue.front();
      input_queue.pop_front();
   }
   input_guard.unlock();
   return s;
}

std::string EngineThreadIO::get_output()
{
   output_guard.lockForWrite();
   std::string s;
   if(!output_queue.empty())
   {
      s = output_queue.front();
      output_queue.pop_front();
   }
   output_guard.unlock();
   return s;
}

void EngineThreadIO::send_input(std::string str)
{
   input_guard.lockForWrite();
   input_queue.push_back(str);
   input_guard.unlock();
   //
   if(inputListener_)
   {
      QApplication::postEvent(inputListener_, new EngineRequestEvent);
   }
}

void EngineThreadIO::send_output(std::string str)
{
   output_guard.lockForWrite();
   output_queue.push_back(str);
   output_guard.unlock();
   //
   if(outputListener_)
   {
      QApplication::postEvent(outputListener_, new EngineResponseEvent);
   }
}

void EngineThreadIO::setInputListener(QObject *obj)
{
   inputListener_ = obj;
}

void EngineThreadIO::setOutputListener(QObject *obj)
{
   outputListener_ = obj;
}
