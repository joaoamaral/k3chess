#ifndef __GlobalStrings_h
#define __GlobalStrings_h

#include <QSettings>
#include "Singletons.h"

// all localized ui, console and other strings
// are to be accessed from here

class LocaleIniFile;

class GlobalStrings: public QObject
{
   Q_OBJECT

   GlobalStrings();
   ~GlobalStrings();

   friend void Singletons::initialize();
   friend void Singletons::finalize();

public:
   QString getMessageText(const QString& name) const;
   QString getMenuItemText(const QString& name) const;
   QString getUILabelText(const QString& name) const;

private slots:
   void localeChanged();

private:
   void attachLocale();

private:
   LocaleIniFile *ini_;
};

#define g_msg(x) g_globalStrings.getMessageText(x)
#define g_menu(x) g_globalStrings.getMenuItemText(x)
#define g_label(x) g_globalStrings.getUILabelText(x)

#endif
