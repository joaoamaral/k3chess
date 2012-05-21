#include "KeyMapper.h"
#include "Settings.h"
#include <QTextCodec>

KeyMapper::KeyMapper() : fileNotFound_(false), modified_(false)
{
   initKeyDefs();
   loadKeyMapping(); // try to load, if it exists
}

KeyMapper::~KeyMapper()
{
   saveKeyMapping(); // only if modified
}

bool KeyMapper::fileNotFound() const
{
   return fileNotFound_;
}

Qt::Key KeyMapper::getQtKey(int rawKey) const
{
   std::map<int, Qt::Key>::const_iterator it = keymap_.find(rawKey);
   if(it==keymap_.end()) return Qt::Key(rawKey);
   else return it->second;
}

const std::vector<KeyDef>& KeyMapper::keyDefs() const
{
   return keyDefs_;
}

void KeyMapper::initKeyDefs()
{
//   keyDefs_.reserve(2); // (8)
   //
//   keyDefs_.push_back(KeyDef("Left", Qt::Key_Left));
//   keyDefs_.push_back(KeyDef("Right", Qt::Key_Right));
//   keyDefs_.push_back(KeyDef("Up", Qt::Key_Up));
//   keyDefs_.push_back(KeyDef("Down", Qt::Key_Down));
//   keyDefs_.push_back(KeyDef("Select", Qt::Key_Select));
//   keyDefs_.push_back(KeyDef("Menu", Qt::Key_Menu));
//   keyDefs_.push_back(KeyDef("Refresh", Qt::Key_Refresh));
}

void KeyMapper::loadKeyMapping()
{
   QString keyFileName = g_settings.keymapFile();
   QFileInfo fi(keyFileName);
   fileNotFound_ = !fi.exists();
   //
   if(fileNotFound_) return;
   //
   QSettings ini(keyFileName, QSettings::IniFormat);
   ini.setIniCodec(QTextCodec::codecForName("UTF-8"));
   //
   ini.beginGroup("KeyMapping");
   QStringList keys = ini.childKeys();
   foreach(QString key, keys)
   {  //
      // @@note: multiple keys can be mapped onto one logical key
      // e.g. Select1=13   Select2=32  - Return and Space will both be used as "Select" key
      //
      int value = ini.value(key, 0).toInt();
      //
      std::vector<KeyDef>::const_iterator
            it=keyDefs_.begin(), itEnd = keyDefs_.end();
      for(;it!=itEnd;++it)
      {
         if(key.startsWith(it->keyName))
         {
            keymap_.insert(std::make_pair(value, it->qtKey));
            break;
         }
      }
   }
   ini.endGroup();
}

void KeyMapper::clearMapping()
{
   keymap_.clear();
   modified_ = true;
}

void KeyMapper::addMappedKey(int rawKey, Qt::Key key)
{
   keymap_.insert(std::make_pair(rawKey, key));
   modified_ = true;
}

QString KeyMapper::findMappableKeyName(Qt::Key qtKey)
{
   std::vector<KeyDef>::const_iterator
         it=keyDefs_.begin(), itEnd = keyDefs_.end();
   for(;it!=itEnd;++it)
   {
      if(it->qtKey==qtKey)
      {
         return it->keyName;
      }
   }
   return QString();
}

void KeyMapper::saveKeyMapping()
{
   if(!modified_) return;
   //
   QSettings ini(g_settings.keymapFile(), QSettings::IniFormat);
   ini.setIniCodec(QTextCodec::codecForName("UTF-8"));
   ini.beginGroup("KeyMapping");
   //
   std::map<int, Qt::Key>::const_iterator it = keymap_.begin(),
                                          itEnd = keymap_.end();
   for(;it!=itEnd;++it)
   {
      QString keyName = findMappableKeyName(it->second);
      ini.setValue(keyName, it->first);
   }
   ini.endGroup();
   ini.sync();
   //
   modified_ = false;
}
