#include "ExtConsole.h"

namespace
{

const int cMaxCaptPieceSize = 32;

}

ExtConsole::ExtConsole(QWidget *parent) :
   QFrame(parent), console_(0), capturedPieces_(0),
   showCapturedPieces_(false)
{
   console_ = new QPlainTextEdit(this);
   capturedPieces_ = new CapturedPiecesView(this);
   //
   setFrameShape(console_->frameShape());
   setFrameStyle(console_->frameStyle());
   //
   console_->setFrameShape(QFrame::NoFrame);
   //
   console_->setReadOnly(true);
   console_->setFocusPolicy(Qt::NoFocus);
   console_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   console_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   console_->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
   //
   QFont textFont("Sans", 8, QFont::Bold);
   console_->setFont(textFont);

}

void ExtConsole::resizeEvent(QResizeEvent *)
{
   updateLayout();
}

void ExtConsole::showCapturedPieces()
{
   if(showCapturedPieces_) return; // already visible
   showCapturedPieces_ = true;
   updateLayout();
}

void ExtConsole::hideCapturedPieces()
{
   if(!showCapturedPieces_) return; // already hidden
   showCapturedPieces_ = false;
   updateLayout();
}

void ExtConsole::updateLayout()
{
   int w = rect().width()-2;
   int h = rect().height()-2;
   int s = 0;
   if(showCapturedPieces_)
   {
      s = w/11;
      if(s>h/2) s = h/2;
      if(s>cMaxCaptPieceSize) s = cMaxCaptPieceSize;
      capturedPieces_->setGeometry(1, h-s+1, w, s);
   }
   else
   {
      capturedPieces_->setGeometry(1, h, w, 0);
   }
   //
   console_->setGeometry(1, 1, w, h-s+1);
}
