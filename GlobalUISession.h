#ifndef __GlobalUISession_h
#define __GlobalUISession_h

#include <QObject>
#include "Singletons.h"
#include "CommandOptions.h"
#include "GameProfile.h"
#include "GameSession.h"
#include "Singletons.h"

class ChessPlayer_LocalEngine;
class ChessPlayer_LocalHuman;

enum MenuType { menuNone, menuNewGame, menuExt };

// GlobalUISession starts and controls individual game sessions

class GlobalUISession : public QObject
{
   Q_OBJECT

   friend void Singletons::initialize();
   friend void Singletons::finalize();

   GlobalUISession();
   ~GlobalUISession();

public:
   void begin();

private slots:
   void userChoice(int id);
   void gameSessionEnded(GameSessionEndReason reason, ChessGameResult result,
                         const QString& message);

   void playerNameChanged();
   void engineChanged();
   void ponderingChanged();
   void localeChanged();
   void keyPressed(Qt::Key key);

private:
   void initialize();
   void preGame();
   void playGame(const GameProfile& profile);
   void postGame();
   void finalize();

   void offerChoice(const CommandOptions& options);

   void beginKeyRemapping();
   void nextKeyRemapPrompt();

   void showNewGameMenu();
   void showExtMenu();

   void saveGameToPGN();

private:
   ChessPlayer_LocalHuman *localHuman_;
   ChessPlayer_LocalEngine *localEngine_;
   ChessPlayer_LocalHuman *localHuman1_; // for two-player game
   ChessPlayer_LocalHuman *localHuman2_;
   GameSession *gameSession_;
   int keyRemapIdx_;
   MenuType menuType_;
};

#endif
