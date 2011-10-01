#ifndef __KeyPreviewImpl_h
#define __KeyPreviewImpl_h

#include <QObject>

class KeyPreviewImpl;

class KeyPreviewEventFilter : public QObject
{
public:
   KeyPreviewEventFilter(KeyPreviewImpl *previewer);
   virtual bool eventFilter(QObject* obj, QEvent* event);
private:
   KeyPreviewImpl *previewer_;
};

class KeyPreviewImpl
{
   friend class KeyPreviewEventFilter;

public:
   KeyPreviewImpl();

protected:
   void enableKeyPreview();
   void disableKeyPreview();

   virtual bool keyPreview(int key,
                           Qt::KeyboardModifiers modifiers,
                           bool pressed) = 0;

private:
   bool enabled_;
   KeyPreviewEventFilter filter_;
};

#endif
