#include "MoveListView.h"
#include <QResizeEvent>
#include <QTextCursor>
#include <assert.h>

namespace
{

QString getMoveString(unsigned idx, const QString& move)
{
   static const QString cWhiteMove = "%1. %2";
   //
   if((idx%2)==0)
   {
      return cWhiteMove.arg(QString::number(idx/2+1), move);
   }
   else
   {
      return move;
   }
}

}

MoveListView::MoveListView(QWidget *parent) :
   QPlainTextEdit(parent)
{
   setReadOnly(true);
   setWordWrapMode(QTextOption::ManualWrap);
}

void MoveListView::addMove(const QString& move)
{
   if(move.isEmpty())
   {
      assert(false);
      return;
   }
   //
   QString s = getMoveString(moves_.size(), move);
   //
   moves_.push_back(s);
   //
   regenerateLines();
}

void MoveListView::addMoves(const QStringList& moves)
{
   if(!moves.isEmpty())
   {
      foreach(QString move, moves)
      {
         QString s = getMoveString(moves_.size(), move);
         moves_.push_back(s);
      }
      regenerateLines();
   }
}

void MoveListView::clearMoves()
{
   moves_.clear();
   clear();
}

void MoveListView::resizeEvent(QResizeEvent*)
{
   regenerateLines();
}

void MoveListView::regenerateLines()
{
   QString text;
   text.reserve(2048);
   //
   lastLine_.clear();
   //
   int halfMoveMargin = fontMetrics().width("44. XXX");
   //
   for(unsigned i=0; i<moves_.size(); ++i)
   {
      bool isWhiteMove = (i%2)==0;
      //
      QString line;
      line.reserve(128);
      if(!lastLine_.isEmpty())
      {
         line.append(lastLine_);
         line.append(' ');
      }
      //
      line.append(moves_[i]);
      //
      int maxWidth = isWhiteMove ? rect().width()-halfMoveMargin : rect().width();
      //
      if(fontMetrics().width(line)>maxWidth)
      {
         if(lastLine_.isEmpty())
         {
            text.append(line);
            text.append('\n');
         }
         else
         {
            if(isWhiteMove)
            {
               text.append(lastLine_);
               lastLine_ = moves_[i];
            }
            else
            {
               text.append(lastLine_.left(lastLine_.length()-moves_[i-1].length()));
               lastLine_ = moves_[i-1];
               lastLine_.append(' ');
               lastLine_.append(moves_[i]);
            }
            text.append('\n');
         }
      }
      else
      {
         lastLine_ = line;
      }
   }
   //
   if(!lastLine_.isEmpty())
   {
      text.append(lastLine_);
   }
   //
   setPlainText(text);
   moveCursor(QTextCursor::End);
   ensureCursorVisible();
}

void MoveListView::dropMoves(unsigned count)
{
   if(count>=moves_.size())
      clearMoves();
   else
   {
      while(count--)
      {
         moves_.removeLast();
      }
      //
      regenerateLines();
   }
}
