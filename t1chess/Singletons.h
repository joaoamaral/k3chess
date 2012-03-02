#ifndef __Singletons_h
#define __Singletons_h

// contains functions for initializing/finalizing singletons
// in specific order and macros to access them globally

class T1ChessSettings;
class ChessRules;
class GlobalStrings;
class KeyMapper;
class CommandOptionDefs;
class LocalChessGui;
class GlobalUISession;
class RandomGen;

namespace Singletons
{
   void initialize();
   void finalize();

   const ChessRules& chessRules();
   const GlobalStrings& globalStrings();

   CommandOptionDefs& commandOptionDefs();
   T1ChessSettings& settings();
   KeyMapper& keyMapper();
   LocalChessGui& localChessGui();
   GlobalUISession& globalUISession();
   RandomGen& randomGen();
}

#define g_globalStrings Singletons::globalStrings()
#define g_chessRules Singletons::chessRules()
#define g_settings Singletons::settings()
#define g_keyMapper Singletons::keyMapper()
#define g_commandOptionDefs Singletons::commandOptionDefs()
#define g_localChessGui Singletons::localChessGui()
#define g_globalUISession Singletons::globalUISession()
#define g_randomGen Singletons::randomGen()

#endif
