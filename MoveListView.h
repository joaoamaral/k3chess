#ifndef MOVELISTVIEW_H
#define MOVELISTVIEW_H

#include <QPlainTextEdit>
#include <QString>
#include <vector>

class MoveListView : public QPlainTextEdit
{
   Q_OBJECT
public:
   explicit MoveListView(QWidget *parent = 0);

   void addMove(const QString& move);
   void dropMoves(unsigned count);
   void clearMoves();

protected:
   virtual void resizeEvent(QResizeEvent *);

private:
   void regenerateLines();

private:
   QString lastLine_;
   std::vector<QString> moves_;
};

#endif // MOVELISTVIEW_H
