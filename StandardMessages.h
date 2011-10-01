#ifndef __StandardMessages_h
#define __StandardMessages_h

#include <QObject>
#include <QString>
#include "Singletons.h"

extern QString cMsg_playerOffersDraw;
extern QString cMsg_playerAcceptsDraw;
extern QString cMsg_playerRejectsDraw;
extern QString cMsg_playerResigns;

extern QString cMsg_newGamePlayerVsPlayer;
extern QString cMsg_waitingForPlayerToMove;
extern QString cMsg_illegalMove;

extern QString cMsg_playerIsNotReady;
extern QString cMsg_gameAborted;
extern QString cMsg_gameDrawnByAgreement;
extern QString cMsg_gameDrawnByStalemate;
extern QString cMsg_gameDrawnByRepetition;
extern QString cMsg_gameDrawnBy50MoveRule;
extern QString cMsg_whiteCheckmates;
extern QString cMsg_blackCheckmates;
extern QString cMsg_whiteWonOnTime;
extern QString cMsg_blackWonOnTime;
extern QString cMsg_whiteResigns;
extern QString cMsg_blackResigns;

extern QString cMsg_cantChangeSettingsWhilePlaying;

class StandardMessagesLocalizer : public QObject
{
   Q_OBJECT

   StandardMessagesLocalizer();
   ~StandardMessagesLocalizer();

   friend void Singletons::initialize();
   friend void Singletons::finalize();

private slots:
   void localeChanged();

private:
   void loadDefaults();
   void loadFromIniFile(const QString& fileName);
};

#endif
