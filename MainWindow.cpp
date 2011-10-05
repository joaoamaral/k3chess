#include "MainWindow.h"
#include "Settings.h"
#include "StyleSheets.h"
#include "KeyMapper.h"
#include <QCloseEvent>

const int cDefaultCommandPanelHeight = 38;

K3ChessMainWindow::K3ChessMainWindow(QWidget *parent) :
   QMainWindow(parent), customKeyboardMode_(false)
{
   boardView_ = new ChessBoardView(this);
   console_ = new QPlainTextEdit(this);
   moveList_ = new MoveListView(this);
   commandPanel_ = new CommandPanel(this);
   //
   QFont textFont("Sans", 8, QFont::Bold);
   //
   boardView_->setFocusPolicy(Qt::NoFocus);
   console_->setReadOnly(true);
   console_->setFocusPolicy(Qt::NoFocus);
   console_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   console_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   console_->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
   console_->setFont(textFont);
   moveList_->setFocusPolicy(Qt::NoFocus);
   moveList_->setFont(textFont);
   moveList_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   moveList_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   commandPanel_->setFocusPolicy(Qt::NoFocus);
   commandPanel_->setFont(textFont);
   commandPanel_->setUseHotKeys(false);
   //
   QObject::connect(commandPanel_, SIGNAL(optionSelected(int)), this,
                    SLOT(commandPanel_optionSelected(int)), Qt::UniqueConnection);
   QObject::connect(commandPanel_, SIGNAL(idleClick()), this,
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
      commandPanel_->setBackgroundColor(Qt::white);
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
      emit keyPressed(Qt::Key(event->key()));
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
      case Qt::Key_X:
      case Qt::Key_Q:
      case Qt::Key_Escape:
         if(event->modifiers() & Qt::AltModifier)
         {
            close();  // close main window when
                      // Alt+Esc, Alt+Q or Alt+X is pressed
         }
         break;
      default:
         break;
   }
   //
   switch(key)
   {
      case Qt::Key_Refresh:
         {
            // this is a cludge, because repaint() does not
            // always trigger actual repaint
            resize(width(), height()+1);
            resize(width(), height());
          }
         break;
      default:
         if(commandPanel_->hasCursor())
         {
            if(commandPanel_->processKey(key)) return;
         }
         else if(boardView_->hasCursor())
         {
            if(boardView_->processKey(key)) return;
         }
         //
         emit keyPressed(key);
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
   // allow entering menu by double-clicking on inactive/static command panel part
   // (this supports stylus/mouse operation, while GUI remains keyboard-oriented)
   emit keyPressed(Qt::Key_Menu);
}

void K3ChessMainWindow::updateControlLayout()
{
   if(rect().isEmpty()) return;
   //
   // find appropriate board size
   int pad = boardView_->padding();
   int w1 = width()-pad*2;
   int h1 = height()-pad*2;
   int wboard = (w1/64)*64 + pad*2;
   int hboard = ((h1-cDefaultCommandPanelHeight)/64)*64 + pad*2;
   //
   int sboard = qMin(wboard, hboard);
   //
   int x0 = (width()-sboard)/2;
   int y0 = (height()-sboard-cDefaultCommandPanelHeight)/2;
   int t0 = qMin(x0, y0);
   //
   QRect boardRect(t0, t0, sboard, sboard);
   QRect commandRect(boardRect.left(), boardRect.bottom()+1,
                     boardRect.width(), cDefaultCommandPanelHeight);

   //
   QRect consoleRect;
   QRect moveListRect;
   //
   if(width()<height())
   {
      // portrait orientation
      consoleRect = QRect(0, commandRect.bottom()+1,
                          width()*31/50, height()-commandRect.bottom()-1);
      moveListRect = QRect(consoleRect.right()+1, consoleRect.top()+1,
                           width()-consoleRect.right()-1, consoleRect.height());
   }
   else
   {
      // landscape orientation
      consoleRect = QRect(boardRect.right()+1, boardRect.top(),
                          width()-boardRect.right()-1, boardRect.height()*19/50);
      moveListRect = QRect(consoleRect.left(), consoleRect.bottom()+1,
                           consoleRect.width(), boardRect.height()-consoleRect.height());
   }
   //
   boardView_->setGeometry(boardRect);
   commandPanel_->setGeometry(commandRect);
   console_->setGeometry(consoleRect);
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
