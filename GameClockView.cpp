#include "GameClockView.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

namespace
{

const int cHorizMargin = 12;
const QColor cDefaultBackgroundColor(213, 196, 170);

QColor blendColor(QColor c, QColor d)
{
   return QColor((c.red()+d.red())/2,
                 (c.green()+d.green())/2,
                 (c.blue()+d.blue())/2);
}

}

GameClockView::GameClockView(QWidget *parent) :
   QWidget(parent),
   backgroundBrush_(cDefaultBackgroundColor),
   textColor_(Qt::black),
   inactiveTextColor_(blendColor(cDefaultBackgroundColor, Qt::black)),
   activeSide_(casNone)
{

}

void GameClockView::setBackgroundColor(QColor color)
{
   if(backgroundBrush_.color()==color) return;
   backgroundBrush_.setColor(color);
   repaint();
}

void GameClockView::setTextColor(QColor color)
{
   if(textColor_==color) return;
   textColor_ = color;
   inactiveTextColor_ = blendColor(textColor_, backgroundBrush_.color());
   repaint();
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
   buffer.setClipRect(event->rect());
   draw(buffer, event->rect());
   //
   QPainter painter(this);
   painter.drawPixmap(QPoint(0, 0), pix);
}

QString GameClockView::getClockText(const QTime &time) const
{
   if(time.isNull()) return "--:--";
   return time.hour()<1 ? time.toString("mm:ss") : time.toString("HH:mm:ss");
}

void GameClockView::updateLayout()
{
   int clockWidth = fontMetrics().width("_0:00:00");
   int h1 = fontMetrics().height();
   int y1 = (rect().height()-h1*9/5)/2;
   leftClockRect_ = QRect(cHorizMargin, y1, clockWidth, h1);
   leftPlayerNameRect_ = QRect(cHorizMargin+clockWidth, y1, fontMetrics().width(leftPlayerName_), h1);
   int w2 = fontMetrics().width(rightPlayerName_);
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
   painter.setPen(Qt::NoPen);
   painter.setBrush(backgroundBrush_);
   painter.drawRect(clipRect);
   //
   painter.setFont(font());
   //
   if(activeSide_==casLeft)
      painter.setPen(textColor_);
   else
      painter.setPen(inactiveTextColor_);
   //
   painter.drawText(leftClockRect_, leftClockText_, Qt::AlignVCenter | Qt::AlignLeft);
   painter.drawText(leftPlayerNameRect_, leftPlayerName_);
   //
   if(activeSide_==casRight)
      painter.setPen(textColor_);
   else
      painter.setPen(inactiveTextColor_);
   //
   painter.drawText(rightClockRect_, rightClockText_, Qt::AlignVCenter | Qt::AlignRight);
   painter.drawText(rightPlayerNameRect_, rightPlayerName_);
}

void GameClockView::mousePressEvent(QMouseEvent *)
{
   emit click();
}
