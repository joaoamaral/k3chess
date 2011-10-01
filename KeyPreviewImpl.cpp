#include "KeyPreviewImpl.h"
#include <QKeyEvent>
#include <assert.h>

KeyPreviewEventFilter::KeyPreviewEventFilter(KeyPreviewImpl *previewer) :
   previewer_(previewer)
{
   assert(previewer_);
}

bool KeyPreviewEventFilter::eventFilter(QObject *obj, QEvent *event)
{
   assert(obj->isWidgetType());
   //
   switch(event->type())
   {
   case QEvent::KeyPress:
      {
         QKeyEvent *ke = static_cast<QKeyEvent*>(event);
         if(previewer_->keyPreview(ke->key(), ke->modifiers(), true))
         {
            event->ignore();
            return true;
         }
      }
      break;
   case QEvent::KeyRelease:
      {
         QKeyEvent *ke = static_cast<QKeyEvent*>(event);
         if(previewer_->keyPreview(ke->key(), ke->modifiers(), false))
         {
            event->ignore();
            return true;
         }
      }
      break;
   default:
      break;
   }
   //
   return false;
}

KeyPreviewImpl::KeyPreviewImpl() :
   enabled_(false), filter_(this)
{
}

void KeyPreviewImpl::enableKeyPreview()
{
   if(enabled_) return;
   //
   QObject *obj = dynamic_cast<QObject*>(this);
   //
   QObjectList chlist = obj->children();
   foreach(QObject *child, chlist)
   {
      if(child->isWidgetType())
      {
         child->installEventFilter(&filter_);
      }
   }
   //
   enabled_ = true;
}

void KeyPreviewImpl::disableKeyPreview()
{
   if(!enabled_) return;
   //
   QObject *obj = dynamic_cast<QObject*>(this);
   //
   QObjectList chlist = obj->children();
   foreach(QObject *child, chlist)
   {
      if(child->isWidgetType())
      {
         child->removeEventFilter(&filter_);
      }
   }
   //
   enabled_ = false;
}
