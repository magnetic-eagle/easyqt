#include <QMargins>
#include <QRect>
#include <QSize>
#include <stdexcept>

#include "logging.hxx"
#include "viewedflowlayout.hxx"

namespace easyqt {
	int ViewedFlowLayout::doLayout(const QRect& rect, bool testOnly) const {
		if (_itemCount == 0 or !_itemSize.isValid()) {
			return 0;
		}

		const QMargins margins = contentsMargins();
		const QRect effectiveRect = rect.adjusted(+margins.left(), +margins.top(), -margins.right(), -margins.bottom());
		int columns = (effectiveRect.width() + horizontalSpacing()) / (_itemSize.width() + horizontalSpacing());
		if (columns <= 0) {
			columns = 1;
		}
		const int rows = ceil((double)_itemCount / columns);

		const int totalHeight = margins.top() + rows * (_itemSize.height() + verticalSpacing()) - verticalSpacing() + margins.bottom();

		if (testOnly or !_visibleArea.isValid()) {
			return totalHeight;
		}

		_columns = columns;
		_rows = rows;

		const int firstVisibleRow = std::max(
			(int)floor((double)_visibleArea.top() / (_itemSize.height() + verticalSpacing())),
			0
		);
		const int lastVisibleRow = std::min(
			(int)ceil((double)_visibleArea.bottom() / (_itemSize.height() + verticalSpacing())),
			_rows
		);
		const int numVisibleRows = (lastVisibleRow - firstVisibleRow);

		const int firstBufferRow = std::max(firstVisibleRow - 2, 0);
		const int lastBufferRow = std::min(lastVisibleRow + 2, _rows);
		const int numBufferRows = lastBufferRow - firstBufferRow;

		int firstVisibleItem = firstVisibleRow * _columns;
		int numVisibleItems = numVisibleRows * _columns;
		if (firstVisibleItem + numVisibleItems > _itemCount) {
			numVisibleItems -= (firstVisibleItem + numVisibleItems) - _itemCount;
		}
		int lastVisibleItem = firstVisibleItem + numVisibleItems;

		int firstBufferItem = firstBufferRow * _columns;
		int numBufferItems = numBufferRows * _columns;
		if (firstBufferItem + numBufferItems > _itemCount) {
			numBufferItems -= (firstBufferItem + numBufferItems) - _itemCount;
		}
		int lastBufferItem = firstBufferItem + numBufferItems;
		int firstBufferRowY = effectiveRect.y() + firstBufferRow * (_itemSize.height() + verticalSpacing());

		if (firstBufferItem == _firstBufferItem and lastBufferItem == _lastBufferItem) {
			// visible items stayed the same since last doLayout; no need to relayout
			return totalHeight;
		} else {
			// that the buffer moved completely outside of the visible area - relayout completely
			bool widgetCountChanged = _widgets.size() != numBufferItems;
			if (_widgets.size() > numBufferItems) {
				_widgets.resize(numBufferItems);
			}
			while (_widgets.size() < numBufferItems) {
				ViewedFlowLayoutWidgetPtr widget = _widgetFactory();
				widget->setParent(parentWidget());
				_widgets.push_back(std::move(widget));
			}

			int currentRow = firstBufferRow;
			int currentColumn = 0;
			int currentItem = firstBufferItem;
			for (const ViewedFlowLayoutWidgetPtr& widget: _widgets) {
				widget->setGeometry(
					effectiveRect.x() + (_itemSize.width() + horizontalSpacing()) * currentColumn,
					effectiveRect.y() + (_itemSize.height() + verticalSpacing()) * currentRow,
					_itemSize.width(),
					_itemSize.height()
				);
				if (currentItem >= _itemCount) {
					widget->setItemIndex(-1, false);
					if (widget->isVisible()) {
						widget->hide();
					}
				} else {
					widget->setItemIndex(currentItem, currentItem >= firstVisibleItem && currentItem < lastVisibleItem);
					if (!widget->isVisible()) {
						widget->show();
					}
				}
				currentItem++;
				currentColumn++;
				if (currentColumn >= _columns) {
					currentColumn = 0;
					currentRow++;
				}
			}

			_numVisibleItems = numVisibleItems;
			_firstVisibleItem = firstVisibleItem;
			_lastVisibleItem = lastVisibleItem;
			_numBufferItems = numBufferItems;
			_firstBufferItem = firstBufferItem;
			_lastBufferItem = lastBufferItem;

			return totalHeight;
		}
	}

	void ViewedFlowLayout::setWidgetFactory(ViewedFlowLayoutWidgetFactory func) {
		_widgetFactory = func;
		auto widget = _widgetFactory();
		_itemSize = widget->sizeHint();
	}

	QRect ViewedFlowLayout::itemGeometry(unsigned int item) const {
		if (_columns == 0) {
			return QRect();
		}
		if (item > _itemCount - 1) {
			throw std::out_of_range(std::format("Item number {} is larger than item count {}", item, _itemCount));
		}
		int row = item / _columns;
		int column = item - (row * _columns);
		int x = contentsMargins().left() + (_itemSize.width() + horizontalSpacing()) * column;
		int y = contentsMargins().top() + (_itemSize.height() + verticalSpacing()) * row;

		return {x, y, _itemSize.width(), _itemSize.height()};
	}

	QSize ViewedFlowLayout::sizeHint() const {
		return minimumSize();
	}

	QSize ViewedFlowLayout::minimumSize() const {
		if (_itemCount == 0) {
			return QSize(0, 0);
		}
		const QMargins margins = contentsMargins();
		int w = _itemSize.width() + margins.left() + margins.right();
		int h = _itemSize.height() + margins.top() + margins.bottom();
		return QSize(w, h);
	}

	void ViewedFlowLayout::updateWidgets() {
		for (ViewedFlowLayoutWidgetPtr& w: _widgets) {
			w->update();
		}
	}
}