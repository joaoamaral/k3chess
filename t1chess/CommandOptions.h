#ifndef __CommandOptions_h
#define __CommandOptions_h

#include <QString>
#include <list>

enum StringResolveType { srtDirect, srtMenuVar };

struct CommandOption
{
   int id;
   Qt::Key key;
   bool enabled;
   //
   CommandOption() : id(0), key(Qt::Key_unknown), enabled(true), srt_(srtDirect) {}
   CommandOption(int i, const QString& text, StringResolveType srt, Qt::Key k) :
      id(i), key(k), enabled(true), srt_(srt), text_(text) {}
   //
   QString text() const;
   //
   bool isSelectable() const { return id>0; } // use negative id's for non-selectable (static) items
                                              // id 0 is reserved for returning 'no id'
private:
   StringResolveType srt_;
   QString text_;
};

class CommandOptions
{
public:
   int idFromIndex(int index) const; // returns 0 if illegal index
   CommandOption *findItemById(int id); // returns 0 if not found
   CommandOption *findItemByKey(Qt::Key key); // returns 0 if not found
   void add(const CommandOption& item);
   void addStaticText(const QString& text);
   const std::list<CommandOption>& items() const { return items_; }
   void clear();
   bool containSelectableOptions() const; // returns true if at least one options is selectable
   void enable(int id);
   void disable(int id);
private:
   std::list<CommandOption> items_;
};

#endif
