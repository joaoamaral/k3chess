#include "GameClockView.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QStyle>
#include <QPalette>

namespace
{

const int cHorizMargin = 12;

}

GameClockView::GameClockView(QWidget *parent) :
   QWidget(parent),
   activeSide_(casNone),
   clockFont_("Sans", 8)
{

}

void GameClockView::resizeEvent(QResizeEvent*)
{
   updateLayout();
   repaint();
}

void GameClockView::paintEvent(QPaintEvent *event)
{
   QPixmap pix(size());
   QPainter buffer(&pix);
   //
   QPainter painter(this);
   //
   buffer.setClipRect(event->rect());
   draw(buffer, event->rect());
   //
   painter.drawPixmap(QPoint(0, 0), pix);
}

QString GameClockView::getClockText(const QTime &time) const
{
   if(time.isNull()) return "--:--";
   return time.hour()<1 ? time.toString("mm:ss") : time.toString("HH:mm:ss");
}

void GameClockView::updateLayout()
{
   QFontMetrics fm(clockFont_);
   int clockWidth = fm.width("_0:00:00");
   int h1 = fm.height();
   int y1 = (rect().height()-h1)/2;
   leftClockRect_ = QRect(cHorizMargin, y1, clockWidth, h1);
   leftPlayerNameRect_ = QRect(cHorizMargin+clockWidth, y1, fm.width(leftPlayerName_), h1);
   int w2 = fm.width(rightPlayerName_);
   rightPlayerNameRect_ = QRect(rect().width()-cHorizMargin-clockWidth-w2, y1, w2, h1);
   rightClockRect_ = QRect(rect().width()-cHorizMargin-clockWidth, y1, clockWidth, h1);
}

void GameClockView::setLeftPlayerName(const QString &name)
{
   if(leftPlayerName_==name) return;
   leftPlayerName_ = name;
   updateLayout();
   repaint(QRect(leftPlayerNameRect_.left(), 0, rect().width()/2-leftPlayerNameRect_.left(), rect().height()));
}

void GameClockView::setRightPlayerName(const QString &name)
{
   if(rightPlayerName_==name) return;
   rightPlayerName_ = name;
   updateLayout();
   repaint(QRect(rect().width()/2, 0, rightPlayerNameRect_.right()-rect().width()/2, rect().height()));
}

void GameClockView::setLeftClock(QTime time)
{
   QString clockText = getClockText(time);
   if(leftClockText_==clockText) return;
   leftClockText_ = clockText;
   repaint(leftClockRect_);
}

void GameClockView::setRightClock(QTime time)
{
   QString clockText = getClockText(time);
   if(rightClockText_==clockText) return;
   rightClockText_ = clockText;
   repaint(rightClockRect_);
}

void GameClockView::setActiveSide(ClockActiveSide value)
{
   if(activeSide_==value) return;
   activeSide_ = value;
   repaint();
}

void GameClockView::draw(QPainter &painter, const QRect &clipRect)
{
   QColor textColor = palette().color(QPalette::Text);
   QColor backgroundColor = palette().color(QPalette::Background);
   //
   painter.setPen(Qt::NoPen);
   painter.setBrush(backgroundColor);
   painter.drawRect(clipRect);
   //
   painter.setPen(textColor);
   painter.setFont(clockFont_);
   //
   painter.drawText(leftClockRect_, Qt::AlignCenter, leftClockText_);
   painter.drawText(leftPlayerNameRect_, Qt::AlignCenter, leftPlayerName_);
   painter.drawText(rightClockRect_, Qt::AlignCenter, rightClockText_);
   painter.drawText(rightPlayerNameRect_, Qt::AlignCenter, rightPlayerName_);
}

void GameClockView::mousePressEvent(QMouseEvent *)
{
   emit click();
}

void GameClockView::setClockFont(const QFont &font)
{
   clockFont_ = font;
   repaint();
}
