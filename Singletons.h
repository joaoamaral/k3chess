#ifndef __Singletons_h
#define __Singletons_h

// contains functions for initializing/finalizing singletons
// in specific order and macros to access them globally

class K3ChessSettings;
class ChessRules;
class GlobalStrings;
class KeyMapper;
class CommandOptionDefs;
class LocalChessGui;
class GlobalUISession;

namespace Singletons
{
   void initialize();
   void finalize();

   const ChessRules& chessRules();
   const GlobalStrings& globalStrings();

   CommandOptionDefs& commandOptionDefs();
   K3ChessSettings& settings();
   KeyMapper& keyMapper();
   LocalChessGui& localChessGui();
   GlobalUISession& globalUISession();
}

#define g_globalStrings Singletons::globalStrings()
#define g_chessRules Singletons::chessRules()
#define g_settings Singletons::settings()
#define g_keyMapper Singletons::keyMapper()
#define g_commandOptionDefs Singletons::commandOptionDefs()
#define g_localChessGui Singletons::localChessGui()
#define g_globalUISession Singletons::globalUISession()

#endif
