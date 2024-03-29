#include "CapturedPiecesView.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyle>

namespace
{
const QColor cDefaultLinePenColor(0xD0, 0xD0, 0xD0);
}

CapturedPiecesView::CapturedPiecesView(QWidget *parent) :
   QWidget(parent),
   whiteCaptPieces_(ptPawn-ptQueen+1),
   blackCaptPieces_(ptPawn-ptQueen+1),
   linePen_(cDefaultLinePenColor),
   numberFont_("Sans", 9)
{
}

void CapturedPiecesView::updateContents(
   const ChessPosition& initialPosition,
   const ChessPosition& currentPosition)
{
   std::vector<int> oldWhite = whiteCaptPieces_;
   std::vector<int> oldBlack = blackCaptPieces_;
   //
   whiteCaptPieces_.clear();
   blackCaptPieces_.clear();
   //
   whiteCaptPieces_.resize(ptPawn-ptQueen+1, 0);
   blackCaptPieces_.resize(ptPawn-ptQueen+1, 0);
   //
   countFromPosition(initialPosition, 1);
   countFromPosition(currentPosition, -1);
   //
   bool whiteChanged = (oldWhite != whiteCaptPieces_);
   bool blackChanged = (oldBlack != blackCaptPieces_);
   //
   if(whiteChanged && blackChanged)
   {
      repaint();
   }
   else if(whiteChanged)
   {
      repaint(whiteRect_);
   }
   else if(blackChanged)
   {
      repaint(blackRect_);
   }
}

void CapturedPiecesView::resizeEvent(QResizeEvent *)
{
   updateLayout();
}

void CapturedPiecesView::draw(QPainter &painter, const QRect &clipRect)
{
   QColor textColor = palette().color(QPalette::Text);
   QColor backgroundColor = palette().color(QPalette::Background);
   //
   painter.setPen(Qt::NoPen);
   painter.setBrush(backgroundColor);
   painter.drawRect(clipRect);
   //
   painter.setPen(linePen_);
   painter.drawLine(0, 0, rect().width(), 0);
   //
   painter.setBackgroundMode(Qt::TransparentMode);
   painter.setPen(textColor);
   painter.setFont(numberFont_);
   //
   if(clipRect.intersects(whiteRect_))
   {
      drawPieces(painter, whiteRect_, whiteCaptPieces_, pcWhite);
   }
   //
   if(clipRect.intersects(blackRect_))
   {
      drawPieces(painter, blackRect_, blackCaptPieces_, pcBlack);
   }
}

void drawOutlinedText(QPainter& painter, QColor textColor, QColor outlineColor,
                      const QPoint& p, const QString& text)
{
   painter.setPen(outlineColor);
   painter.drawText(p.x()-1, p.y()-1, text);
   painter.drawText(p.x(), p.y()-1, text);
   painter.drawText(p.x()-1, p.y(), text);
   painter.setPen(textColor);
   painter.drawText(p.x(), p.y(), text);
}

void CapturedPiecesView::drawPieces(QPainter& painter, const QRect& rect,
                                    const std::vector<int>& captured,
                                    PieceColor color)
{
   QFontMetrics fm(numberFont_);
   QSize sz(fm.width("8"), fm.height());
   QRect r(rect.left(), rect.top(), (rect.width())/5, rect.height());
   const std::vector<QImage>& images = color==pcWhite ? whitePieces_ : blackPieces_;
   for(unsigned i=0; i<=(ptPawn-ptQueen); ++i)
   {
      int ncap = captured[i];
      //
      if(ncap>0)
      {
         QPoint p(r.left()+r.width()/6, r.top()+r.height()/6);
         //painter.drawText(p.x(), p.y()+12, QString::number(i));
         const QImage& pieceImage = images[i];
         painter.drawImage(p, pieceImage);

         if(ncap>1)
         {
            QPoint p2(r.right()-sz.width()*72/100, r.bottom()-sz.height()/8);
            //drawOutlinedText(painter, painter.pen().color(), Qt::white,
            //                 p2, QString::number(ncap));
            painter.drawText(p2, QString::number(ncap));
            //style()->drawItemText(&painter, r2, Qt::AlignVCenter, palette(), true, QString::number(captured[i]), QPalette::WindowText);
         }
      }
      r.moveLeft(r.right());
   }
}

void CapturedPiecesView::paintEvent(QPaintEvent *event)
{
   QPixmap pix(size());
   QPainter buffer(&pix);
   QPainter painter(this);
   //
   buffer.setClipRect(event->rect());
   buffer.setFont(painter.font());
   draw(buffer, event->rect());
   //
   painter.drawPixmap(QPoint(0, 0), pix);
}

void CapturedPiecesView::resizeImages(int size)
{
   /*numberFont_.setPixelSize(size*2/5);*/
   //
   QImage resizedPieces = originalPieces_.scaledToHeight(
            size*4, Qt::SmoothTransformation);
   //
   whitePieces_.clear();
   whitePieces_.reserve(ptPawn-ptQueen+1);
   //
   blackPieces_.clear();
   blackPieces_.reserve(ptPawn-ptQueen+1);
   //
   for(int i=0; i<=(ptPawn-ptQueen); ++i)
   {
      int x1 = (i+ptQueen)*size;
      whitePieces_.push_back(resizedPieces.copy(x1, 0, size, size));
      blackPieces_.push_back(resizedPieces.copy(x1, size, size, size));
   }
}

void CapturedPiecesView::setPiecesImage(const QImage &image)
{
   originalPieces_ = image;
   updateLayout();
}

void CapturedPiecesView::updateLayout()
{
   int w = rect().width();
   int h = rect().height();
   //
   int s = w/11; // 5 a side plus divider
   if(s > h) s = h;
   //
   int sblock = s*5;
   //
   whiteRect_ = QRect(0, h-s, sblock+s/2, s);
   blackRect_ = QRect(w-sblock-s/2, h-s, sblock+s/2, s);
   //
   int imgSize = s*4/5;
   resizeImages(imgSize);
   //
   repaint();
}

void CapturedPiecesView::countFromPosition(const ChessPosition &position, int sign)
{
   ChessCoord coord;
   //
   for(coord.row=1;coord.row<=position.maxRow();++coord.row)
   {
      for(coord.col=1;coord.col<=position.maxCol();++coord.col)
      {
         ChessPiece piece = position.cell(coord);
         if(piece.type()>=ptQueen)
         {
            if(piece.color()==pcWhite)
            {
               whiteCaptPieces_[piece.type()-ptQueen] += sign;
            }
            else
            {
               blackCaptPieces_[piece.type()-ptQueen] += sign;
            }
         }
      }
   }
}

void CapturedPiecesView::setNumberFont(const QFont& font)
{
   numberFont_ = font;
   repaint();
}
