#ifndef __PlayerStatsView_h
#define __PlayerStatsView_h

#include <QTime>
#include <QFrame>

class PlayerStatsView : public QFrame
{
   Q_OBJECT
public:
   explicit PlayerStatsView(QObject *parent = 0);

   void setPlayerName(const QString& value);
   void setActive(bool value);
   void setTime(const QTime& value);

signals:

public slots:

protected:
   virtual void paintEvent(QPaintEvent*);
   virtual void resizeEvent(QResizeEvent*);

private:
   void updateLayout();

private:
   QString playerName_;
   QString clockText_;
   QRect playerNameRect_;
   QRect clockRect_;
   QFont playerNameFont_;
   QFont activePlayerNameFont_;
   QFont clockFont_;
   int padding_;
   int clockPadding_;
   bool active_;
};

#endif
