#ifndef __KeyMapper_h
#define __KeyMapper_h

#include <QString>
#include "Singletons.h"

#include <map>
#include <vector>

struct KeyDef
{
   QString keyName;
   Qt::Key qtKey;
   //
   KeyDef() : qtKey(Qt::Key_unknown) {}
   KeyDef(const char *n, Qt::Key k) : keyName(n), qtKey(k) {}
};

class KeyMapper
{
   friend void Singletons::initialize();
   friend void Singletons::finalize();

   KeyMapper();
   ~KeyMapper();

public:
   bool fileNotFound() const;
   void saveKeyMapping();

   Qt::Key getQtKey(int rawKey) const;
   void clearMapping();
   void addMappedKey(int rawKey, Qt::Key key);

   const std::vector<KeyDef>& keyDefs() const;

private:
   void initKeyDefs();
   void loadKeyMapping();
   QString findMappableKeyName(Qt::Key qtKey);

private:
   std::map<int, Qt::Key> keymap_;
   std::vector<KeyDef> keyDefs_;
   bool fileNotFound_;
   bool modified_;
};

#endif
