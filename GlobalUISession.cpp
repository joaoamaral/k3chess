#include "GlobalUISession.h"
#include "LocalChessGui.h"
#include "CommandOptionDefs.h"
#include "ChessPlayer_LocalHuman.h"
#include "ChessPlayer_LocalEngine.h"
#include "Settings.h"
#include "GlobalStrings.h"
#include "KeyMapper.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>

GlobalUISession::GlobalUISession() :
   localHuman_(0), localEngine_(0), localHuman1_(0), localHuman2_(0),
   gameSession_(0), keyRemapIdx_(-1), menuType_(menuNone)
{
   initialize();
   //
   QObject::connect(&g_settings, SIGNAL(playerNameChanged()), this, SLOT(playerNameChanged()), Qt::UniqueConnection);
   QObject::connect(&g_settings, SIGNAL(engineChanged()), this, SLOT(engineChanged()), Qt::UniqueConnection);
   QObject::connect(&g_settings, SIGNAL(ponderingChanged()), this, SLOT(ponderingChanged()), Qt::UniqueConnection);
   QObject::connect(&g_settings, SIGNAL(localeChanged()), this, SLOT(localeChanged()), Qt::UniqueConnection);
   QObject::connect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key)), this, SLOT(keyPressed(Qt::Key)), Qt::UniqueConnection);
}

GlobalUISession::~GlobalUISession()
{
   finalize();
}

void GlobalUISession::initialize()
{
   localHuman_ = new ChessPlayer_LocalHuman(g_settings.playerName());
   localHuman1_ = new ChessPlayer_LocalHuman("P1");   // @@todo: ask player names via UI before game starts
   localHuman2_ = new ChessPlayer_LocalHuman("P2");
   localEngine_ = new ChessPlayer_LocalEngine(g_settings.engineInfo());
}

void GlobalUISession::begin()
{
   if(g_keyMapper.fileNotFound())
   {
      beginKeyRemapping();
   }
   else
   {
      preGame(); // begin by offering a game
   }
}

void GlobalUISession::preGame()
{
   // offer user a choice of game types to play
   showNewGameMenu();
}

void GlobalUISession::playGame(const GameProfile& profile)
{
   assert(gameSession_==0);
   //
   // reset gui
   g_localChessGui.reset();
   //
   switch(profile.type)
   {
      case gameComputerBlack:
         g_localChessGui.flipBoard(false);
         gameSession_ = new GameSession(localHuman_, localEngine_, profile);
         break;
      case gameComputerWhite:
         g_localChessGui.flipBoard(true);
         gameSession_ = new GameSession(localEngine_, localHuman_, profile);
         break;
      case gameTwoPlayers:
         g_localChessGui.flipBoard(false);
         gameSession_ = new GameSession(localHuman1_, localHuman2_, profile);
         break;
      case gameTwoEngines:
         assert(false); // @@note: not supported yet
         break;
   }
   //
   if(gameSession_)
   {
      QObject::connect(gameSession_, SIGNAL(end(GameSessionEndReason, ChessGameResult, const QString&)), this,
                       SLOT(gameSessionEnded(GameSessionEndReason, ChessGameResult, const QString&)), Qt::UniqueConnection);
      gameSession_->begin();
   }
}

void GlobalUISession::postGame()
{
   QObject::disconnect(gameSession_, SIGNAL(end(GameSessionEndReason, ChessGameResult, const QString&)), this,
                       SLOT(gameSessionEnded(GameSessionEndReason, ChessGameResult, const QString&)));
   delete gameSession_; gameSession_ = 0;
   preGame();
}

void GlobalUISession::finalize()
{
   delete gameSession_; gameSession_ = 0;
   delete localHuman_;  localHuman_ = 0;
   delete localEngine_; localEngine_ = 0;
   delete localHuman1_; localHuman1_ = 0;
   delete localHuman2_; localHuman2_ = 0;
}

void GlobalUISession::offerChoice(const CommandOptions& options)
{
   QObject::connect(&g_localChessGui, SIGNAL(userChoice(int)), this, SLOT(userChoice(int)), Qt::UniqueConnection);
   g_localChessGui.offerChoice(options);
}

void GlobalUISession::userChoice(int id)
{
   QObject::disconnect(&g_localChessGui, SIGNAL(userChoice(int)));
   //
   switch(id)
   {
      case cmd_NewGame_PlayerWhite:
         playGame(GameProfile(gameComputerBlack, g_settings.playerClock(), g_settings.engineClock()));
         break;
      case cmd_NewGame_PlayerBlack:
         playGame(GameProfile(gameComputerWhite, g_settings.engineClock(), g_settings.playerClock()));
         break;
      case cmd_NewGame_TwoPlayers:
         playGame(GameProfile(gameTwoPlayers, g_settings.playerClock(), g_settings.playerClock()));
         break;
      //
      case cmd_ExtMenu_Settings:
         g_localChessGui.showSettingsDialog();
         showNewGameMenu();
         break;
      case cmd_ExtMenu_Quit:
         g_localChessGui.exitProgram();
         break;
      //
      case cmd_PostGame_Save:
         saveGameToPGN();
         postGame();
         break;
      case cmd_PostGame_Discard:
         postGame();
         break;
   }
}

void GlobalUISession::gameSessionEnded(GameSessionEndReason reason,
                                       ChessGameResult result, const QString &message)
{
   if(reason==reasonGameFinished)
   {
      // offer to save/discard/analyze game
      if(g_settings.autoSaveGames())
      {
         saveGameToPGN();
         postGame();
      }
      else
      {
         offerChoice(g_commandOptionDefs.postGameOptions());
      }
   }
   else
   {
      postGame();
   }
}

void GlobalUISession::saveGameToPGN()
{
   assert(gameSession_);
   //
   QFile pgnFile(g_settings.pgnFilePath());
   //
   if(pgnFile.open(QIODevice::Append | QIODevice::WriteOnly))
   {
      QTextStream pgn(&pgnFile);
      // use UTF-8 for pgn file, however, it may display
      // gibberish in some chess programs which expect it to be ANSI
      // this only affects metadata, but not the game data itself
      // at least we don't lose any information, and the file can
      // be converted to another code page manually, if needed
      pgn.setCodec(QTextCodec::codecForName("UTF-8"));
      pgn << gameSession_->pgn();
   }
   //
   pgnFile.close();
}

void GlobalUISession::engineChanged()
{
   if(localEngine_==0) return;
   if(gameSession_!=0)
   {
      assert(false);  // attempt to change engine while playing: not supported
      return;
   }
   if(g_settings.engineInfo().name==localEngine_->name()) return;
   delete localEngine_; localEngine_ = 0;
   localEngine_ = new ChessPlayer_LocalEngine(g_settings.engineInfo());
}

void GlobalUISession::playerNameChanged()
{
   if(localHuman_==0) return;
   if(gameSession_!=0)
   {
      assert(false);  // attempt to change player name while playing: not supported
      return;
   }
   if(g_settings.playerName()==localHuman_->name()) return;
   delete localHuman_; localHuman_= 0;
   localHuman_ = new ChessPlayer_LocalHuman(g_settings.playerName());
}

void GlobalUISession::ponderingChanged()
{
   if(localEngine_==0) return;
   if(gameSession_!=0)
   {
      assert(false);  // attempt to change pondering setting while playing: not supported
      return;
   }
   localEngine_->ponderingChanged();
}

void GlobalUISession::localeChanged()
{
   if(gameSession_!=0) return;
   //
   preGame();
}

void GlobalUISession::beginKeyRemapping()
{
   keyRemapIdx_ = 0;
   g_keyMapper.clearMapping();
   g_localChessGui.enableDefaultKeyProcessing(false);
   QObject::connect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key)), this,
      SLOT(keyPressed(Qt::Key)), Qt::UniqueConnection);
   nextKeyRemapPrompt();
}

void GlobalUISession::keyPressed(Qt::Key key)
{
   if(keyRemapIdx_>=0)
   {
      g_keyMapper.addMappedKey(key, g_keyMapper.keyDefs()[keyRemapIdx_].qtKey);
      ++keyRemapIdx_;
      nextKeyRemapPrompt();
   }
   else
   {
      switch(key)
      {
         case Qt::Key_Cancel:
         case Qt::Key_Escape:
         case Qt::Key_Menu:
            if(gameSession_==0)
            {
               if(menuType_==menuNewGame)
               {
                  showExtMenu();
               }
               else if(menuType_==menuExt)
               {
                  showNewGameMenu();
               }
            }
            break;
         default:
            break;
      }
   }
}

void GlobalUISession::showNewGameMenu()
{
   menuType_ = menuNewGame;
   if(g_settings.engineInfo().name.isEmpty())
   {
      g_commandOptionDefs.newGameOptions().disable(cmd_NewGame_PlayerWhite);
      g_commandOptionDefs.newGameOptions().disable(cmd_NewGame_PlayerBlack);
   }
   else
   {
      g_commandOptionDefs.newGameOptions().enable(cmd_NewGame_PlayerWhite);
      g_commandOptionDefs.newGameOptions().enable(cmd_NewGame_PlayerBlack);
   }
   offerChoice(g_commandOptionDefs.newGameOptions());
}

void GlobalUISession::showExtMenu()
{
   menuType_ = menuExt;
   offerChoice(g_commandOptionDefs.extMenuOptions());
}

void GlobalUISession::nextKeyRemapPrompt()
{
   if(keyRemapIdx_==g_keyMapper.keyDefs().size())
   {
      keyRemapIdx_ = -1;
      // end of key remapping sequence,
      // proceed to preGame
      g_keyMapper.saveKeyMapping();
      g_localChessGui.enableDefaultKeyProcessing(true);
      g_localChessGui.reset();
      preGame();
   }
   else
   {
      g_localChessGui.showStaticMessage(g_msg("PressKeyFor").arg(
         g_msg(g_keyMapper.keyDefs()[keyRemapIdx_].keyName)));
   }
}

