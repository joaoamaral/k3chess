#ifndef __LocalChessGui_h
#define __LocalChessGui_h

#include "MainWindow.h"
#include "CommandOptions.h"
#include "Singletons.h"
#include "GameProfile.h"

class T1ChessMainWindow;

class LocalChessGui : public QObject
{
   Q_OBJECT

   friend void Singletons::initialize();
   friend void Singletons::finalize();

   LocalChessGui();
   ~LocalChessGui();

public:
   void reset(const ChessPosition& position = ChessPosition()); // resets gui to the initial state (showing the given position)
   void beginGame(const QString& whitePlayerName, const QString& blackPlayerName,
                     const GameProfile& profile, bool isResumedGame=false);
   void beginMoveSelection();
   void setInitialMoveCursorPos(ChessCoord coord);
   void showStaticMessage(const QString& msg);
   void showSessionMessage(const QString& msg);
   void endGame();
   void showPlayerMessage(const QString& playerName, const QString& msg);
   void flipBoard(bool flipped);
   void updatePosition(const ChessPosition& position,
                       const CoordPair& lastMove=CoordPair(),
                       const ChessMoveMap& possibleMoves=ChessMoveMap());
   void updateCapturedPieces(const ChessPosition& initialPosition,
                             const ChessPosition& currentPosition);
   void updateGameClock(ClockActiveSide cas, const ChessClock& whiteClock, const ChessClock& blackClock);
   void appendToMoveList(const QString& str);
   void appendToMoveList(const QStringList& str);
   void updateMoveList(const QStringList& moves);
   void offerChoice(const CommandOptions& options);
   void enableDefaultKeyProcessing(bool value);
   //
   void dropLastFullMove();
   //
   void showSettingsDialog();
   void exitProgram();

   void switchToClockView();
   void switchToCommandView();
   void updateShowClock();
   void updateShowCaptured();

signals:
   void userMoves(const CoordPair& pair);
   void userChoice(int id);
   void keyPressed(Qt::Key key, Qt::KeyboardModifiers modifiers); // for keys pressed while no control is active
   void commandPanelClick(); // non-menu idle click in custom keyboard mode
   void clockUpdateRequest();
   void isExiting();

private slots:
   void updateBoardStyle();
   void updateInputMode();

private:
   void prepareCommandOptions();

private:
   T1ChessMainWindow *mainWindow_;
   bool clockDisplay_;
};

#endif
