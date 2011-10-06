#include "GlobalStrings.h"
#include "Settings.h"

#include <QSettings>
#include <QTextCodec>

class LocaleIniFile
{
public:
   LocaleIniFile(const QString& iniFileName) : ini_(0)
   {
      QFileInfo fi(iniFileName);
      if(fi.exists())
      {
         ini_ = new QSettings(iniFileName, QSettings::IniFormat);
         ini_->setIniCodec(QTextCodec::codecForName("UTF-8"));
      }
   }
   ~LocaleIniFile()
   {
      delete ini_;
   }
   QString getString(const QString& section, const QString& name) const
   {
      if(!ini_) return QString();
      ini_->beginGroup(section);
      QString value = ini_ ? ini_->value(name, QString()).toString() : QString();
      ini_->endGroup();
      return value;
   }

private:
   QSettings *ini_;
};

GlobalStrings::GlobalStrings() : ini_(0)
{
   attachLocale();
   QObject::connect(&g_settings, SIGNAL(localeChanged()), this, SLOT(localeChanged()), Qt::UniqueConnection);
}

GlobalStrings::~GlobalStrings()
{
   delete(ini_);
}

void GlobalStrings::localeChanged()
{
   attachLocale();
}

void GlobalStrings::attachLocale()
{
   delete(ini_);
   ini_ = new LocaleIniFile(g_settings.localeIniFilePath());
}

QString GlobalStrings::getMessageText(const QString& name) const
{
   return ini_ ? ini_->getString("Messages", name) : QString();
}

QString GlobalStrings::getMenuItemText(const QString& name) const
{
   return ini_ ? ini_->getString("Menus", name) : QString();
}

QString GlobalStrings::getUILabelText(const QString& name) const
{
   return ini_ ? ini_->getString("Labels", name) : QString();
}
