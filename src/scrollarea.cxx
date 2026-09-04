#include <QScrollBar>

#include "scrollarea.hxx"

namespace easyqt {
	QPoint ScrollArea::scrollPos() {
		return QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value());
	}
	void ScrollArea::ensureRectVisible(const QRect& rect, int xmargin, int ymargin) {
		if (rect.isNull()) {
			return;
		}

		const QRect viewportRect = viewport()->rect();

		int x = horizontalScrollBar()->value();
		int y = verticalScrollBar()->value();

		const int left = x + xmargin;
		const int right = x + viewportRect.width() - xmargin;
		const int top = y + ymargin;
		const int bottom = y + viewportRect.height() - ymargin;

		if (rect.left() < left) {
			x -= left - rect.left();
		} else if (rect.right() > right) {
			x += rect.right() - right;
		}

		if (rect.top() < top) {
			y -= top - rect.top();
		} else if (rect.bottom() > bottom) {
			y += rect.bottom() - bottom;
		}

		horizontalScrollBar()->setValue(x);
		verticalScrollBar()->setValue(y);
	}
}

