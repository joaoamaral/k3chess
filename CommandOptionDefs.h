#ifndef __CommandOptionDefs_h
#define __CommandOptionDefs_h

#include "CommandOptions.h"
#include "Singletons.h"

#include <QObject>

const int cmd_NewGame_PlayerWhite = 1;
const int cmd_NewGame_PlayerBlack = 2;
const int cmd_NewGame_TwoPlayers = 3;
const int cmd_ExtMenu_Settings = 10;
const int cmd_ExtMenu_Quit = 11;
const int cmd_InGame_Takeback = 20;
const int cmd_InGame_OfferDraw = 21;
const int cmd_InGame_Resign = 22;
const int cmd_InGame_Abort = 23;
const int cmd_Promotion_Prompt = -30; // id<0: static text
const int cmd_Promotion_Queen = 30;
const int cmd_Promotion_Rook = 31;
const int cmd_Promotion_Bishop = 32;
const int cmd_Promotion_Knight = 33;
const int cmd_PostGame_Save = 40;
const int cmd_PostGame_Discard = 42;
#ifdef CONFIG_DESKTOP
const int cmd_ExtMenu_FEN = 12;
#endif

class CommandOptionDefs : public QObject
{
   Q_OBJECT

   friend void Singletons::initialize();
   friend void Singletons::finalize();

   CommandOptionDefs();
   ~CommandOptionDefs();

public:
   CommandOptions& newGameOptions() { return newGameOptions_; }
   CommandOptions& extMenuOptions() { return extMenuOptions_; }
   CommandOptions& inGameOptions() { return inGameOptions_; }
   CommandOptions& promotionOptions() { return promotionOptions_; }
   CommandOptions& postGameOptions() { return postGameOptions_; }

private:
   void initialize();

private:
   CommandOptions newGameOptions_;
   CommandOptions extMenuOptions_;
   CommandOptions inGameOptions_;
   CommandOptions promotionOptions_;
   CommandOptions postGameOptions_;
};


#endif
