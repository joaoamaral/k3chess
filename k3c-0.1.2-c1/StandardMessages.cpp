#include "StandardMessages.h"
#include "Settings.h"

#include <QSettings>
#include <QTextCodec>

QString cMsg_playerOffersDraw;
QString cMsg_playerAcceptsDraw;
QString cMsg_playerRejectsDraw;
QString cMsg_playerResigns;

QString cMsg_newGamePlayerVsPlayer;
QString cMsg_waitingForPlayerToMove;
QString cMsg_illegalMove;

QString cMsg_playerIsNotReady;
QString cMsg_gameAborted;
QString cMsg_gameDrawnByAgreement;
QString cMsg_gameDrawnByStalemate;
QString cMsg_gameDrawnByRepetition;
QString cMsg_gameDrawnBy50MoveRule;
QString cMsg_whiteCheckmates;
QString cMsg_blackCheckmates;
QString cMsg_whiteWonOnTime;
QString cMsg_blackWonOnTime;
QString cMsg_whiteResigns;
QString cMsg_blackResigns;
QString cMsg_cantChangeSettingsWhilePlaying;

const QString cDefaultMsg_playerOffersDraw = "%1 offers draw";   // @@todo: move to settings
const QString cDefaultMsg_playerAcceptsDraw = "%1 accepts draw"; // @@todo: move to settings
const QString cDefaultMsg_playerRejectsDraw = "%1 rejects draw"; // @@todo: move to settings
const QString cDefaultMsg_playerResigns = "%1 resigns";        // @@todo: move to settings

const QString cDefaultMsg_newGamePlayerVsPlayer = "New game: %1 vs %2";
const QString cDefaultMsg_waitingForPlayerToMove = "%1 is thinking...";
const QString cDefaultMsg_illegalMove = "Illegal move: %1";

const QString cDefaultMsg_playerIsNotReady = "%1 timed out";
const QString cDefaultMsg_gameAborted = "Game aborted";
const QString cDefaultMsg_gameDrawnByAgreement = "Game drawn by agreement";
const QString cDefaultMsg_gameDrawnByStalemate = "Game drawm by stalemate";
const QString cDefaultMsg_gameDrawnByRepetition = "Game drawn by repetition";
const QString cDefaultMsg_gameDrawnBy50MoveRule = "Game drawm by 50 move rule";
const QString cDefaultMsg_whiteCheckmates = "White checkmates";
const QString cDefaultMsg_blackCheckmates = "Black checkmates";
const QString cDefaultMsg_whiteWonOnTime = "White won on time";
const QString cDefaultMsg_blackWonOnTime = "Black won on time";
const QString cDefaultMsg_whiteResigns = "White resigns";
const QString cDefaultMsg_blackResigns = "Black resigns";

const QString cDefaultMsg_cantChangeSettingsWhilePlaying = "Settings dialog cannot be opened while playing";

StandardMessagesLocalizer::StandardMessagesLocalizer()
{
   loadDefaults();
   QObject::connect(&g_settings, SIGNAL(localeChanged()), this, SLOT(localeChanged()), Qt::UniqueConnection);
}

StandardMessagesLocalizer::~StandardMessagesLocalizer()
{

}

void StandardMessagesLocalizer::localeChanged()
{
   // re-read locale strings from locale ini file
   QString iniFile = g_settings.localeIniFilePath();
   //
   if(iniFile.isEmpty())
   {
      loadDefaults();
   }
   else
   {
      loadFromIniFile(iniFile);
   }
}

void StandardMessagesLocalizer::loadDefaults()
{
   cMsg_playerOffersDraw = cDefaultMsg_playerOffersDraw;
   cMsg_playerAcceptsDraw = cDefaultMsg_playerAcceptsDraw;
   cMsg_playerRejectsDraw = cDefaultMsg_playerRejectsDraw;
   cMsg_playerResigns = cDefaultMsg_playerResigns;

   cMsg_newGamePlayerVsPlayer = cDefaultMsg_newGamePlayerVsPlayer;
   cMsg_waitingForPlayerToMove = cDefaultMsg_waitingForPlayerToMove;
   cMsg_illegalMove = cDefaultMsg_illegalMove;

   cMsg_playerIsNotReady = cDefaultMsg_playerIsNotReady;
   cMsg_gameAborted = cDefaultMsg_gameAborted;
   cMsg_gameDrawnByAgreement = cDefaultMsg_gameDrawnByAgreement;
   cMsg_gameDrawnByStalemate = cDefaultMsg_gameDrawnByStalemate;
   cMsg_gameDrawnByRepetition = cDefaultMsg_gameDrawnByRepetition;
   cMsg_gameDrawnBy50MoveRule = cDefaultMsg_gameDrawnBy50MoveRule;
   cMsg_whiteCheckmates = cDefaultMsg_whiteCheckmates;
   cMsg_blackCheckmates = cDefaultMsg_blackCheckmates;
   cMsg_whiteWonOnTime = cDefaultMsg_blackCheckmates;
   cMsg_blackWonOnTime = cDefaultMsg_blackCheckmates;
   cMsg_whiteResigns = cDefaultMsg_blackCheckmates;
   cMsg_blackResigns = cDefaultMsg_blackCheckmates;

   cMsg_cantChangeSettingsWhilePlaying = cDefaultMsg_cantChangeSettingsWhilePlaying;
}

void StandardMessagesLocalizer::loadFromIniFile(const QString& fileName)
{
   QSettings ini(fileName, QSettings::IniFormat);
   ini.setIniCodec(QTextCodec::codecForName("UTF-8"));
   //
   cMsg_playerOffersDraw = ini.value("Messages/PlayerOffersDraw", cDefaultMsg_playerOffersDraw).toString();
   cMsg_playerAcceptsDraw = ini.value("Messages/PlayerAcceptsDraw", cDefaultMsg_playerAcceptsDraw).toString();
   cMsg_playerRejectsDraw = ini.value("Messages/PlayerRejectsDraw", cDefaultMsg_playerRejectsDraw).toString();
   cMsg_playerResigns = ini.value("Messages/PlayerResigns", cDefaultMsg_playerResigns).toString();

   cMsg_newGamePlayerVsPlayer = ini.value("Messages/NewGamePlayerVsPlayer", cDefaultMsg_newGamePlayerVsPlayer).toString();
   cMsg_waitingForPlayerToMove = ini.value("Messages/WaitingForPlayerToMove", cDefaultMsg_waitingForPlayerToMove).toString();
   cMsg_illegalMove = ini.value("Messages/IllegalMove", cDefaultMsg_illegalMove).toString();

   cMsg_playerIsNotReady = ini.value("Messages/PlayerIsNotReady", cDefaultMsg_playerIsNotReady).toString();
   cMsg_gameAborted = ini.value("Messages/GameAborted", cDefaultMsg_gameAborted).toString();
   cMsg_gameDrawnByAgreement = ini.value("Messages/GameDrawnByAgreement", cDefaultMsg_gameDrawnByAgreement).toString();
   cMsg_gameDrawnByStalemate = ini.value("Messages/GameDrawnByStalemate", cDefaultMsg_gameDrawnByStalemate).toString();
   cMsg_gameDrawnByRepetition = ini.value("Messages/GameDrawnByRepetition", cDefaultMsg_gameDrawnByRepetition).toString();
   cMsg_gameDrawnBy50MoveRule = ini.value("Messages/GameDrawnBy50MoveRule", cDefaultMsg_gameDrawnBy50MoveRule).toString();
   cMsg_whiteCheckmates = ini.value("Messages/WhiteCheckmates", cDefaultMsg_whiteCheckmates).toString();
   cMsg_blackCheckmates = ini.value("Messages/BlackCheckmates", cDefaultMsg_blackCheckmates).toString();
   cMsg_whiteWonOnTime = ini.value("Messages/WhiteWonOnTime", cDefaultMsg_blackCheckmates).toString();
   cMsg_blackWonOnTime = ini.value("Messages/BlackWonOnTime", cDefaultMsg_blackCheckmates).toString();
   cMsg_whiteResigns = ini.value("Messages/WhiteResigns", cDefaultMsg_blackCheckmates).toString();
   cMsg_blackResigns = ini.value("Messages/BlackResigns", cDefaultMsg_blackCheckmates).toString();

   cMsg_cantChangeSettingsWhilePlaying = ini.value("Messages/CantChangeSettingsWhilePlaying", cDefaultMsg_cantChangeSettingsWhilePlaying).toString();
}
