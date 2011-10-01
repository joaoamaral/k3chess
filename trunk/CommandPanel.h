#ifndef __CommandPanel_h
#define __CommandPanel_h

#include <QWidget>
#include "CommandOptions.h"
#include <vector>

/* CommandPanel is a panel with clickable labels with command options
   passed in setCommandOptions method. Hot keys are also supported
   for command options. This widget can be used on mouse/keyboard/stylus devices
   for fast and easy command/option selection. It is a simple replacement
   of toolbar or menu with items arranged in horizontal lines. */

class CommandPanel : public QWidget
{
    Q_OBJECT
public:
   explicit CommandPanel(QWidget *parent = 0);

   void setCommandOptions(const CommandOptions& options);
   void setUseHotKeys(bool value); // allow or disallow the use of hotkeys included in command options (default is false)
   void setBackgroundColor(QColor color);
   void setTopMargin(int value);

   bool hasCursor() const;
   bool enter();  // enter the control to select items, returns false if enter fails because there are no selectable items
   void suspendSelection();

   bool processKey(Qt::Key key); // can be used for external key input (avoiding problems with event routing)

protected:
   virtual void keyPressEvent(QKeyEvent*);
   virtual void resizeEvent(QResizeEvent*);
   virtual void paintEvent(QPaintEvent*);
   virtual void mousePressEvent(QMouseEvent*);
   virtual void mouseReleaseEvent(QMouseEvent*);

signals:
   void optionSelected(int id);
   void idleClick(); // sent when user clicks mouse
                     // outside selectable items or
                     // on a static item
private:
   void setLabels();
   void updateLabelPositions();
   void centerAlignRects(std::vector<QRect>& rects, int first, int last);

   void drawPanel(QPainter& painter, const QRect& clipRect);

   void selectOption(int id);
   void moveToPrevItem();
   void moveToNextItem();

private:
   std::vector<QRect> rects_;
   std::vector<bool> selectable_;
   std::vector<bool> enabled_;
   CommandOptions options_;
   QBrush backgroundBrush_;
   QColor textColor_;
   QColor disabledTextColor_;
   int topMargin_;
   int highlightedIndex_;
   int pressedIndex_;
   bool useHotkeys_;
   bool hasCursor_;
   bool showHighlight_;
   bool staticMode_;
};

#endif
