#ifndef EASYQT_SCROLLAREA_HXX
#define EASYQT_SCROLLAREA_HXX

#include <QScrollArea>

namespace easyqt {
	class ScrollArea: public QScrollArea {
		Q_OBJECT
		
		public:
			QPoint scrollPos();
			void ensureRectVisible(const QRect& rect, int xmargin = 50, int ymargin = 50);
	};
}

#endif

