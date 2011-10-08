#include "GlobalUISession.h"
#include "LocalChessGui.h"
#include "CommandOptionDefs.h"
#include "ChessPlayer_LocalHuman.h"
#include "ChessPlayer_LocalEngine.h"
#include "Settings.h"
#include "GlobalStrings.h"
#include "KeyMapper.h"
#include "GameSessionInfo.h"
#include "StringUtils.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>

GlobalUISession::GlobalUISession() :
   localHuman_(0), localEngine_(0), localHuman1_(0), localHuman2_(0),
   gameSession_(0), keyRemapIdx_(-1), menuType_(menuNone)
{
   initialize();
   //
   initialPosition_ = ChessPosition::fromString(g_settings.initialPositionFen().toStdString());
   initialPosition_.setChess960(g_settings.isChess960());
   //
   QObject::connect(&g_settings, SIGNAL(playerNameChanged()), this, SLOT(playerNameChanged()), Qt::UniqueConnection);
   QObject::connect(&g_settings, SIGNAL(engineChanged()), this, SLOT(engineChanged()), Qt::UniqueConnection);
   QObject::connect(&g_settings, SIGNAL(ponderingChanged()), this, SLOT(ponderingChanged()), Qt::UniqueConnection);
   QObject::connect(&g_settings, SIGNAL(localeChanged()), this, SLOT(localeChanged()), Qt::UniqueConnection);
   QObject::connect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key,Qt::KeyboardModifiers)), this, SLOT(keyPressed(Qt::Key, Qt::KeyboardModifiers)), Qt::UniqueConnection);
   QObject::connect(&g_localChessGui, SIGNAL(isExiting()), this, SLOT(isExiting()));
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
   localEngine_ = new ChessPlayer_LocalEngine(g_settings.engineInfo(), g_settings.currentEngineProfile());
   check960Support();
}

void GlobalUISession::begin()
{
   if(g_keyMapper.fileNotFound())
   {
      beginKeyRemapping();
   }
   else if(!restoreLastGame())
   {
      preGame(); // begin by offering a game
   }
}

void GlobalUISession::preGame()
{
   // offer user a choice of game types to play
   showNewGameMenu();
}

void GlobalUISession::playGame(GameType type,
                               const ChessClock& wclock,
                               const ChessClock& bclock)
{
   clearLastGame();
   //
   GameSessionInfo sessionInfo;
   //
   sessionInfo.profile.type = type;
   sessionInfo.profile.whiteClock = wclock;
   sessionInfo.profile.blackClock = bclock;
   sessionInfo.initialPosition = initialPosition_;
   //
   playGame(sessionInfo);
}

void GlobalUISession::playGame(const GameSessionInfo& sessionInfo)
{
   assert(gameSession_==0);
   //
   // reset gui
   g_localChessGui.reset(initialPosition_);
   //
   switch(sessionInfo.profile.type)
   {
      case gameComputerBlack:
         g_localChessGui.flipBoard(false);
         gameSession_ = new GameSession(localHuman_, localEngine_, sessionInfo);
         break;
      case gameComputerWhite:
         g_localChessGui.flipBoard(true);
         gameSession_ = new GameSession(localEngine_, localHuman_, sessionInfo);
         break;
      case gameTwoPlayers:
         g_localChessGui.flipBoard(false);
         gameSession_ = new GameSession(localHuman1_, localHuman2_, sessionInfo);
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
         playGame(gameComputerBlack, g_settings.playerClock(), g_settings.engineClock());
         break;
      case cmd_NewGame_PlayerBlack:
         playGame(gameComputerWhite, g_settings.engineClock(), g_settings.playerClock());
         break;
      case cmd_NewGame_TwoPlayers:
         playGame(gameTwoPlayers, g_settings.playerClock(), g_settings.playerClock());
         break;
      //
      case cmd_ExtMenu_Settings:
         g_localChessGui.showSettingsDialog();
         showNewGameMenu();
         break;
      case cmd_ExtMenu_Quit:
         requestExit();
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
      clearLastGame();
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
      clearLastGame();
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
      pgn << gameSession_->game().toPGN();
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
   if(g_settings.engineInfo().name==localEngine_->name()
      && g_settings.currentEngineProfile()==localEngine_->profileName()) return;
   delete localEngine_; localEngine_ = 0;
   localEngine_ = new ChessPlayer_LocalEngine(g_settings.engineInfo(), g_settings.currentEngineProfile());
   check960Support();
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
   QObject::connect(&g_localChessGui, SIGNAL(keyPressed(Qt::Key, Qt::KeyboardModifiers)), this,
      SLOT(keyPressed(Qt::Key, Qt::KeyboardModifiers)), Qt::UniqueConnection);
   nextKeyRemapPrompt();
}

void GlobalUISession::keyPressed(Qt::Key key, Qt::KeyboardModifiers modifiers)
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
         case Qt::Key_Home:
            if(gameSession_==0)
            {
               if(modifiers & Qt::ShiftModifier)
               {
                  // turn off chess 960 mode if it is on
                  if(g_settings.isChess960())
                  {
                     g_settings.setChess960(false);
                     initialPosition_ = cStandardInitialPosition;
                     g_localChessGui.updatePosition(initialPosition_);
                     if(localEngine_) localEngine_->setChess960(false);
                     g_settings.setInitialPositionFen(cStandardInitialFen.c_str());
                  }
               }
               else if(localEngine_ && localEngine_->info().supports960())
               {
                  // prepare a new initial 960 position if current engine supports it
                  g_settings.setChess960(true);
                  initialPosition_ = ChessPosition::new960Position();
                  g_localChessGui.updatePosition(initialPosition_);
                  g_settings.setInitialPositionFen(initialPosition_.toString().c_str());
                  localEngine_->setChess960(true);
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
      g_localChessGui.reset(initialPosition_);
      preGame();
   }
   else
   {
      g_localChessGui.showStaticMessage(g_msg("PressKeyFor").arg(
         g_msg(g_keyMapper.keyDefs()[keyRemapIdx_].keyName)));
   }
}

bool GlobalUISession::restoreLastGame()
{
   GameSessionInfo sessionInfo;
   if(!sessionInfo.loadFromFile(g_settings.lastGameFile())) return false;
   //
   QObject::connect(&startSavedGameTimer_, SIGNAL(timeout()),
                    this, SLOT(startSavedGameTimeout()));
   //
   startSavedGameTimer_.setSingleShot(true);
   startSavedGameTimer_.start(10); // any ridiculous amount will do
   //
   return true;
}

void GlobalUISession::clearLastGame()
{
   QDir dir(extractFolderPath(g_settings.lastGameFile()));
   dir.remove(g_settings.lastGameFile());
}

void GlobalUISession::requestExit()
{
   finalize();
   g_localChessGui.exitProgram();
}

void GlobalUISession::startSavedGameTimeout()
{
   QObject::disconnect(&startSavedGameTimer_, SIGNAL(timeout()),
                       this, SLOT(startSavedGameTimeout()));
   GameSessionInfo sessionInfo;
   sessionInfo.loadFromFile(g_settings.lastGameFile());
   playGame(sessionInfo);
}

void GlobalUISession::isExiting()
{
   if(gameSession_ && gameSession_->game().result()==resultNone
         && !gameSession_->game().moves().empty())
   {
      // save game (will be restored and continued on next program start)
      gameSession_->sessionInfo().saveToFile(g_settings.lastGameFile());
   }
}

void GlobalUISession::check960Support()
{
   if(!localEngine_) return;
   if(!localEngine_->info().supports960())
   {
      if(g_settings.isChess960())
      {
         g_settings.setChess960(false);
         initialPosition_ = cStandardInitialPosition;
         g_settings.setInitialPositionFen(cStandardInitialFen.c_str());
         g_localChessGui.updatePosition(initialPosition_);
      }
   }
   //
   if(g_settings.isChess960())
   {
      localEngine_->setChess960(true);
   }
}
