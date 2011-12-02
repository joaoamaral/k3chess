#include "MainWindow.h"
#include "Settings.h"
#include "StyleSheets.h"
#include "KeyMapper.h"
#include <QCloseEvent>

const int cDefaultCommandPanelHeight = 48;

K3ChessMainWindow::K3ChessMainWindow(QWidget *parent) :
   QMainWindow(parent), boardView_(0),
   extConsole_(0), moveList_(0),
   commandPanel_(0), commandArea_(0),
   gameClock_(0), customKeyboardMode_(false)
{
   boardView_ = new ChessBoardView(this);
   extConsole_ = new ExtConsole(this);
   moveList_ = new MoveListView(this);
   commandPanel_ = new CommandPanel(this);
   commandArea_ = new QStackedWidget(this);
   gameClock_ = new GameClockView(this);
   //
   boardBorderColor_ = boardView_->borderColor();
   //
   commandArea_->addWidget(commandPanel_);
   commandArea_->addWidget(gameClock_);
   commandArea_->setCurrentWidget(commandPanel_);
   //
   if(g_settings.showCapturedPieces())
      extConsole_->showCapturedPieces();
   else
      extConsole_->hideCapturedPieces();
   //
   QFont textFont("Sans", 8, QFont::Bold);
   //
   boardView_->setFocusPolicy(Qt::NoFocus);
   moveList_->setFocusPolicy(Qt::NoFocus);
   moveList_->setFont(textFont);
   moveList_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   moveList_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   commandPanel_->setFocusPolicy(Qt::NoFocus);
   commandPanel_->setFont(textFont);
   commandPanel_->setUseHotKeys(false);
   //console_->setReadOnly(true);
   //console_->setFocusPolicy(Qt::NoFocus);
   //
   QObject::connect(commandPanel_, SIGNAL(optionSelected(int)), this,
                    SLOT(commandPanel_optionSelected(int)), Qt::UniqueConnection);
   QObject::connect(commandPanel_, SIGNAL(idleClick()), this,
                    SLOT(commandPanel_idleClick()), Qt::UniqueConnection);
   QObject::connect(gameClock_, SIGNAL(click()), this,
                    SLOT(commandPanel_idleClick()), Qt::UniqueConnection);
   //
   setMinimumSize(256, 256);
   setMaximumSize(4096, 4096);
   //
   resize(480, 640);
   //
   if(g_settings.profile().contains("ebook"))
   {
      setStyleSheet(cUIStyleSheet_Ebook);
      //
      QFont clockFont(font());
      clockFont.setPixelSize(15);
      clockFont.setBold(false);
      gameClock_->setClockFont(clockFont);
      //
      QFont captNumberFont(font());
      captNumberFont.setPixelSize(14);
      captNumberFont.setBold(false);
      capturedPieces()->setNumberFont(captNumberFont);
   }
   else
   {
      setStyleSheet(cUIStyleSheet_Normal);
      //
      QFont clockFont(font());
      clockFont.setPixelSize(12);
      clockFont.setBold(false);
      gameClock_->setClockFont(clockFont);
      //
      QFont captNumberFont(font());
      captNumberFont.setPixelSize(11);
      captNumberFont.setBold(false);
      capturedPieces()->setNumberFont(captNumberFont);
   }
}

void K3ChessMainWindow::resizeEvent(QResizeEvent *)
{
   updateControlLayout();
}

void K3ChessMainWindow::keyPressEvent(QKeyEvent *event)
{
   event->accept(); // consume this event anyway
   //
   if(customKeyboardMode_)
   {
      emit keyPressed(Qt::Key(event->key()), event->modifiers());
      return;
   }
   //
   // @@todo: add different handlers for different profiles;
   // cache keyboard profile type in an integer/logical value
   // for faster processing
   //
   Qt::Key key = g_keyMapper.getQtKey(event->key());
   //
   switch(key)
   {
      /*case Qt::Key_X:
      case Qt::Key_Q:*/
      case Qt::Key_Escape:
      case Qt::Key_Back:
         /*if(event->modifiers() & Qt::AltModifier)
         {*/
            close();  // close main window when Esc (Back) is pressed
         /*}*/
         break;
      default:
         break;
   }
   //
   switch(key)
   {
      case Qt::Key_Refresh:
         {
            refresh();
         }
         break;
      default:
         if((event->modifiers() & Qt::AltModifier)==0)
         {
            if(commandPanel_->hasCursor())
            {
               if(commandPanel_->processKey(key)) return;
            }
            else if(boardView_->hasCursor())
            {
               if(boardView_->processKey(key)) return;
            }
         }
         //
         emit keyPressed(key, event->modifiers());
         //
         break;
   }
}

void K3ChessMainWindow::commandPanel_optionSelected(int)
{
   // return to board view (if possible)
   // after user selected an option in the command panel
   boardView_->enter();
}

void K3ChessMainWindow::commandPanel_idleClick()
{
    if(customKeyboardMode_)
    {
        // in custom keyboard mode command panel idle click
        // is NOT converted into Menu key press
        emit commandPanelClick();
    }
    else
    {
       // allow entering menu by clicking inactive/static region of command panel
       // (this supports stylus/mouse operation, while GUI remains keyboard-oriented)
       emit keyPressed(Qt::Key_Menu, 0);
    }
}

void K3ChessMainWindow::updateControlLayout()
{
   if(rect().isEmpty()) return;
   //
   // find appropriate board size
   int pad = boardView_->padding();
   int bm = g_settings.boardMargins();
   int w1 = width()-pad*2-bm;
   int h1 = height()-pad*2-bm;
   int wboard = (w1/8)*8 + pad*2;
   int hboard = ((h1-cDefaultCommandPanelHeight)/8)*8 + pad*2;
   //
   int sboard = qMin(wboard, hboard);
   //
   int x0 = (width()-sboard)/2;
   int y0 = (height()-sboard-cDefaultCommandPanelHeight)/2;
   int t0 = qMin(x0, y0);
   //
   if(t0<=24)
      boardView_->setBorderColor(Qt::white);
   else
      boardView_->setBorderColor(boardBorderColor_);
   //
   QRect boardRect(t0, t0, sboard, sboard);
   QRect commandRect;
   QRect consoleRect;
   QRect moveListRect;
   //
   if(width()<height())
   {
      // portrait orientation
      commandRect = QRect(0, boardRect.bottom()+1,
                          rect().width(), cDefaultCommandPanelHeight);
      consoleRect = QRect(0, commandRect.bottom()+1,
                          width()*31/50, height()-commandRect.bottom()-1);
      moveListRect = QRect(consoleRect.right()+1, consoleRect.top(),
                           width()-consoleRect.right()-1, consoleRect.height());
   }
   else
   {
      int x1 = boardRect.right() + t0;
      commandRect = QRect(0, boardRect.bottom()+1,
                          x1, cDefaultCommandPanelHeight);
      // landscape orientations
      consoleRect = QRect(x1+1, boardRect.top(),
                          width()-x1-1, boardRect.height()*19/50);
      moveListRect = QRect(consoleRect.left(), consoleRect.bottom()+1,
                           consoleRect.width(), boardRect.height()-consoleRect.height());
   }
   //
   boardView_->setGeometry(boardRect);
   commandArea_->setGeometry(commandRect);
   extConsole_->setGeometry(consoleRect);
   //console_->setGeometry(consoleRect);
   moveList_->setGeometry(moveListRect);
}

void K3ChessMainWindow::setCustomKeyboardMode(bool value)
{
   customKeyboardMode_ = value;
}

void K3ChessMainWindow::closeEvent(QCloseEvent*)
{
   emit isClosing();
}

int K3ChessMainWindow::switchToClockView()
{
   int retval = commandArea_->currentIndex();
   commandArea_->setCurrentWidget(gameClock_);
   return retval;
}

int K3ChessMainWindow::switchToCommandView()
{
   int retval = commandArea_->currentIndex();
   commandArea_->setCurrentWidget(commandPanel_);
   return retval;
}

int K3ChessMainWindow::setCommandAreaMode(int idx)
{
   int retval = commandArea_->currentIndex();
   commandArea_->setCurrentIndex(idx);
   return retval;
}

void K3ChessMainWindow::showCapturedPieces()
{
   extConsole_->showCapturedPieces();
}

void K3ChessMainWindow::hideCapturedPieces()
{
   extConsole_->hideCapturedPieces();
}

void K3ChessMainWindow::refresh()
{
   // this is a cludge, because repaint() does not
   // always trigger actual repaint
   if(height()%2)
   {
      resize(width(), height()-1);
   }
   else
   {
      resize(width(), height()+1);
   }
}
