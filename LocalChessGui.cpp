#include "LocalChessGui.h"
#include "MainWindow.h"
#include "GlobalStrings.h"
#include "Settings.h"
#include "SettingsDialog.h"

LocalChessGui::LocalChessGui()
{
   mainWindow_ = new K3ChessMainWindow;
   //
   updateBoardStyle();
   updateInputMode();
   //
   mainWindow_->boardView()->updatePosition(
               ChessPosition::fromString(g_settings.defaultBoardSetup().toStdString()));
   //
   if(g_settings.profile().contains("ebook")||
         g_settings.profile().contains("handheld"))
   {
      mainWindow_->setWindowFlags(mainWindow_->windowFlags() | Qt::FramelessWindowHint);
      mainWindow_->showFullScreen();
   }
   else
   {
      QRect rect = mainWindow_->frameGeometry();
      rect.moveCenter(QDesktopWidget().availableGeometry().center());
      mainWindow_->move(rect.topLeft());
      //
      mainWindow_->show();
   }
   //
   QObject::connect(mainWindow_->boardView(), SIGNAL(moveEntered(const CoordPair&)), this, SIGNAL(userMoves(const CoordPair&)), Qt::UniqueConnection);
   QObject::connect(mainWindow_->commandPanel(), SIGNAL(optionSelected(int)), this, SIGNAL(userChoice(int)), Qt::UniqueConnection);
   QObject::connect(mainWindow_, SIGNAL(keyPressed(Qt::Key)), this, SIGNAL(keyPressed(Qt::Key)), Qt::UniqueConnection);
   //
   QObject::connect(&g_settings, SIGNAL(boardStyleChanged()), this, SLOT(updateBoardStyle()), Qt::UniqueConnection);
   QObject::connect(&g_settings, SIGNAL(inputSettingsChanged()), this, SLOT(updateInputMode()), Qt::UniqueConnection);
}

LocalChessGui::~LocalChessGui()
{
   delete mainWindow_;
}

QString timeSetupToString(const ChessClock& clock)
{
   QString s;
   s.reserve(32);
   s.append(QString::number(clock.initialTime/60000));
   if(clock.initialTime%60000 > 0)
   {
      s.append(':');
      s.append(QString::number((clock.initialTime%60000)/1000));
   }
   s.append(' ');
   s.append(QString::number(clock.moveIncrement/1000));
   //
   return s;
}

QString getNewGameText(
      const QString& whitePlayerName, const QString& blackPlayerName,
      const GameProfile& profile)
{
   QString s;
   s.reserve(128);
   //
   s.append(g_msg("NewGamePlayerVsPlayer").arg(whitePlayerName, blackPlayerName));
   //
   s.append("   ");
   s.append(timeSetupToString(profile.whiteClock));
   //
   if(profile.blackClock!=profile.whiteClock)
   {
      s.append(" (");
      s.append(timeSetupToString(profile.blackClock));
      s.append(")");
   }
   //
   return s;
}

void LocalChessGui::beginNewGame(
      const QString& whitePlayerName, const QString& blackPlayerName,
      const GameProfile& profile)
{
   mainWindow_->moveList()->clearMoves();
   mainWindow_->console()->clear();
   mainWindow_->boardView()->setInitialCursorPos(ChessCoord());
   mainWindow_->console()->appendPlainText(
      getNewGameText(whitePlayerName, blackPlayerName, profile));
}

void LocalChessGui::beginMoveSelection()
{
   g_localChessGui.showStaticMessage(g_msg("MovePrompt"));
   mainWindow_->boardView()->beginMoveSelection();
}

void LocalChessGui::showStaticMessage(const QString &msg)
{
   CommandOptions opt;
   opt.addStaticText(msg);
   mainWindow_->commandPanel()->setCommandOptions(opt);
}

void LocalChessGui::showSessionMessage(const QString& msg)
{
   mainWindow_->console()->appendPlainText(msg);
   mainWindow_->console()->moveCursor(QTextCursor::End);
   mainWindow_->console()->ensureCursorVisible();
}

void LocalChessGui::showPlayerMessage(const QString& playerName, const QString& msg)
{
   static const QString cMsgFormat = "[%1]: %2";
   mainWindow_->console()->appendPlainText(cMsgFormat.arg(playerName, msg));
}

void LocalChessGui::updatePosition(const ChessPosition& position,
                                   const CoordPair& lastMove,
                                   const ChessMoveMap& possibleMoves)
{
   mainWindow_->boardView()->updatePosition(position, lastMove, possibleMoves);
}

void LocalChessGui::appendToMoveList(const QString& str)
{
   mainWindow_->moveList()->addMove(str);
}

void LocalChessGui::offerChoice(const CommandOptions& options)
{
   mainWindow_->commandPanel()->setCommandOptions(options);
   if(mainWindow_->commandPanel()->enter())
   {
      mainWindow_->boardView()->cancelMoveSelection();
   }
}

void LocalChessGui::flipBoard(bool flipped)
{
   mainWindow_->boardView()->setFlipped(flipped);
}

void LocalChessGui::reset()
{
   mainWindow_->moveList()->clearMoves();
   mainWindow_->console()->clear();
   updatePosition(ChessPosition::fromString(
      g_settings.defaultBoardSetup().toStdString()));
}

void LocalChessGui::showSettingsDialog()
{
   SettingsDialog sdiag;
   if(g_settings.profile().contains("ebook")||
         g_settings.profile().contains("handheld"))
   {
      sdiag.setWindowFlags(sdiag.windowFlags() | Qt::FramelessWindowHint);
      sdiag.setGeometry(mainWindow_->geometry());
   }
   sdiag.exec();

}

void LocalChessGui::updateBoardStyle()
{
   mainWindow_->boardView()->setPiecesImage(QImage(g_settings.pieceImageFilePath()));
   mainWindow_->boardView()->setDrawMoveArrow(g_settings.drawMoveArrow());
   mainWindow_->boardView()->setDrawCoords(g_settings.drawCoordinates());
   mainWindow_->boardView()->setShowMoveHints(g_settings.showMoveHints());
}

void LocalChessGui::updateInputMode()
{
   mainWindow_->boardView()->setKeyCoordSelect(g_settings.keyColumnSelect());
   mainWindow_->boardView()->setKeyPieceSelect(g_settings.keyPieceSelect());
   mainWindow_->boardView()->setQuickSingleMoveSelection(g_settings.quickSingleMoveSelection());
}

void LocalChessGui::enableDefaultKeyProcessing(bool value)
{
   mainWindow_->setCustomKeyboardMode(!value);
}

void LocalChessGui::setInitialMoveCursorPos(ChessCoord coord)
{
   mainWindow_->boardView()->setInitialCursorPos(coord);
}

void LocalChessGui::dropLastFullMove()
{
   mainWindow_->moveList()->dropMoves(2);
}

void LocalChessGui::exitProgram()
{
   mainWindow_->close();
}
