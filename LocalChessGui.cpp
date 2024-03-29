#include "LocalChessGui.h"
#include "MainWindow.h"
#include "StringUtils.h"
#include "GlobalStrings.h"
#include "Settings.h"
#include "SettingsDialog.h"

#include <QInputDialog>
#include <QMessageBox>

LocalChessGui::LocalChessGui() : clockDisplay_(false)
{
   mainWindow_ = new K3ChessMainWindow;
   //
   updateBoardStyle();
   updateInputMode();
   //
   mainWindow_->boardView()->updatePosition(
               ChessPosition::fromString(toStdString(g_settings.initialPositionFen())));
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
   QObject::connect(mainWindow_, SIGNAL(keyPressed(Qt::Key,Qt::KeyboardModifiers)), this, SIGNAL(keyPressed(Qt::Key,Qt::KeyboardModifiers)), Qt::UniqueConnection);
   QObject::connect(mainWindow_, SIGNAL(isClosing()), this, SIGNAL(isExiting()));
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
   if(clock.untimed) return "--";
   //
   QString s;
   //
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

QString getGameAnnouncementText(
      const QString& whitePlayerName, const QString& blackPlayerName,
      const GameProfile& profile, bool isResumedGame)
{
   QString s;
   s.reserve(128);
   //
   QString format;
   if(isResumedGame)
      format = g_msg("ResumedGamePlayerVsPlayer");
   else
      format = g_msg("NewGamePlayerVsPlayer");
   //
   s.append(format.arg(whitePlayerName, blackPlayerName));
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

void LocalChessGui::beginGame(
      const QString& whitePlayerName, const QString& blackPlayerName,
      const GameProfile& profile, bool isResumedGame)
{
   mainWindow_->refresh();
   mainWindow_->moveList()->clearMoves();
   mainWindow_->console()->clear();
   mainWindow_->boardView()->setInitialCursorPos(ChessCoord());
   mainWindow_->gameClock()->setLeftPlayerName(whitePlayerName);
   mainWindow_->gameClock()->setRightPlayerName(blackPlayerName);
   mainWindow_->console()->appendPlainText(
      getGameAnnouncementText(whitePlayerName, blackPlayerName, profile, isResumedGame));
   ClockActiveSide cas = casNone;
   switch(profile.type)
   {
      case gameComputerBlack: cas = casLeft; break;
      case gameComputerWhite: cas = casRight; break;
      default:
         break;
   }
   updateGameClock(cas, profile.whiteClock, profile.blackClock);
   g_localChessGui.switchToClockView();
}

void LocalChessGui::beginMoveSelection()
{
   g_localChessGui.showStaticMessage(g_msg("MovePrompt"));
   mainWindow_->boardView()->beginMoveSelection();
   switchToClockView();
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

void LocalChessGui::updateGameClock(ClockActiveSide cas,
                                    const ChessClock& whiteClock,
                                    const ChessClock& blackClock)
{
   QTime wtime;
   QTime btime;
   //
   //
   if(!whiteClock.untimed)
      wtime = QTime(0, 0, 0).addMSecs(whiteClock.remainingTime);
   if(!blackClock.untimed)
      btime = QTime(0, 0, 0).addMSecs(blackClock.remainingTime);
   //
   mainWindow_->gameClock()->setLeftClock(wtime);
   mainWindow_->gameClock()->setRightClock(btime);
   //
   mainWindow_->gameClock()->setActiveSide(cas);
}

void LocalChessGui::switchToClockView()
{
   if(g_settings.showGameClock())
      mainWindow_->switchToClockView();
   clockDisplay_ = true;
}

void LocalChessGui::switchToCommandView()
{
   mainWindow_->switchToCommandView();
   clockDisplay_ = false;
}

void LocalChessGui::appendToMoveList(const QString& str)
{
   mainWindow_->moveList()->addMove(str);
}

void LocalChessGui::appendToMoveList(const QStringList& slist)
{
   if(!slist.empty())
      mainWindow_->moveList()->addMoves(slist);
}

void LocalChessGui::offerChoice(const CommandOptions& options)
{
   mainWindow_->commandPanel()->setCommandOptions(options);
   //
   if(mainWindow_->commandPanel()->enter())
   {
      mainWindow_->boardView()->cancelMoveSelection();
   }
}

void LocalChessGui::flipBoard(bool flipped)
{
   mainWindow_->boardView()->setFlipped(flipped);
}

void LocalChessGui::reset(const ChessPosition& position)
{
   mainWindow_->moveList()->clearMoves();
   mainWindow_->console()->clear();
   //
   if(position.isEmpty())
      updatePosition(cStandardInitialPosition);
   else
      updatePosition(position);
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
   QImage pieces(g_settings.pieceImageFilePath());
   mainWindow_->boardView()->setPiecesImage(pieces);
   mainWindow_->capturedPieces()->setPiecesImage(pieces);
   mainWindow_->boardView()->setDrawMoveArrow(g_settings.drawMoveArrow());
   mainWindow_->boardView()->setDrawCoords(g_settings.drawCoordinates());
   mainWindow_->boardView()->setShowMoveHints(g_settings.showMoveHints());
   mainWindow_->updateControlLayout();
}

void LocalChessGui::updateInputMode()
{
   mainWindow_->boardView()->setDirectCoordinateInput(g_settings.coordinateMoveInput());
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

void LocalChessGui::endGame()
{
   switchToCommandView();
}

void LocalChessGui::updateShowClock()
{
   if(clockDisplay_ && g_settings.showGameClock())
   {
      emit clockUpdateRequest();
      mainWindow_->switchToClockView();
   }
   else
      mainWindow_->switchToCommandView();
}

void LocalChessGui::updateCapturedPieces(const ChessPosition &initialPosition, const ChessPosition &currentPosition)
{
   mainWindow_->capturedPieces()->updateContents(initialPosition, currentPosition);
}

void LocalChessGui::updateShowCaptured()
{
   if(g_settings.showCapturedPieces())
   {
      mainWindow_->showCapturedPieces();
   }
   else
   {
      mainWindow_->hideCapturedPieces();
   }
}

bool LocalChessGui::getInitialPosition(const QString &message, ChessPosition& position)
{
   bool ok;
   QString text = QInputDialog::getText(mainWindow_, "K3Chess",
                                        message, QLineEdit::Normal,
                                        "", &ok);
   if(ok)
   {
      position = ChessPosition::fromString(text.toStdString());
      if(position.isEmpty())
      {
         QMessageBox msgBox;
         msgBox.setText(g_msg("InvalidFEN"));
         msgBox.setIcon(QMessageBox::Critical);
         msgBox.exec();
         return false;
      }
      updatePosition(position);
      return true;
   }

   return false;
}

void LocalChessGui::updateMoveList(const QStringList &slist)
{
   mainWindow_->moveList()->updateMoves(slist);
}
