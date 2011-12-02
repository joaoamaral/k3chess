#include "Settings.h"
#include "GlobalStrings.h"
#include "StringUtils.h"
#include "ChessPosition.h"
#include "assert.h"

#include <QTime>
#include <QTextCodec>

// @@note: you can override default locale ("English")
// by providing a file named "English.ini" in locales directory
// with your own text strings for labels and messages

namespace
{

const QString cK3ChessIniPath = "./K3Chess.ini";
const QString cDefaultKeymapIniFile = "./keys.ini";
const QString cDefaultPlayerClockSetup = "15 0";
const QString cDefaultEngineClockSetup = "5 0";
const QString cDefaultPgnFilePath = "./games.pgn";
const QString cDefaultLastGameFile = "./lastgame.dat";
const QString cDefaultPlayerName = "Player";
const QString cDefaultLocaleName = "English";
const QString cDefaultPgnEventName = "K3Chess game";
const QString cDefaultSiteName = "?";
const int cDefaultBoardMargins = 16;

bool containsDigits(const QString& s)
{
   for(int i=0; i<s.length(); ++i)
   {
      if(s[i].isDigit()) return true;
   }
   return false;
}

ChessClock stringToClock(const QString& s)
{
   ChessClock clock;
   //
   if(!containsDigits(s)) return clock;
   //
   int secs_per_game = 0;
   int secs_per_move = 0;
   //
   if(s.indexOf(' ')==0)
   {
      secs_per_game = s.toInt()*60000;
   }
   else
   {
      //
      QString s_game = s.section(' ', 0, 0);
      QString s_move = s.section(' ', 1, 1);
      //
      if(s_game.indexOf(':')>=0)
      {
         QString s_min = s_game.section(':', 0, 0);
         QString s_sec = s_game.section(':', 1, 1);
         //
         secs_per_game = s_min.toInt()*60 + s_sec.toInt();
      }
      else
      {
         secs_per_game = s_game.toInt()*60;
      }
      //
      secs_per_move = s_move.toInt();
   }
   //
   clock.initialTime = secs_per_game*1000;
   clock.remainingTime = clock.initialTime;
   clock.moveIncrement = secs_per_move*1000;
   //
   clock.untimed = false;
   //
   return clock;
}

}

Profile Profile::fromString(const QString& str)
{
   Profile profile;
   profile.keywords_ = str.toLower().split(';', QString::SkipEmptyParts);
   return profile;
}

bool Profile::contains(const QString& str) const
{
   return keywords_.indexOf(str.toLower())>=0;
}

K3ChessSettings::K3ChessSettings() :
   settings_(cK3ChessIniPath, QSettings::IniFormat)
{
   settings_.setIniCodec(QTextCodec::codecForName("UTF-8"));
   //
   profile_ = Profile::fromString(settings_.value("Profile", "Default").toString());
   // enumerate engines (must have "engine.ini"-files with descriptions)
   enumEngines(QDir("./engines"));
   enumPiecesStyles(QDir(":/pieces"));
   enumLocales(QDir(":/locales"));
}

K3ChessSettings::~K3ChessSettings()
{

}

const EngineInfo& K3ChessSettings::engineInfo() const
{
   QString name = settings_.value("Engine", QString()).toString();
   std::map<QString, EngineInfo>::const_iterator it = engines_.find(name);
   if(it==engines_.end())
   {
      static const EngineInfo dummy;
      if(engines_.empty()) return dummy;
      return engines_.begin()->second;
   }
   else
      return it->second;
}

QString K3ChessSettings::localeName() const
{
   QString name = settings_.value("Locale", cDefaultLocaleName).toString();
   if(locales_.find(name)==locales_.end())
   {
      if(locales_.empty()) return cDefaultLocaleName;
      return locales_.begin()->first;
   }
   else
      return name;
}

QString K3ChessSettings::piecesStyle() const
{
   QString name = settings_.value("Board/PiecesStyle", QString()).toString();
   if(piecesStyles_.find(name)==piecesStyles_.end())
   {
      if(piecesStyles_.empty()) return QString();
      return piecesStyles_.begin()->first;
   }
   else
      return name;
}

QString K3ChessSettings::pieceImageFilePath() const
{
   return pieceImageFileFromName(piecesStyle());
}

QString K3ChessSettings::localeIniFilePath() const
{
   return localeIniFilePathFromName(localeName());
}

QString K3ChessSettings::pgnFilePath() const
{
   return settings_.value("Export/PGNFile", cDefaultPgnFilePath).toString();
}

QString K3ChessSettings::playerName() const
{
   return settings_.value("PlayerName", cDefaultPlayerName).toString();
}

bool K3ChessSettings::drawMoveArrow() const
{
   return settings_.value("Board/DrawMoveArrow", true).toBool();
}

bool K3ChessSettings::drawCoordinates() const
{
   return settings_.value("Board/DrawCoordinates", true).toBool();
}

ChessClock K3ChessSettings::playerClock() const
{
   return stringToClock(playerClockString());
}

ChessClock K3ChessSettings::engineClock() const
{
   return stringToClock(engineClockString());
}

QString K3ChessSettings::playerClockString() const
{
   return settings_.value("Game/PlayerClock", cDefaultPlayerClockSetup).toString();
}

QString K3ChessSettings::engineClockString() const
{
   return settings_.value("Game/EngineClock", cDefaultEngineClockSetup).toString();
}

bool K3ChessSettings::autoSaveGames() const
{
   return settings_.value("Misc/AutoSaveGames", false).toBool();
}

bool K3ChessSettings::canPonder() const
{
   return settings_.value("Game/Pondering", false).toBool();
}

bool K3ChessSettings::keyColumnSelect() const
{
   return settings_.value("Input/KeyCoordSelect", true).toBool();
}

bool K3ChessSettings::keyPieceSelect() const
{
   return settings_.value("Input/KeyPieceSelect", true).toBool();
}

QString K3ChessSettings::keymapFile() const
{
   return cDefaultKeymapIniFile;
}

void K3ChessSettings::enumEngines(QDir dir)
{
    enumEnginesProc(dir);
    if(!engines_.empty() &&
          engines_.find(settings_.value("Engine",
             QString()).toString())==engines_.end())
    {
       settings_.setValue("Engine", engines_.begin()->first);
    }
}

void K3ChessSettings::enumEnginesProc(QDir dir)
{
   dir.setFilter(QDir::Files | QDir::Dirs);
   QFileInfoList items = dir.entryInfoList();
   foreach(QFileInfo fi, items)
   {
      if(fi.isDir())
      {
         if(!fi.fileName().startsWith("."))
            enumEnginesProc(fi.filePath());
      }
      else if(fi.fileName()=="engine.ini")
      {
         EngineInfo info;
         if(readEngineInfo(fi.absoluteFilePath(), info))
         {
            engines_.insert(std::make_pair(info.name, info));
         }
      }
   }
}

void K3ChessSettings::enumPiecesStyles(QDir dir)
{
   dir.setFilter(QDir::Files | QDir::Dirs);
   QFileInfoList items = dir.entryInfoList();
   foreach(QFileInfo fi, items)
   {
      if(fi.isDir())
      {
         if(!fi.fileName().startsWith("."))
            enumPiecesStyles(fi.filePath());
      }
      else if(fi.fileName().contains('.'))
      {
         QString ext = fi.fileName().section('.', -1, -1);
         QString name = fi.fileName().left(fi.fileName().length()-ext.length()-1);
         if(ext=="png" || ext=="bmp" || ext=="jpg")
         {
            piecesStyles_.insert(std::make_pair(name, fi.filePath()));
         }
      }
   }
   //
   if(!piecesStyles_.empty() &&
         piecesStyles_.find(settings_.value("Board/PiecesStyle",
            QString()).toString())==piecesStyles_.end())
   {
      settings_.setValue("Board/PiecesStyle", piecesStyles_.begin()->first);
   }
}

void K3ChessSettings::enumLocales(QDir dir)
{
   dir.setFilter(QDir::Files | QDir::Dirs);
   QFileInfoList items = dir.entryInfoList();
   foreach(QFileInfo fi, items)
   {
      if(fi.isDir())
      {
         if(!fi.fileName().startsWith("."))
            enumLocales(fi.filePath());
      }
      else if(fi.fileName().endsWith(".ini"))
      {
         QSettings ini(fi.filePath(), QSettings::IniFormat);
         ini.setIniCodec(QTextCodec::codecForName("UTF-8"));
         QString name = ini.value("Info/LocaleName", QString()).toString();
         if(!name.isEmpty())
         {
            locales_.insert(std::make_pair(name, fi.filePath()));
         }
      }
   }
   //
   QString lname = settings_.value("Locale",
                                    cDefaultLocaleName).toString();
   //
   if((!locales_.empty() && locales_.find(lname)==locales_.end()) ||
       (locales_.empty() && lname!=cDefaultLocaleName))
   {
      settings_.setValue("Locale", cDefaultLocaleName);
   }
}

QStringList K3ChessSettings::getEngineNames() const
{
   QStringList qsl;
   std::map<QString, EngineInfo>::const_iterator it = engines_.begin(), itEnd = engines_.end();
   for(;it!=itEnd;++it)
   {
      qsl.append(it->first);
   }
   return qsl;
}

QStringList K3ChessSettings::getPiecesStyleNames() const
{
   QStringList qsl;
   std::map<QString, QString>::const_iterator it = piecesStyles_.begin(), itEnd = piecesStyles_.end();
   for(;it!=itEnd;++it)
   {
      qsl.append(it->first);
   }
   return qsl;
}

QStringList K3ChessSettings::getLocaleNames() const
{
   QStringList qsl;
   std::map<QString, QString>::const_iterator it = locales_.begin(), itEnd = locales_.end();
   for(;it!=itEnd;++it)
   {
      qsl.append(it->first);
   }
   if(qsl.isEmpty())
   {
      qsl.append(cDefaultLocaleName);
   }
   return qsl;
}

QString K3ChessSettings::pieceImageFileFromName(const QString& name) const
{
   std::map<QString, QString>::const_iterator it = piecesStyles_.find(name);
   if(it==piecesStyles_.end())
   {
      return QString();
   }
   else
   {
      return it->second;
   }
}

QString K3ChessSettings::localeIniFilePathFromName(const QString& name) const
{
   std::map<QString, QString>::const_iterator it = locales_.find(name);
   if(it==locales_.end())
   {
      return QString(); // indicates default (hard coded) locale
   }
   else
   {
      return it->second;
   }
}

void K3ChessSettings::setPiecesStyle(const QString& name)
{
   if(name==settings_.value("Board/PiecesStyle").toString()) return;
   settings_.setValue("Board/PiecesStyle", name);
   emit boardStyleChanged();
}

void K3ChessSettings::setLocaleName(const QString& name)
{
   if(locales_.empty() || name==settings_.value("Locale").toString()) return;
   settings_.setValue("Locale", name);
   emit localeChanged();
}

void K3ChessSettings::setCanPonder(bool value)
{
   if(value==canPonder()) return;
   settings_.setValue("Game/Pondering", value);
   emit ponderingChanged();
}

void K3ChessSettings::setDrawMoveArrow(bool value)
{
   if(value==drawMoveArrow()) return;
   settings_.setValue("Board/DrawMoveArrow", value);
   emit boardStyleChanged();
}

void K3ChessSettings::setDrawCoordinates(bool value)
{
   if(value==drawCoordinates()) return;
   settings_.setValue("Board/DrawCoordinates", value);
   emit boardStyleChanged();
}

void K3ChessSettings::setAutoSaveGames(bool value)
{
   if(value==autoSaveGames()) return;
   settings_.setValue("Misc/AutoSaveGames", value);
}

void K3ChessSettings::setPlayerName(const QString& name)
{
   if(name==playerName()) return;
   settings_.setValue("PlayerName", name);
   emit playerNameChanged();
}

void K3ChessSettings::setPlayerClock(const QString& str)
{
   QString s = containsDigits(str) ? str : QString("--");
   if(playerClockString()==s) return;
   settings_.setValue("Game/PlayerClock", s);
   emit timeSettingsChanged();
}

void K3ChessSettings::setEngineClock(const QString& str)
{
   if(engineClockString()==str) return;
   settings_.setValue("Game/EngineClock", str);
   emit timeSettingsChanged();
}

const Profile& K3ChessSettings::profile() const
{
   return profile_;
}

void K3ChessSettings::flush()
{
   settings_.sync();
}

bool K3ChessSettings::showMoveHints() const
{
   return settings_.value("Board/ShowMoveHints", true).toBool();
}

void K3ChessSettings::setShowMoveHints(bool value)
{
   if(value==showMoveHints()) return;
   settings_.setValue("Board/ShowMoveHints", value);
   emit boardStyleChanged();
}

bool K3ChessSettings::readEngineInfo(const QString& engineIniFile,
                                     EngineInfo& info)
{
   QSettings ini(engineIniFile, QSettings::IniFormat);
   ini.setIniCodec(QTextCodec::codecForName("UTF-8"));
   //
   ini.beginGroup("Description");
   QString name = ini.value("EngineName", QString()).toString();
   if(name.isEmpty()) return false;
   QString exePath = ini.value("Executable", QString()).toString();
   if(exePath.isEmpty()) return false;
   if(exePath.startsWith("../") || exePath.startsWith("..\\"))
   {
      exePath = extractFolderPath(extractFolderPath(engineIniFile))+exePath.mid(2);
   }
   else if(exePath.startsWith("./") || exePath.startsWith(".\\"))
   {
      exePath = extractFolderPath(engineIniFile)+exePath.mid(1);
   }
   else if(!exePath.contains('/') && !exePath.contains('\\'))
   {
      exePath = extractFolderPath(engineIniFile) + '/' + exePath;
   }
   EngineType type = etDetect;
   QString typeStr = ini.value("EngineType", QString()).toString();
   if(typeStr.toLower()=="uci")
   {
      type = etUCI;
   }
   else if(typeStr.toLower()=="xboard")
   {
      type = etXBoard;
   }
   //
   ini.endGroup();
   //
   info.name = name;
   info.exePath = exePath;
   info.type = type;
   //
   QStringList groupNames = ini.childGroups();
   foreach(QString groupName, groupNames)
   {
      if(groupName.startsWith("Profile"))
      {
         ini.beginGroup(groupName);
         //
         QString profileName = ini.value("Name", QString()).toString();
         //
         QStringList& commands = info.startupCommands[profileName];
         commands.clear();
         //
         int nStartupCommands = ini.value("CommandCount", 0).toInt();
         if(nStartupCommands)
         {
            commands.reserve(nStartupCommands);
            for(int i=1; i<=nStartupCommands; ++i)
            {
               QString cmdVar = QString("Command") + QString::number(i);
               QString cmd = ini.value(cmdVar, QString()).toString();
               if(!cmd.isEmpty()) commands.push_back(cmd);
            }
         }
         //
         ini.endGroup();
         //
         if(commands.isEmpty())
         {
            info.startupCommands.erase(info.startupCommands.find(groupName));
         }
         else
         {
            info.profileNames.push_back(profileName);
         }
      }
   }
   //
   if(info.profileNames.isEmpty())
   {
      info.profileNames.push_back("Default");
   }
   //
   QString masks = ini.value("Cleanup/DeleteFiles", QString()).toString().trimmed();
   if(!masks.isEmpty())
      info.cleanUpMasks = masks.split(';');
   //
   ini.beginGroup("Modes");
   //
   info.commandStandard = ini.value("CommandStandard", QString()).toString();
   info.command960 = ini.value("Command960", QString()).toString();
   //
   ini.endGroup();
   return true;
}

bool K3ChessSettings::quickSingleMoveSelection() const
{
   return settings_.value("QuickSingleMoveSelection", true).toBool();
}

void K3ChessSettings::setQuickSingleMoveSelection(bool value)
{
   if(value==quickSingleMoveSelection()) return;
   settings_.setValue("QuickSingleMoveSelection", value);
   emit inputSettingsChanged();
}

QString K3ChessSettings::lastGameFile() const
{
   return cDefaultLastGameFile;
}

bool K3ChessSettings::isChess960() const
{
   return settings_.value("Chess960", false).toBool();
}

void K3ChessSettings::setChess960(bool value)
{
   settings_.setValue("Chess960", value);
}

QString K3ChessSettings::currentEngineProfile() const
{
   return settings_.value("EngineProfile", "Default").toString();
}

void K3ChessSettings::setEngine(const QString& engineName, const QString& profileName)
{
   bool changed = false;
   if(engineName!=settings_.value("Engine").toString())
   {
      settings_.setValue("Engine", engineName);
      changed = true;
   }
   //
   if(profileName!=currentEngineProfile())
   {
      if(engineInfo().startupCommands.find(profileName)==engineInfo().startupCommands.end())
      {
         assert(false);
         return; // attempt to set a non-existing profile
      }
      settings_.setValue("EngineProfile", profileName);
      changed = true;
   }
   //
   if(changed) emit engineChanged();
}

const EngineInfo& K3ChessSettings::engineInfo(const QString &engineName) const
{
   std::map<QString, EngineInfo>::const_iterator it = engines_.find(engineName);
   if(it==engines_.end())
   {
      static const EngineInfo dummy;
      return dummy;
   }
   else
   {
      return it->second;
   }
}

void K3ChessSettings::setCoordinateMoveInput(bool value)
{
   if(coordinateMoveInput()==value) return;
   settings_.setValue("CoordinateMoveInput", value);
   emit inputSettingsChanged();
}

bool K3ChessSettings::coordinateMoveInput() const
{
   return settings_.value("CoordinateMoveInput", false).toBool();
}

QString K3ChessSettings::initialPositionFen() const
{
   return settings_.value("InitialPosition", QString(cStandardInitialFen.c_str())).toString();
}

void K3ChessSettings::setInitialPositionFen(const QString &fen)
{
   settings_.setValue("InitialPosition", fen);
}

int K3ChessSettings::boardMargins() const
{
   int value = settings_.value("Board/Margins", cDefaultBoardMargins).toInt();
   if(value<0) return 0;
   else return value;
}

void K3ChessSettings::setBoardMargins(int value)
{
   if(value==boardMargins()) return;
   settings_.setValue("Board/Margins", value);
   emit boardStyleChanged();
}

bool K3ChessSettings::showGameClock() const
{
   return settings_.value("ShowGameClock", false).toBool();
}

void K3ChessSettings::setShowGameClock(bool value)
{
   settings_.setValue("ShowGameClock", value);
}

bool K3ChessSettings::showCapturedPieces() const
{
   return settings_.value("ShowCapturedPieces", false).toBool();
}

void K3ChessSettings::setShowCapturedPieces(bool value)
{
   settings_.setValue("ShowCapturedPieces", value);
}

bool K3ChessSettings::useRussianNotation() const
{
   return settings_.value("UseRussianNotation", false).toBool();
}

void K3ChessSettings::setUseRussianNotation(bool value)
{
   settings_.setValue("UseRussianNotation", value);
}

