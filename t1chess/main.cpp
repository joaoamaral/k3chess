#include "MainWindow.h"
#include "GlobalUISession.h"

#include <QApplication>
#include <QCoreApplication>
#include <QWSEvent>

bool customEventFilter(void *message, long *)
{
	QWSEvent * pev = (QWSEvent *) message;

	if(pev->type == QWSEvent::Key)
	{
		QWSKeyEvent *pke = (QWSKeyEvent *) message;
		if(pke->simpleData.keycode == Qt::Key_Sleep) {
			return true;
		}
		if(pke->simpleData.keycode == Qt::Key_WakeUp) {
			QSize size = qApp->activeWindow()->size();
			// force repaint
			qApp->activeWindow()->resize(size.width(), size.height()+1);
			qApp->activeWindow()->resize(size.width(), size.height());
			return true;
		}
		qDebug("QWS key: key=0x%x, press=%d, uni=%d", pke->simpleData.keycode,
			pke->simpleData.is_press, pke->simpleData.unicode);
		return false;
	}
	return false;
}

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   //
   app.setApplicationName("T1Chess");
   //
#ifdef QT_KEYPAD_NAVIGATION
   app.setNavigationMode(Qt::NavigationModeKeypadTabOrder);
#endif
   app.setEventFilter(customEventFilter);
   //
   Singletons::initialize();
   //
   g_globalUISession.begin();
   //
   int result = app.exec();
   //
   Singletons::finalize();
   //
   return result;
}
