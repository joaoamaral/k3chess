#include "ChessBoardView.h"
#include "MoveSelectionHelper.h"
#include "Settings.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>

// @@note: pieces image is a bmp/png/jpg image with 7 x 4 sare items
// (defining both pieces and empty squares)
// recommended square size is 64 x 64 or 128 x 128
// (piece sequence (left to right): _KQRBNP,
// row sequence (top to bottom): w/w, b/w, w/b, b/b)

namespace
{

const int cDefaultCoordsBorderPadding = 16;
const QColor cBoardBorderColor(213, 196, 170);
const QColor cBoardInnerFrameColor(148, 119, 95);
const QColor cMoveHintColor(0x30, 0x30, 0x30);

}

ChessBoardView::ChessBoardView(QWidget *parent) :
   QWidget(parent),
   flipped_(false), cellSelectMode_(selectNone),
   drawCoords_(false), padding_(0),
   drawMoveArrow_(false),
   showMoveHints_(false),
   moveSelectionSuspended_(false),
   keyCoordSelect_(false),
   keyPieceSelect_(false),
   quickSingleMoveSelection_(false),
   directCoordinateInput_(false),
   lastKeyType_(ktOther)
{
   setMinimumSize(128, 128);
   setMaximumSize(2048, 2048);
   //
   initGraphicObjects();
}

void ChessBoardView::initGraphicObjects()
{
   thinPen_.setCapStyle(Qt::SquareCap);
   thinPen_.setJoinStyle(Qt::MiterJoin);
   thinPen_.setStyle(Qt::SolidLine);
   thinPen_.setColor(Qt::black);
   thinPen_.setWidth(2);
   //
   thickPen_.setCapStyle(Qt::SquareCap);
   thickPen_.setJoinStyle(Qt::MiterJoin);
   thickPen_.setStyle(Qt::SolidLine);
   thickPen_.setColor(Qt::black);
   thickPen_.setWidth(4);
   //
   borderBrush_ = QBrush(cBoardBorderColor);
   //
   horizCoordsFont_ = QFont("Sans", 7, QFont::Bold);
   vertCoordsFont_ = QFont("Sans", 6, QFont::Bold);
}

void ChessBoardView::updateScaledPieces()
{
   scaledPieces_.clear();
   scaledPieces_.reserve(2*2*7);
   //
   int newPieceSize = (width()-padding_*2)/position_.maxRow();
   QImage pieces = originalPieces_.scaled(newPieceSize * 7, newPieceSize * 4, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   //
   for(unsigned cc=0; cc<=1; ++cc)
   {
      for(unsigned pc=0; pc<=1; ++pc)
      {
         for(PieceType pt=ptNone; pt<=ptPawn; ++pt)
         {
            QRect pieceRect(pt*newPieceSize, (pc+2*cc)*newPieceSize, newPieceSize, newPieceSize);
            scaledPieces_.push_back(pieces.copy(pieceRect));
         }
      }
   }
}

int ChessBoardView::padding() const
{
   return padding_;
}

void ChessBoardView::updatePosition(const ChessPosition& position, // updates position and accompanying data (if provided)
                                    const CoordPair& lastMove,
                                    const ChessMoveMap& possibleMoves)
{
   int oldmaxCol = position_.maxCol();
   position_ = position;
   lastMove_ = lastMove;
   possibleMoves_ = possibleMoves;
   //
   if(position_.maxCol()!=oldmaxCol)
   {
      updateScaledPieces();
   }
   //
   repaint(rect());
}

bool ChessBoardView::enter()
{
   if(moveSelectionSuspended_)
   {
      moveSelectionSuspended_ = false;
      beginMoveSelection();
      return true;
   }
   else
   {
      return false;
   }
}

void ChessBoardView::beginMoveSelection()
{
   assert(cellSelectMode_==selectNone);
   //
   if(focusPolicy()!=Qt::NoFocus)
   {
      setFocus();
   }
   //
   QRect repaintRect(getMoveRect(move_));
   //
   cellSelectMode_ = selectSource;
   lastKeyType_ = ktOther;
   //
   if(lastCursorPos_!=ChessCoord() &&
       position_.inRange(lastCursorPos_))
   {
      move_.from = lastCursorPos_;
      move_.to = move_.from;
   }
   else if(position_.sideToMove() == pcWhite)
   {
      move_.from.row = 2;
      move_.from.col = (position_.maxCol()+1)/2;
      move_.to = move_.from;
   }
   else
   {
      move_.from.row = position_.maxRow()-1;
      move_.from.col = position_.maxCol()+1-(position_.maxCol()+1)/2;
      move_.to = move_.from;
   }
   //
   repaint(repaintRect.united(getMoveRect(move_)));
}

void ChessBoardView::setFlipped(bool value)
{
   if(flipped_==value) return;
   flipped_ = value;
   repaint(rect());
}

void ChessBoardView::setPadding(unsigned value)
{
   if(padding_==value) return;
   padding_ = value;
   repaint(rect());
}

QRect ChessBoardView::getMoveRect(const CoordPair& move) const
{
   return getCellRect(move.from).united(getCellRect(move.to));
}

QRect ChessBoardView::getCellRect(ChessCoord coord) const
{
   if(!position_.inRange(coord)) return QRect();
   //
   if(flipped_)
   {
      coord.col = position_.maxCol()-coord.col+1;
      coord.row = position_.maxRow()-coord.row+1;
   }
   //
   int x0, x1, y0, y1;
   //
   x0 = (width()-padding_*2)*(coord.col-1)/position_.maxCol() + padding_;
   //
   if(coord.col==position_.maxCol())
   {
      x1 = width()-padding_;
   }
   else
   {
      x1 = x0 + (width()-padding_*2)/position_.maxCol();
   }
   //
   y0 = (height()-padding_*2)*(position_.maxRow()-coord.row)/position_.maxRow() + padding_;
   //
   if(coord.row==1)
   {
      y1 = height()-padding_;
   }
   else
   {
      y1 = y0 + (height()-padding_*2)/position_.maxRow();
   }
   //
   return QRect(x0, y0, x1-x0, y1-y0);
}

ChessCoord ChessBoardView::chessCoordFromPoint(const QPoint& p) const
{
   ChessCoord coord(
        (p.x()-padding_)*position_.maxCol()/(width()-padding_*2)+1,
        (p.y()-padding_)*position_.maxRow()/(height()-padding_*2)+1);
   //
   if(flipped_)
   {
      coord.col = position_.maxCol()-coord.col+1;
   }
   else
   {
      coord.row = position_.maxRow()-coord.row+1;
   }
   //
   if(position_.inRange(coord))
   {
      return coord;
   }
   else
   {
      return ChessCoord();
   }
}

void ChessBoardView::keyPressEvent(QKeyEvent *event)
{
   event->accept(); // consume event if it arrived via default event handing mechanism
   processKey(Qt::Key(event->key()));
}

void ChessBoardView::columnSelect(int col)
{
   if(cellSelectMode_==selectNone) return;
   //
   QRect repaintRect(getCellRect(move_.to));
   //
   if(cellSelectMode_==selectSource)
   {
      ChessCoord c = MoveSelectionHelper::getNextPieceByCol(
         position_, possibleMoves_, move_.from, col, lastKeyType_==ktPiece);
      if(c==move_.from) return;
      move_.from = c;
      move_.to = c;
   }
   else
   {
      bool isUnique = false;
      ChessCoord c = MoveSelectionHelper::getNextTargetByCol(
         position_, possibleMoves_, move_, col, isUnique);
      if(c==move_.to) return;
      move_.to = c;
      if(isUnique)
      {
         select();
      }
   }
   //
   repaint(repaintRect.united(getCellRect(move_.to)));
}

void ChessBoardView::rowSelect(int row)
{
   if(cellSelectMode_==selectNone) return;
   //
   if(row<1 || row>position_.maxRow()) return;
   //
   QRect repaintRect(getCellRect(move_.to));
   //
   if(cellSelectMode_==selectSource)
   {
      if(move_.from.row!=row)
      {
         move_.from.row = row;
         move_.to.row = row;
      }
      else
      {
         return;
      }
   }
   else
   {
      bool isUnique = false;
      ChessCoord c = MoveSelectionHelper::getNextTargetByRow(
         position_, possibleMoves_, move_, row, isUnique);
      if(c==move_.to) return;
      move_.to = c;
      if(isUnique)
      {
         select();
      }
   }
   //
   repaint(repaintRect.united(getCellRect(move_.to)));
}

void ChessBoardView::pieceSelect(PieceType pt)
{
   if(cellSelectMode_==selectNone) return;
   //
   QRect repaintRect(getCellRect(move_.to));
   //
   if(cellSelectMode_==selectSource)
   {
      ChessCoord c = MoveSelectionHelper::getNextPieceByType(
         position_, possibleMoves_, move_.from, pt);
      if(c==move_.from) return;
      move_.from = c;
      move_.to = c;
   }
   else
   {
      bool isUnique = false;
      ChessCoord c = MoveSelectionHelper::getNextTargetByPiece(
         position_, possibleMoves_, move_, pt, isUnique);
      if(c==move_.to) return;
      move_.to = c;
      if(isUnique)
      {
         select();
      }
   }
   //
   repaint(repaintRect.united(getCellRect(move_.to)));
}

bool ChessBoardView::checkColumnSelectionKey(Qt::Key key)
{
   switch(key)
   {
      case Qt::Key_A:  columnSelect(1);  break;
      case Qt::Key_B:  columnSelect(2);  break;
      case Qt::Key_C:  columnSelect(3);  break;
      case Qt::Key_D:  columnSelect(4);  break;
      case Qt::Key_E:  columnSelect(5);  break;
      case Qt::Key_F:  columnSelect(6);  break;
      case Qt::Key_G:  columnSelect(7);  break;
      case Qt::Key_H:  columnSelect(8);  break;
      default:
         return false;
   }
   return true;
}

bool ChessBoardView::checkRowSelectionKey(Qt::Key key)
{
   switch(key)
   {
      case Qt::Key_1:  rowSelect(1);  break;
      case Qt::Key_2:  rowSelect(2);  break;
      case Qt::Key_3:  rowSelect(3);  break;
      case Qt::Key_4:  rowSelect(4);  break;
      case Qt::Key_5:  rowSelect(5);  break;
      case Qt::Key_6:  rowSelect(6);  break;
      case Qt::Key_7:  rowSelect(7);  break;
      case Qt::Key_8:  rowSelect(8);  break;
      default:
         return false;
   }
   return true;
}

bool ChessBoardView::checkPieceSelectionKey(Qt::Key key)
{
   if(cellSelectMode_==selectNone) return false;
   //
   switch(key)
   {
      case Qt::Key_K:  pieceSelect(ptKing); break;
      case Qt::Key_Q:  pieceSelect(ptQueen); break;
      case Qt::Key_R:  pieceSelect(ptRook); break;
      case Qt::Key_I:  /*fallthrough*/
      case Qt::Key_B:  pieceSelect(ptBishop); break;
      case Qt::Key_N:  pieceSelect(ptKnight); break;
      case Qt::Key_P:  pieceSelect(ptPawn); break;
      default:
         return false;
   }
   return true;
}

bool ChessBoardView::processKey(Qt::Key key)
{
   if(cellSelectMode_==selectNone) return false;
   //
   if(directCoordinateInput_)
   {
      return processDirectCoordinateKey(key);
   }
   else
   {
      return processStandardModeKey(key);
   }
   //
   lastKeyType_ = ktOther;
   //
   return false;
}

bool ChessBoardView::expectDirectColKey(Qt::Key key, ChessCoord& coord)
{
   switch(key)
   {
      case Qt::Key_A:   coord.col = 1;  break;
      case Qt::Key_B:   coord.col = 2;  break;
      case Qt::Key_C:   coord.col = 3;  break;
      case Qt::Key_D:   coord.col = 4;  break;
      case Qt::Key_E:   coord.col = 5;  break;
      case Qt::Key_F:   coord.col = 6;  break;
      case Qt::Key_G:   coord.col = 7;  break;
      case Qt::Key_H:   coord.col = 8;  break;
      default:
         return false;
   }
   //
   lastKeyType_ = ktColumn;
   //
   return true;
}

bool ChessBoardView::expectDirectRowKey(Qt::Key key, ChessCoord &coord)
{
   switch(key)
   {
      case Qt::Key_Q:
      case Qt::Key_1:   coord.row = 1;  break;
      case Qt::Key_W:
      case Qt::Key_2:   coord.row = 2;  break;
      case Qt::Key_E:
      case Qt::Key_3:   coord.row = 3;  break;
      case Qt::Key_R:
      case Qt::Key_4:   coord.row = 4;  break;
      case Qt::Key_T:
      case Qt::Key_5:   coord.row = 5;  break;
      case Qt::Key_Y:
      case Qt::Key_6:   coord.row = 6;  break;
      case Qt::Key_U:
      case Qt::Key_7:   coord.row = 7;  break;
      case Qt::Key_I:
      case Qt::Key_8:   coord.row = 8;  break;
      default:
         return false;
   }
   //
   lastKeyType_ = ktRow;
   //
   return true;
}

QRect ChessBoardView::getBaseRect(ColValue col)
{
   return getCellRect(ChessCoord(col, flipped_ ? position_.maxRow() : 1));
}

bool ChessBoardView::processDirectCoordinateSource(Qt::Key key)
{
   switch(lastKeyType_)
   {
      case ktOther:
         if(expectDirectColKey(key, move_.from))
         {
            QRect fromBaseRect = getBaseRect(move_.from.col);
            repaint(fromBaseRect);
         }
         else
            return false;
         break;
      case ktColumn:
         {
            QRect oldFromBaseRect = getBaseRect(move_.from.col);
            if(expectDirectRowKey(key, move_.from))
            {
               ChessPiece piece = position_.cell(move_.from);
               if(piece.type()!=ptNone && piece.color()==position_.sideToMove())
               {
                  lastKeyType_ = ktOther;
                  cellSelectMode_ = selectTarget;
                  repaint(getCellRect(move_.from).united(oldFromBaseRect));
               }
               else
               {
                  if(quickSingleMoveSelection_)
                  {
                     CoordPair move;
                     if(possibleMoves_.getUniqueMove(move_.from, move))
                     {
                        cellSelectMode_ = selectNone;
                        emit moveEntered(move);
                        return true;
                     }
                  }
                  lastKeyType_ = ktOther;
                  repaint(oldFromBaseRect);
               }
            }
            else
               return false;
         }
         break;
      default:
         {
            QRect fromBaseRect = getBaseRect(move_.from.col);
            lastKeyType_ = ktOther;
            repaint(fromBaseRect);
         }
         return false;
   }
   //
   return true;
}

bool ChessBoardView::processDirectCoordinateTarget(Qt::Key key)
{
   switch(lastKeyType_)
   {
      case ktOther:
         if(expectDirectColKey(key, move_.to))
         {
            QRect toBaseRect = getBaseRect(move_.to.col);
            repaint(toBaseRect);
         }
         else
            return false;
         break;
      case ktColumn:
         {
            QRect oldToBaseRect = getBaseRect(move_.to.col);
            if(expectDirectRowKey(key, move_.to))
            {
               if(possibleMoves_.contains(move_))
               {
                  cellSelectMode_ = selectNone;
                  emit moveEntered(move_);
                  return true;
               }
               else
               {
                  cellSelectMode_ = selectSource;
                  lastKeyType_ = ktOther;
                  repaint(getCellRect(move_.from).united(oldToBaseRect));
               }
            }
            else
               return false;
         }
         break;
      default:
         {
            QRect oldToBaseRect = getBaseRect(move_.to.col);
            lastKeyType_ = ktOther;
            repaint(getCellRect(move_.from).united(oldToBaseRect));
            break;
         }
         return false;
   }
   //
   return true;
}

bool ChessBoardView::processDirectCoordinateKey(Qt::Key key)
{
   switch(cellSelectMode_)
   {
      case selectSource:
         return processDirectCoordinateSource(key);
      case selectTarget:
         return processDirectCoordinateTarget(key);
   }
   //
   return false;
}

bool ChessBoardView::processStandardModeKey(Qt::Key key)
{
   switch(key)
   {
      case Qt::Key_Left:  moveCursor(cursorLeft); break;
      case Qt::Key_Up:    moveCursor(cursorUp); break;
      case Qt::Key_Right: moveCursor(cursorRight); break;
      case Qt::Key_Down:  moveCursor(cursorDown); break;
      case Qt::Key_Select:  select(); break;
      case Qt::Key_Escape:
      case Qt::Key_Cancel:  if(!cancel()) return false; break;
      default:
         if(keyCoordSelect_)
         {
            if(checkColumnSelectionKey(key))
            {
               lastKeyType_ = ktColumn;
               return true;
            }
            else if(checkRowSelectionKey(key))
            {
               lastKeyType_ = ktRow;
               return true;
            }
         }
         if(keyPieceSelect_)
         {
            if(checkPieceSelectionKey(key))
            {
               lastKeyType_ = ktPiece;
               return true;
            }
         }
         return false;
   }
   //
   lastKeyType_ = ktOther;
   //
   return true;
}

void ChessBoardView::mousePressEvent(QMouseEvent *event)
{
   if(cellSelectMode_==selectNone) return;
   //
   ChessCoord coord = chessCoordFromPoint(event->pos());
   //
   QRect moveRect(getMoveRect(move_));
   //
   if(coord==ChessCoord())
   {
      if(cellSelectMode_==selectTarget)
      {
         move_.to = move_.from;
         repaint(moveRect);
         //
         select();
      }
   }
   else
   {
      switch(cellSelectMode_)
      {
         case selectSource:
            move_.from = coord;
            move_.to = coord;
            repaint(moveRect.united(getMoveRect(move_)));
            break;
         case selectTarget:
            move_.to = coord;
            repaint(moveRect.united(getMoveRect(move_)));
            break;
         default:
            return;
      }
      //
      select();
   }
}

void ChessBoardView::paintEvent(QPaintEvent *event)
{
   QPixmap pix(size());
   QPainter buffer(&pix);
   //
   drawBoard(buffer, event->rect());
   drawBorder(buffer, event->rect());
   //
   QPainter painter(this);
   painter.drawPixmap(QPoint(0, 0), pix);
}

void ChessBoardView::resizeEvent(QResizeEvent *)
{
   scaledPieces_.clear(); // will be updated on next paint
}

void ChessBoardView::drawColumnHighlight(QPainter& painter, ColValue col, const QRect& clipRect)
{
   QRect rect = getBaseRect(col);
   if(!rect.intersects(clipRect)) return;
   rect.setTop(rect.bottom()-4);
   painter.setBrush(Qt::black);
   painter.setPen(Qt::NoPen);
   painter.drawRect(rect);
}

void ChessBoardView::drawBoard(QPainter& painter, const QRect& clipRect)
{
   if(scaledPieces_.empty())
   {
      updateScaledPieces();
   }
   //
   ChessCoord coord;
   //
   for(coord.row=1; coord.row<=position_.maxRow(); ++coord.row)
   {
      for(coord.col=1; coord.col<=position_.maxCol(); ++coord.col)
      {
         QRect cellRect = getCellRect(coord);
         if(!cellRect.intersects(clipRect)) continue;
         //
         drawPiece(painter, cellRect, position_.cell(coord), (coord.row+coord.col)%2==0);
      }
   }
   //
   if(directCoordinateInput_)
   {
      switch(cellSelectMode_)
      {
         case selectSource:
            if(lastKeyType_==ktColumn)
            {
               drawColumnHighlight(painter, move_.from.col, clipRect);
            }
            break;
         case selectTarget:
            {
               QRect cellRect = getCellRect(move_.from);
               if(cellRect.intersects(clipRect))
               {
                 drawCellHighlight(painter, cellRect, true);
               }
               if(lastKeyType_==ktColumn)
               {
                  drawColumnHighlight(painter, move_.to.col, clipRect);
               }
            }
            break;
         default:
            break;
      }
   }
   else
   {
      if(cellSelectMode_!=selectNone)
      {
         if(move_.from != ChessCoord() && move_.from != move_.to)
         {
            QRect cellRect = getCellRect(move_.from);
            if(cellRect.intersects(clipRect))
            {
               drawCellHighlight(painter, cellRect, false);
            }
         }
         if(move_.to != ChessCoord())
         {
            QRect cellRect = getCellRect(move_.to);
            if(cellRect.intersects(clipRect))
            {
               drawCellHighlight(painter, cellRect, true);
            }
         }
      }
   }
   //
   if(drawMoveArrow_ &&
      lastMove_.from != lastMove_.to)
   {
      QRect fromRect = getCellRect(lastMove_.from);
      QRect toRect = getCellRect(lastMove_.to);
      if(fromRect.united(toRect).intersects(clipRect))
      {
         drawMoveArrow(painter, fromRect, toRect);
      }
   }
   //
   if(showMoveHints_ && cellSelectMode_==selectTarget)
   {
      drawMoveHints(painter, move_.from, clipRect);
   }
}

void ChessBoardView::drawMoveHints(QPainter& painter, ChessCoord& coord, const QRect& clipRect)
{
   ChessMoveMap::const_iterator_pair range =
         possibleMoves_.getMovesFrom(coord);
   //
   if(range.first==range.second) return;
   //
   painter.setPen(Qt::NoPen);
   painter.setBrush(cMoveHintColor);
   //
   for(;range.first!=range.second;++range.first)
   {
      QRect rect = getCellRect(range.first->to);
      if(!rect.intersects(clipRect)) continue;
      int d = rect.width()/8;
      QRect rmark(rect.left()+2, rect.top()+2, d, d);
      painter.drawEllipse(rmark);
   }
}

void ChessBoardView::drawPiece(QPainter& painter, const QRect& rect, ChessPiece piece, bool blackCell)
{
   unsigned index = (blackCell ? 14 : 0) + (piece.color()==pcWhite ? 0 : 7) + piece.type();
   //
   if(index<scaledPieces_.size())
   {
      painter.drawImage(rect.left(), rect.top(), scaledPieces_[index]);
   }
}

void ChessBoardView::drawBorder(QPainter& painter, const QRect& clipRect)
{
   if(padding_>0)
   {
      //
      QRect left(0, padding_, padding_, height()-padding_*2);
      QRect right(width()-padding_, padding_, padding_, height()-padding_*2);
      QRect top(0, 0, width(), padding_);
      QRect bottom(0, height()-padding_, width(), padding_);
      //
      painter.setPen(Qt::NoPen);
      painter.setBrush(borderBrush_);
      if(top.intersects(clipRect)) painter.drawRect(top);
      if(left.intersects(clipRect)) painter.drawRect(left);
      if(bottom.intersects(clipRect)) painter.drawRect(bottom);
      if(right.intersects(clipRect)) painter.drawRect(right);
      //
      QRect innerFrame(padding_-1, padding_-1, width()-(padding_-1)*2, height()-(padding_-1)*2);
      //
      painter.setPen(cBoardInnerFrameColor);
      painter.setBrush(Qt::NoBrush);
      painter.drawRect(innerFrame);
      //
      if(drawCoords_)
      {
         drawCoords(painter, clipRect);
      }
   }
   else
   {
      QRect r(getCellRect(ChessCoord(1, 1)).united(
                 getCellRect(ChessCoord(position_.maxCol(), position_.maxRow()))));
      r.setRight(r.right()-1);
      r.setBottom(r.bottom()-1);
      painter.setPen(cBoardInnerFrameColor);
      painter.setBrush(Qt::NoBrush);
      painter.drawRect(r);
   }
}

void ChessBoardView::drawCoords(QPainter& painter, const QRect& clipRect)
{
   painter.setPen(Qt::black);
   //
   ChessCoord coord(1, 1);
   //
   QRect leftCoords(0, padding_, padding_, height()-padding_*2);
   QRect bottomCoords(padding_, height()-padding_, width()-padding_*2, padding_);
   //
   painter.setRenderHint(QPainter::TextAntialiasing);
   //
   painter.setFont(horizCoordsFont_);
   //
   if(leftCoords.intersects(clipRect))
   {
      for(coord.col=1; coord.col<=position_.maxCol(); ++coord.col)
      {
         QString s(QChar(colToChar(coord.col)));
         QRect rect = getCellRect(coord);
         QRect textRect(rect.left(), height()-padding_, rect.width(), padding_);
         painter.drawText(textRect, Qt::AlignCenter, s);
      }
   }
   //
   coord.col = 1;
   //
   painter.setFont(vertCoordsFont_);
   //
   if(bottomCoords.intersects(clipRect))
   {
      for(coord.row=1; coord.row<=position_.maxRow(); ++coord.row)
      {
         QString s(QChar(rowToChar(coord.row)));
         QRect rect = getCellRect(coord);
         QRect textRect(0, rect.top(), padding_, rect.height());
         painter.drawText(textRect, Qt::AlignCenter, s);
      }
   }
}

QRect shrink(const QRect& rect, int d)
{
   return QRect(rect.left()+d, rect.top()+d, rect.width()-d*2, rect.height()-d*2);
}

QPoint midPoint(const QRect& rect)
{
   return QPoint((rect.left()+rect.right())/2, (rect.top()+rect.bottom())/2);
}

bool within(int x, int a, int b, int tol)
{
   int lo = qMin(a, b);
   int hi = qMax(a, b);
   return x >= lo-tol && x <= hi+tol;
}

int crossProduct(const QPoint& a, const QPoint& b)
{
   return a.x() * b.y() - a.y() * b.x();
}

bool intersectLines(const QPoint& a1, const QPoint& a2, const QPoint& b1, const QPoint& b2, QPoint& q)
{
   QPoint ra(a2.x()-a1.x(), a2.y()-a1.y());
   QPoint rb(b2.x()-b1.x(), b2.y()-b1.y());
   //
   int w = crossProduct(ra, rb);
   //
   if(w==0) return false; // lines are parallel
   //
   QPoint d(b1.x()-a1.x(), b1.y()-a1.y());
   //
   int z = crossProduct(d, rb);
   //
   q.setX(a1.x() + ra.x()*z/w);
   q.setY(a1.y() + ra.y()*z/w);
   //
   bool ok = within(q.x(), a1.x(), a2.x(), 1) &&
          within(q.y(), a1.y(), a2.y(), 1) &&
          within(q.x(), b1.x(), b2.x(), 1) &&
          within(q.y(), b1.y(), b2.y(), 1);
   //
   return ok;
}

QPoint touchPoint(const QPoint& p, const QRect& rect)
{
   QPoint pc = midPoint(rect);
   //
   QPoint q;
   //
   if(intersectLines(p, pc, rect.topLeft(), rect.topRight(), q) ||
      intersectLines(p, pc, rect.topLeft(), rect.bottomLeft(), q) ||
      intersectLines(p, pc, rect.bottomLeft(), rect.bottomRight(), q) ||
      intersectLines(p, pc, rect.topRight(), rect.bottomRight(), q))
   {
      return q;
   }
   else
   {
      return p; // actually no touch point with the current tolerance
   }
}

void ChessBoardView::drawCellHighlight(QPainter& painter, const QRect& rect, bool bold)
{
   QPen& pen = bold ? thickPen_ : thinPen_;
   //
   painter.setRenderHint(QPainter::Antialiasing, false);
   //
   painter.setBrush(QBrush());
   painter.setPen(pen);
   painter.drawRect(shrink(rect, pen.width()/2));
}

void ChessBoardView::drawMoveArrow(QPainter& painter, const QRect& fromRect, const QRect& toRect)
{
   painter.setPen(thinPen_);
   //
   QPoint point1 = midPoint(fromRect);
   QPoint point2 = touchPoint(point1, toRect);
   //
   // @@todo: add triangle arrow
   //
   painter.setRenderHint(QPainter::Antialiasing);
   //
   painter.drawLine(point1, point2);
}

void ChessBoardView::flipDirection(CursorDirection& dir)
{
   switch(dir)
   {
      case cursorLeft:  dir = cursorRight; break;
      case cursorRight:  dir = cursorLeft; break;
      case cursorUp:  dir = cursorDown; break;
      case cursorDown: dir = cursorUp; break;
   }
}

void ChessBoardView::moveCursor(CursorDirection dir)
{
   QRect repaintRect;
   //
   if(flipped_) flipDirection(dir);
   //
   switch(cellSelectMode_)
   {
      case selectNone: return;
      case selectSource:
         repaintRect = getCellRect(move_.from);
         advanceCoord(move_.from, dir);
         move_.to = move_.from;
         repaintRect = repaintRect.united(getCellRect(move_.from));
         break;
      case selectTarget:
         repaintRect = getCellRect(move_.to);
         advanceCoord(move_.to, dir);
         repaintRect = repaintRect.united(getCellRect(move_.to));
      break;
   }
   //
   repaint(repaintRect);
}

void ChessBoardView::select()
{
   switch(cellSelectMode_)
   {
      case selectSource:
         {
            ChessPiece piece = position_.cell(move_.from);
            if(possibleMoves_.hasMovesFrom(move_.from))
            {
               cellSelectMode_ = selectTarget;
               //
               if(quickSingleMoveSelection_ &&
                  possibleMoves_.getUniqueMove(move_.from, move_))
               {
                  select();
                  return;
               }
               if(showMoveHints_)
               {
                  repaint(boardCellsRect());
               }
            }
         }
         break;
      case selectTarget:
         {
            if(move_.to == move_.from)
            {
               cellSelectMode_ = selectSource;
               //
               if(showMoveHints_)
               {
                  repaint(boardCellsRect());
               }
            }
            else if(possibleMoves_.contains(move_))
            {
               lastCursorPos_ = move_.to;
               cellSelectMode_ = selectNone;
               emit moveEntered(move_);
            }
         }
         break;
      case selectNone:
         break;
   }
}

QRect ChessBoardView::boardCellsRect() const
{
   return getCellRect(ChessCoord(1, 1)).united(
      getCellRect(ChessCoord(position_.maxCol(), position_.maxRow())));
}

bool ChessBoardView::cancel()
{
   if(cellSelectMode_==selectTarget)
   {
      QRect repaintRect = showMoveHints_ ? boardCellsRect() : getMoveRect(move_);
      //
      cellSelectMode_ = selectSource;
      move_.from = move_.to;
      //
      repaint(repaintRect);
      //
      return true;
   }
   else
      return false;
}

void ChessBoardView::advanceCoord(ChessCoord& coord, CursorDirection dir)
{
   switch(dir)
   {
      case cursorLeft:  if(--coord.col==0) coord.col = position_.maxCol(); break;
      case cursorDown:  if(--coord.row==0) coord.row = position_.maxRow();  break;
      case cursorRight: if(++coord.col>position_.maxCol()) coord.col = 1; break;
      case cursorUp:    if(++coord.row>position_.maxRow()) coord.row = 1; break;
   }
}

void ChessBoardView::cancelMoveSelection()
{
   if(cellSelectMode_==selectNone) return;
   QRect repaintRect(getCellRect(move_.from).united(getCellRect(move_.to)));
   lastCursorPos_ = move_.to;
   cellSelectMode_ = selectNone;
   repaint(repaintRect);
}

bool ChessBoardView::hasCursor() const
{
   return cellSelectMode_ != selectNone;
}

void ChessBoardView::suspendSelection()
{
   moveSelectionSuspended_ = true;
   cancelMoveSelection();
}

void ChessBoardView::setPiecesImage(const QImage& image)
{
   originalPieces_ = image;
   updateScaledPieces();
   repaint(rect());
}

void ChessBoardView::setDrawCoords(bool value)
{
   if(drawCoords_==value) return;
   //
   drawCoords_ = value;
   //
   if(drawCoords_)
      padding_ = cDefaultCoordsBorderPadding;
   else
      padding_ = 0;
   //
   updateScaledPieces();
   //
   repaint(rect());
}

void ChessBoardView::setDrawMoveArrow(bool value)
{
   if(drawMoveArrow_==value) return;
   drawMoveArrow_ = value;
   repaint(getMoveRect(lastMove_));
}

void ChessBoardView::setShowMoveHints(bool value)
{
   if(showMoveHints_==value) return;
   showMoveHints_ = value;
   repaint(rect());
}

void ChessBoardView::setKeyCoordSelect(bool value)
{
   keyCoordSelect_ = value;
}

void ChessBoardView::setKeyPieceSelect(bool value)
{
   keyPieceSelect_ = value;
}

void ChessBoardView::setQuickSingleMoveSelection(bool value)
{
   quickSingleMoveSelection_ = value;
}

void ChessBoardView::setInitialCursorPos(ChessCoord coord)
{
   lastCursorPos_ = coord;
}

void ChessBoardView::setDirectCoordinateInput(bool value)
{
   directCoordinateInput_ = value;
}
