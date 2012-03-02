#include "Singletons.h"
#include "ChessRules.h"
#include "Settings.h"
#include "CommandOptionDefs.h"
#include "GlobalStrings.h"
#include "LocalChessGui.h"
#include "GlobalUISession.h"
#include "KeyMapper.h"
#include "RandomGen.h"

#define __freeAndNil(T, p) { T *t = p; p = 0; delete t; }

namespace Singletons
{

ChessRules *chessRules_ = 0;
T1ChessSettings *settings_ = 0;
CommandOptionDefs *commandOptionDefs_ = 0;
GlobalStrings *globalStrings_ = 0;
LocalChessGui *localChessGui_ = 0;
GlobalUISession *globalUISession_ = 0;
KeyMapper *keyMapper_ = 0;
RandomGen *randomGen_ = 0;

void initialize()
{
   assert(settings_==0 && chessRules_==0 && commandOptionDefs_==0 &&
          globalStrings_==0 && localChessGui_==0 && globalUISession_==0);
   // singleton initialization with explicit order
   randomGen_ = new RandomGen();
   settings_ = new T1ChessSettings();
   chessRules_ = new ChessRules();
   globalStrings_ = new GlobalStrings();
   keyMapper_ = new KeyMapper();
   commandOptionDefs_ = new CommandOptionDefs();
   localChessGui_ = new LocalChessGui();
   globalUISession_ = new GlobalUISession();
}

void finalize()
{
   assert(settings_ && chessRules_ && commandOptionDefs_ &&
          globalStrings_ && localChessGui_ && globalUISession_);
   // singleton finalization with explicit order
   __freeAndNil(GlobalUISession, globalUISession_);
   __freeAndNil(LocalChessGui, localChessGui_);
   __freeAndNil(CommandOptionDefs, commandOptionDefs_);
   __freeAndNil(KeyMapper, keyMapper_);
   __freeAndNil(GlobalStrings, globalStrings_);
   __freeAndNil(ChessRules, chessRules_);
   __freeAndNil(T1ChessSettings, settings_);
   __freeAndNil(RandomGen, randomGen_);
}

const ChessRules& chessRules() { assert(chessRules_); return *chessRules_; }
const GlobalStrings& globalStrings() { assert(globalStrings_); return *globalStrings_; }
CommandOptionDefs& commandOptionDefs() { assert(commandOptionDefs_); return *commandOptionDefs_; }
T1ChessSettings& settings() { assert(settings_); return *settings_; }
KeyMapper& keyMapper() { assert(keyMapper_); return *keyMapper_; }
LocalChessGui& localChessGui() { assert(localChessGui_); return *localChessGui_; }
GlobalUISession& globalUISession() { assert(globalUISession_); return *globalUISession_; }
RandomGen& randomGen() { assert(randomGen_); return *randomGen_; }

}
