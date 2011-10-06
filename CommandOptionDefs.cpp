#include "CommandOptionDefs.h"

const CommandOption cmdOption_NewGame_PlayerWhite(cmd_NewGame_PlayerWhite, "NewGame_PlayerWhite", srtMenuVar, Qt::Key_N);
const CommandOption cmdOption_NewGame_PlayerBlack(cmd_NewGame_PlayerBlack, "NewGame_PlayerBlack", srtMenuVar, Qt::Key_B);
const CommandOption cmdOption_NewGame_TwoPlayers(cmd_NewGame_TwoPlayers, "NewGame_TwoPlayers", srtMenuVar, Qt::Key_T);
const CommandOption cmdOption_ExtMenu_Settings(cmd_ExtMenu_Settings, "ExtMenu_Settings", srtMenuVar, Qt::Key_O);
//const CommandOption cmdOption_ExtMenu_About(cmd_ExtMenu_About, "ExtMenu_About", srtMenuVar, Qt::Key_A);
const CommandOption cmdOption_ExtMenu_Quit(cmd_ExtMenu_Quit, "ExtMenu_Quit", srtMenuVar, Qt::Key_Q);
const CommandOption cmdOption_InGame_Takeback(cmd_InGame_Takeback, "InGame_Takeback", srtMenuVar, Qt::Key_T);
const CommandOption cmdOption_InGame_OfferDraw(cmd_InGame_OfferDraw, "InGame_OfferDraw", srtMenuVar, Qt::Key_D);
const CommandOption cmdOption_InGame_Resign(cmd_InGame_Resign, "InGame_Resign", srtMenuVar, Qt::Key_R);
const CommandOption cmdOption_InGame_Abort(cmd_InGame_Abort, "InGame_Abort", srtMenuVar, Qt::Key_R);
const CommandOption cmdOption_Promotion_Prompt(cmd_Promotion_Prompt, "Promotion_Prompt", srtMenuVar, Qt::Key_unknown);
const CommandOption cmdOption_Promotion_Queen(cmd_Promotion_Queen, "Promotion_Queen", srtMenuVar, Qt::Key_Q);
const CommandOption cmdOption_Promotion_Rook(cmd_Promotion_Rook, "Promotion_Rook", srtMenuVar, Qt::Key_R);
const CommandOption cmdOption_Promotion_Bishop(cmd_Promotion_Bishop, "Promotion_Bishop", srtMenuVar, Qt::Key_B);
const CommandOption cmdOption_Promotion_Knight(cmd_Promotion_Knight, "Promotion_Knight", srtMenuVar, Qt::Key_N);
const CommandOption cmdOption_PostGame_Save(cmd_PostGame_Save, "PostGame_Save", srtMenuVar, Qt::Key_S);
const CommandOption cmdOption_PostGame_Discard(cmd_PostGame_Discard, "PostGame_Discard", srtMenuVar, Qt::Key_D);

CommandOptionDefs::CommandOptionDefs()
{
   initialize();
}

CommandOptionDefs::~CommandOptionDefs()
{
}

void CommandOptionDefs::initialize()
{
   newGameOptions_.clear();
   inGameOptions_.clear();
   promotionOptions_.clear();
   postGameOptions_.clear();
   //
   newGameOptions_.add(cmdOption_NewGame_PlayerWhite);
   newGameOptions_.add(cmdOption_NewGame_PlayerBlack);
   newGameOptions_.add(cmdOption_NewGame_TwoPlayers);
   //
   extMenuOptions_.add(cmdOption_ExtMenu_Settings);
   extMenuOptions_.add(cmdOption_ExtMenu_Quit);
   //
   inGameOptions_.add(cmdOption_InGame_Takeback);
   inGameOptions_.add(cmdOption_InGame_OfferDraw);
   inGameOptions_.add(cmdOption_InGame_Resign);
   inGameOptions_.add(cmdOption_InGame_Abort);
   //
   promotionOptions_.add(cmdOption_Promotion_Prompt);
   promotionOptions_.add(cmdOption_Promotion_Queen);
   promotionOptions_.add(cmdOption_Promotion_Rook);
   promotionOptions_.add(cmdOption_Promotion_Bishop);
   promotionOptions_.add(cmdOption_Promotion_Knight);
   //
   postGameOptions_.add(cmdOption_PostGame_Save);
   postGameOptions_.add(cmdOption_PostGame_Discard);
}
