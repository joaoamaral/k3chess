#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include "CommandPanel.h"
#include "Settings.h"
#include <assert.h>

namespace
{

const int cDefaultTopMargin = 6;
const int cHorizontalLabelPadding = 14;
const int cVerticalLabelPadding = 6;
const int cUnderlineHeight = 4; // must be within cVerticalLabelPadding
const QColor cDefaultBackgroundColor(213, 196, 170);

QColor blendColor(QColor c, QColor d)
{
   return QColor((c.red()+d.red())/2,
                 (c.green()+d.green())/2,
                 (c.blue()+d.blue())/2);
}

}

CommandPanel::CommandPanel(QWidget *parent) :
   QWidget(parent),
   backgroundBrush_(cDefaultBackgroundColor),
   textColor_(Qt::black),
   disabledTextColor_(blendColor(cDefaultBackgroundColor, Qt::black)),
   topMargin_(cDefaultTopMargin),
   highlightedIndex_(-1),
   pressedIndex_(-1),
   useHotkeys_(false),
   showHighlight_(false),
   staticMode_(false)
{
   setMinimumSize(256, 12);
   setMaximumSize(2048, 512);
   //
   rects_.reserve(16);
   selectable_.reserve(16);
   enabled_.reserve(16);
}

void CommandPanel::setLabels()
{
   rects_.clear();
   selectable_.clear();
   enabled_.clear();
   //
   staticMode_ = true;
   //
   std::list<CommandOption>::const_iterator it = options_.items().begin(), itEnd = options_.items().end();
   for(;it!=itEnd;++it)
   {
      selectable_.push_back(it->enabled && it->id>0);
      enabled_.push_back(it->enabled);
      if(it->id>0)
         staticMode_ = false;
   }
   //
   updateLabelPositions();
   //
   repaint(rect());
}

void CommandPanel::setCommandOptions(const CommandOptions& options)
{
   options_ = options;
   //
   pressedIndex_ = -1;
   highlightedIndex_ = -1;
   //
   setLabels();
}

void CommandPanel::setTopMargin(int value)
{
   if(value<0 || value>100) return;
   if(value==topMargin_) return;
   //
   topMargin_ = value;
   updateLabelPositions();
}

void CommandPanel::keyPressEvent(QKeyEvent *event)
{
   event->accept(); // consume event if it arrived via default event handing mechanism
   processKey(Qt::Key(event->key()));
}

bool CommandPanel::processKey(Qt::Key key)
{
   switch(key)
   {
      case Qt::Key_Left:
         moveToPrevItem();
         break;
      case Qt::Key_Right:
         moveToNextItem();
         break;
      case Qt::Key_Select:
         selectOption(options_.idFromIndex(highlightedIndex_));
         break;
      default:
         if(useHotkeys_)
         {
            const CommandOption *option = options_.findItemByKey(key);
            if(option && option->isSelectable())
            {
               selectOption(option->id);
               break;
            }
         }
         return false;
   }
   //
   return true;
}

void CommandPanel::mousePressEvent(QMouseEvent *event)
{
   for(unsigned i=0; i<rects_.size(); ++i)
   {
      if(rects_[i].contains(event->pos()))
      {
         if(selectable_[i])
         {
            highlightedIndex_ = i;
            pressedIndex_ = i;
            repaint(rects_[i]);
            return;
         }
      }
   }
   //
   emit idleClick();
}

void CommandPanel::mouseReleaseEvent(QMouseEvent*)
{
   if(pressedIndex_<0) return;
   //
   QRect repaintRect = rects_[pressedIndex_];
   int id = options_.idFromIndex(pressedIndex_);
   if(id>0)
   {
      selectOption(id);
   }
   pressedIndex_ = -1;
   repaint(repaintRect);
}

void CommandPanel::resizeEvent(QResizeEvent*)
{
   updateLabelPositions();
}

void CommandPanel::updateLabelPositions()
{
   QRect lastRect;
   QPoint nextTopLeft(0, topMargin_);
   //
   rects_.clear();
   unsigned lineStartIdx = 0;
   //
   std::list<CommandOption>::const_iterator it = options_.items().begin(), itEnd = options_.items().end();
   for(;it!=itEnd;++it)
   {
      QRect r(0, 0, fontMetrics().width(it->text()), fontMetrics().height());
      r.setBottom(r.bottom()+cUnderlineHeight);
      //
      rects_.push_back(r);
      //
      QRect& rect = rects_.back();
      //
      if(nextTopLeft.x()+rect.width()>width())
      {
         if(nextTopLeft.x()==0)
         {
            // the label is too wide, place it anyway and move to the next line
            rect.moveTopLeft(nextTopLeft);
            lastRect = rect;
            nextTopLeft.setX(0);
            nextTopLeft.setY(lastRect.bottom()+cVerticalLabelPadding);
            lineStartIdx = rects_.size();
            continue;
         }
         else
         {
            // center align current line
            centerAlignRects(rects_, lineStartIdx, rects_.size()-1);
            //
            lineStartIdx = rects_.size();
            //
            nextTopLeft.setX(0);
            nextTopLeft.setY(lastRect.bottom()+cVerticalLabelPadding);
         }
      }
      rect.moveTopLeft(nextTopLeft);
      nextTopLeft.setX(rect.right()+cHorizontalLabelPadding);
      nextTopLeft.setY(rect.top());
      lastRect = rect;
   }
   //
   if(lineStartIdx < rects_.size())
   {
      centerAlignRects(rects_, lineStartIdx, rects_.size()-1);
   }
}

void CommandPanel::centerAlignRects(std::vector<QRect>& rects, int first, int last)
{
   int x1 = rects[first].left();
   int x2 = rects[last].right();
   //
   int w = width();
   int w0 = x2-x1+1;
   //
   int dx = ((w-w0)/2)-x1;
   //
   for(int i=first; i<=last; ++i)
   {
      rects[i].moveLeft(rects[i].left()+dx);
   }
}

void CommandPanel::selectOption(int id)
{
   if(id==0) return;
   //
   highlightedIndex_ = -1;
   pressedIndex_ = -1;
   hasCursor_ = false;
   //
   options_.clear();
   rects_.clear();
   selectable_.clear();
   repaint(rect());
   //
   emit optionSelected(id);
}

void CommandPanel::paintEvent(QPaintEvent *event)
{
   QPixmap pix(size());
   QPainter buffer(&pix);
   //
   drawPanel(buffer, event->rect());
   //
   QPainter painter(this);
   painter.drawPixmap(QPoint(0, 0), pix);
}

void CommandPanel::drawPanel(QPainter& painter, const QRect& clipRect)
{
   painter.setRenderHint(QPainter::TextAntialiasing);
   //
   painter.setPen(Qt::NoPen);
   painter.setBrush(backgroundBrush_);
   painter.drawRect(clipRect);
   //
   painter.setFont(font());
   painter.setPen(textColor_);
   painter.setBackgroundMode(Qt::OpaqueMode);
   painter.setBackground(backgroundBrush_);
   //
   std::list<CommandOption>::const_iterator it = options_.items().begin(), itEnd = options_.items().end();
   for(int i=0;it!=itEnd;++it, ++i)
   {
      assert(i<(int)rects_.size());
      //
      QRect rect(rects_[i]);
      QRect textRect(rect);
      textRect.setBottom(textRect.bottom()-cUnderlineHeight);
      //
      if(!rect.intersects(clipRect)) continue;
      //
      if(i==pressedIndex_)
      {
         painter.setPen(backgroundBrush_.color());
         painter.setBackground(textColor_);
         painter.drawText(textRect, Qt::AlignHCenter, it->text());
      }
      else if(enabled_[i])
      {
         painter.setPen(textColor_);
         //
         QRect underlineRect(rect.left(), rect.bottom()-cUnderlineHeight,
                             rect.width()-1, cUnderlineHeight);
         if(i==highlightedIndex_ && showHighlight_)
         {
            painter.setBrush(textColor_);
            painter.drawRect(underlineRect);
         }
         //
         painter.drawText(textRect, Qt::AlignCenter, it->text());
      }
      else
      {
         painter.setPen(disabledTextColor_);
         painter.drawText(textRect, Qt::AlignCenter, it->text());
      }
   }
}

bool CommandPanel::enter()
{
   if(highlightedIndex_>=0)
   {
      showHighlight_ = true; // resume selection
   }
   else
   {
      showHighlight_ = true;
      highlightedIndex_ = rects_.size()-1;
      moveToNextItem();
      //
      if(highlightedIndex_==-1) return false;
   }
   repaint(rects_[highlightedIndex_]);
   //
   return true;
}

void CommandPanel::suspendSelection()
{
   if(highlightedIndex_>=0)
   {
      showHighlight_ = false;
      repaint(rects_[highlightedIndex_]);
   }
}

void CommandPanel::moveToNextItem()
{
   QRect repaintRect;
   //
   int origIdx = highlightedIndex_;
   //
   if(highlightedIndex_<0)
   {
      highlightedIndex_ = rects_.size()-1;
   }
   else
   {
      if(selectable_[highlightedIndex_])
         repaintRect = rects_[highlightedIndex_];
   }
   //
   int nitems = (int)rects_.size();
   //
   assert(highlightedIndex_<nitems);
   //
   if(++highlightedIndex_==nitems)
   {
      highlightedIndex_ = 0;
   }
   //
   while(highlightedIndex_<nitems)
   {
      if(selectable_[highlightedIndex_]) break;
      if(++highlightedIndex_==nitems)
         highlightedIndex_=0;
   }
   //
   if(highlightedIndex_!=origIdx)
   {
      repaint(repaintRect.united(rects_[highlightedIndex_]));
   }
}

void CommandPanel::moveToPrevItem()
{
   QRect repaintRect;
   //
   int origIdx = highlightedIndex_;
   //
   if(highlightedIndex_<0)
   {
      highlightedIndex_ = 0;
   }
   else
   {
      if(selectable_[highlightedIndex_])
         repaintRect = rects_[highlightedIndex_];
   }
   //
   int nitems = (int)rects_.size();
   //
   assert(highlightedIndex_<nitems);
   //
   if(--highlightedIndex_<0)
   {
      highlightedIndex_ = nitems-1;
   }
   //
   while(highlightedIndex_>=0)
   {
      if(selectable_[highlightedIndex_]) break;
      if(--highlightedIndex_<0)
         highlightedIndex_=nitems-1;
   }
   //
   if(highlightedIndex_<0)
   {
      assert(highlightedIndex_==-1);
   }
   else
   {
      repaintRect = repaintRect.united(rects_[highlightedIndex_]);
   }
   //
   if(highlightedIndex_!=origIdx)
   {
      repaint(repaintRect.united(rects_[highlightedIndex_]));
   }
}

bool CommandPanel::hasCursor() const
{
   return highlightedIndex_>=0 && showHighlight_;
}

void CommandPanel::setBackgroundColor(QColor color)
{
   if(color==backgroundBrush_.color()) return;
   backgroundBrush_.setColor(color);
   disabledTextColor_ = blendColor(color, textColor_);
   repaint(rect());
}

void CommandPanel::setUseHotKeys(bool value)
{
   useHotkeys_ = value;
}
