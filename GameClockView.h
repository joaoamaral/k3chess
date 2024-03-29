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
   void setLeftPlayerName(const QString& name);
   void setRightPlayerName(const QString& name);
   void setLeftClock(QTime time);
   void setRightClock(QTime time);
   void setActiveSide(ClockActiveSide value);
   void setClockFont(const QFont& font);

signals:
   void click();

protected:
   virtual void resizeEvent(QResizeEvent*);
   virtual void paintEvent(QPaintEvent*);
   virtual void mousePressEvent(QMouseEvent*);
private:
   void draw(QPainter& painter, const QRect& clipRect);
   QString getClockText(const QTime& time) const;
   void updateLayout();
private:
   QString leftPlayerName_;
   QString rightPlayerName_;
   QString leftClockText_;
   QString rightClockText_;
   QRect leftPlayerNameRect_;
   QRect rightPlayerNameRect_;
   QRect leftClockRect_;
   QRect rightClockRect_;
   ClockActiveSide activeSide_;
   QFont clockFont_;
};

#endif
