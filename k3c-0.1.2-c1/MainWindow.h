#ifndef __MainWindow_h
#define __MainWindow_h

#include <QDesktopWidget>
#include <QMainWindow>
#include <QPlainTextEdit>
#include "ChessBoardView.h"
#include "MoveListView.h"
#include "CommandPanel.h"

// Local GUI main window, can be used in turn by multiple players
// GlobalUISession ensures that only one game session at any time
// has access to local GUI

class K3ChessMainWindow : public QMainWindow
{
   Q_OBJECT
public:
   explicit K3ChessMainWindow(QWidget *parent = 0);

   ChessBoardView *boardView() const { return boardView_; }
   QPlainTextEdit *console() const { return console_; }
   MoveListView *moveList() const { return moveList_; }
   CommandPanel *commandPanel() const { return commandPanel_; }

   void setCustomKeyboardMode(bool value);
   void updateControlLayout();

protected:
   virtual void closeEvent(QCloseEvent*);

signals:
   void keyPressed(Qt::Key key);
   void isClosing();

protected:
   virtual void resizeEvent(QResizeEvent*);
   virtual void keyPressEvent(QKeyEvent*);

private slots:
   void commandPanel_optionSelected(int id);
   void commandPanel_idleClick();

private:
   ChessBoardView *boardView_;
   QPlainTextEdit *console_;
   MoveListView *moveList_;
   CommandPanel *commandPanel_;
   bool customKeyboardMode_;
};

#endif
