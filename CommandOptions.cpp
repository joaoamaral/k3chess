#include "CommandOptions.h"
#include "GlobalStrings.h"

QString CommandOption::text() const
{
   return srt_==srtMenuVar ? g_menu(text_) : text_;
}

CommandOption *CommandOptions::findItemByKey(Qt::Key key)
{
   std::list<CommandOption>::iterator it = items_.begin(), itEnd = items_.end();
   for(;it!=itEnd;++it)
   {
      if(it->key==key) return &(*it);
   }
   return 0;
}

CommandOption *CommandOptions::findItemById(int id)
{
   std::list<CommandOption>::iterator it = items_.begin(), itEnd = items_.end();
   for(;it!=itEnd;++it)
   {
      if(it->id==id) return &(*it);
   }
   return 0;
}

int CommandOptions::idFromIndex(int index) const
{
   std::list<CommandOption>::const_iterator it = items_.begin(), itEnd = items_.end();
   for(int i=0;it!=itEnd;++it,++i)
   {
      if(i>index) return 0;
      if(i==index)
      {
         return it->id;
      }
   }
   return 0;
}

bool CommandOptions::containSelectableOptions() const
{
   std::list<CommandOption>::const_iterator it = items_.begin(), itEnd = items_.end();
   for(;it!=itEnd;++it)
   {
      if(it->isSelectable())
         return true;
   }
   return false;
}

void CommandOptions::add(const CommandOption& item)
{
   items_.push_back(item);
}

void CommandOptions::addStaticText(const QString& text)
{
   CommandOption staticText(-1, text, srtDirect, Qt::Key_unknown);
   items_.push_back(staticText);
}

void CommandOptions::clear()
{
   items_.clear();
}

void CommandOptions::enable(int id)
{
   CommandOption *option = findItemById(id);
   if(option) option->enabled = true;
}

void CommandOptions::disable(int id)
{
   CommandOption *option = findItemById(id);
   if(option) option->enabled = false;
}
