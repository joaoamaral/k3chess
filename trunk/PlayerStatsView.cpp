#include "PlayerStatsView.h"

#include <QFontMetrics>
#include <QPaintEvent>

namespace
{

const int cDefaultPadding = 8;
const int cDefaultClockPadding = 16;

}

PlayerStatsView::PlayerStatsView(QObject *parent) :
   QFrame(parent), playerNameFont_("Sans", 18),
   activePlayerNameFont_("Sans", 18, 600),
   clockFont_("Sans", 24), padding_(cDefaultPadding),
   clockPadding_(cDefaultClockPadding), active_(false)
{
}

void PlayerStatsView::setPlayerName(const QString &value)
{
   if(playerName_==value) return;
   playerName_ = value;
   updateLayout();
}

void PlayerStatsView::setTime(const QTime &value)
{
   QString newClockText = value.hour()>0 ?
            value.toString("HH:mm:ss") :
            value.toString("mm:ss");
   if(clockText_==newClockText) return;
   clockText_ = newClockText;
   updateLayout();
}

void PlayerStatsView::paintEvent(QPaintEvent *event)
{
   QPixmap pix(size());
   QPainter buffer(&pix);
   //
   if(event->rect().intersects(playerNameRect_))
      drawPlayerName(buffer);
   if(event->rect().intersects(clockRect_))
      drawClock(buffer);
   //
   QPainter painter(this);
   painter.drawPixmap(QPoint(0, 0), pix);
}

void PlayerStatsView::resizeEvent(QResizeEvent *)
{
   updateLayout();
}

void PlayerStatsView::setActive(bool value)
{
   if(active_==value) return;
   active_ = value;
   updateLayout();
}

void PlayerStatsView::updateLayout()
{
   QFontMetrics fmName(active_ ? activePlayerNameFont_ : playerNameFont_);
   QFontMetrics fmClock(clockFont_);
   playerNameRect_.setWidth(fmName.width(playerName_));
   playerNameRect_.setHeight(fmName.height());
   playerNameRect_.setX((rect().width()-playerNameRect_.width())/2);
   playerNameRect_.setY(padding_);
   clockRect_.setWidth(fmClock.width(clockText_));
   clockRect_.setHeight(fmClock.height());
   clockRect_.setX((rect().width()-clockRect_.width())/2);
   clockRect_.setY(playerNameRect_.bottom()+clockPadding_);
   repaint();
}

