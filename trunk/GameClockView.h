#ifndef __GameClockView_h
#define __GameClockView_h

#include <QWidget>
#include <QTime>

enum ClockActiveSide { casNone, casLeft, casRight };

class GameClockView : public QWidget
{
   Q_OBJECT
public:
   GameClockView(QWidget *parent);
   void setBackgroundColor(QColor color);
   void setTextColor(QColor color);
   void setLeftPlayerName(const QString& name);
   void setRightPlayerName(const QString& name);
   void setLeftClock(QTime time);
   void setRightClock(QTime time);
   void setActiveSide(ClockActiveSide value);
protected:
   virtual void resizeEvent(QResizeEvent*);
   virtual void paintEvent(QPaintEvent*);
private:
   void draw(QPainter& painter, const QRect& clipRect);
   QString getClockText(const QTime& time) const;
   void updateLayout();
private:
   QString leftPlayerName_;
   QString rightPlayerName_;
   QString leftClockText_;
   QString rightClockText_;
   QBrush backgroundBrush_;
   QColor textColor_;
   QColor inactiveTextColor_;
   QRect leftPlayerNameRect_;
   QRect rightPlayerNameRect_;
   QRect leftClockRect_;
   QRect rightClockRect_;
   ClockActiveSide activeSide_;
};

#endif
