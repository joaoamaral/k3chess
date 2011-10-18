#ifndef __CapturedPiecesView_h
#define __CapturedPiecesView_h

#include "ChessPosition.h"
#include <QWidget>
#include <QImage>
#include <QPen>
#include <QBrush>
#include <vector>

class CapturedPiecesView : public QWidget
{
   Q_OBJECT

public:
   explicit CapturedPiecesView(QWidget *parent=0);

   void setPiecesImage(const QImage& image);
   void updateContents(const ChessPosition& initialPosition,
                       const ChessPosition& currentPosition);

protected:
   virtual void paintEvent(QPaintEvent*);
   virtual void resizeEvent(QResizeEvent*);

private:
   void draw(QPainter& painter, const QRect& clipRect);
   void countFromPosition(const ChessPosition& position,
                          int sign);
   void resizeImages(int size);
   void drawPieces(QPainter& painter, const QRect& rect,
                   const std::vector<int>& captured,
                   PieceColor color);
   void updateLayout();

private:
   std::vector<int> whiteCaptPieces_;
   std::vector<int> blackCaptPieces_;
   QRect whiteRect_;
   QRect blackRect_;
   QBrush backgroundBrush_;
   QPen linePen_;
   QColor numberColor_;
   QFont numberFont_;
   QImage originalPieces_;
   std::vector<QImage> whitePieces_;
   std::vector<QImage> blackPieces_;
};

#endif
