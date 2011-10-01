#ifndef __LocalChessGui_h
#define __LocalChessGui_h

#include "MainWindow.h"
#include "CommandOptions.h"
#include "Singletons.h"
#include "GameProfile.h"

class K3ChessMainWindow;

class LocalChessGui : public QObject
{
   Q_OBJECT

   friend void Singletons::initialize();
   friend void Singletons::finalize();

   LocalChessGui();
   ~LocalChessGui();

public:
   void reset(); // resets gui to the initial state
   void beginNewGame(const QString& whitePlayerName, const QString& blackPlayerName,
                     const GameProfile& profile);
   void beginMoveSelection();
   void setInitialMoveCursorPos(ChessCoord coord);
   void showStaticMessage(const QString& msg);
   void showSessionMessage(const QString& msg);
   void showPlayerMessage(const QString& playerName, const QString& msg);
   void flipBoard(bool flipped);
   void updatePosition(const ChessPosition& position,
                       const CoordPair& lastMove=CoordPair(),
                       const ChessMoveMap& possibleMoves=ChessMoveMap());
   void appendToMoveList(const QString& str);
   void offerChoice(const CommandOptions& options);
   void enableDefaultKeyProcessing(bool value);
   //
   void dropLastFullMove();
   //
   void showSettingsDialog();
   void exitProgram();

signals:
   void userMoves(const CoordPair& pair);
   void userChoice(int id);
   void keyPressed(Qt::Key key);  // for keys pressed while no control is active

private slots:
   void updateBoardStyle();
   void updateInputMode();

private:
   void prepareCommandOptions();

private:
   K3ChessMainWindow *mainWindow_;
};

#endif
