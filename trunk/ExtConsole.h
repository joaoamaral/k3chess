#ifndef __ExtConsole_h
#define __ExtConsole_h

#include <QFrame>
#include <QPlainTextEdit>
#include "CapturedPiecesView.h"

class ExtConsole : public QFrame
{
   Q_OBJECT
public:
   explicit ExtConsole(QWidget *parent=0);
   QPlainTextEdit *console() { return console_; }
   CapturedPiecesView *capturedPieces() { return capturedPieces_; }

protected:
   virtual void resizeEvent(QResizeEvent*);

private:
   QPlainTextEdit *console_;
   CapturedPiecesView *capturedPieces_;
};

#endif
