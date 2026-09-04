#pragma once

#include <QLayoutItem>
#include <QSize>
#include <QRect>
#include <QWidget>
#include <memory>

#include "flowlayout.hxx"

namespace easyqt {
	class ViewedFlowLayoutWidget: public QWidget {
		public:
			using QWidget::QWidget;

			unsigned int itemIndex() const { return _itemIndex; }
			void setItemIndex(unsigned int index, bool visible) {
				if (index != _itemIndex) {
					_itemIndex = index;
					itemIndexChanged();
				}
				if (visible != _itemVisible) {
					_itemVisible = visible;
					itemVisibilityChanged();
				}
			}

			virtual void itemIndexChanged() {};
			virtual void itemVisibilityChanged() {};
		
		protected:
			unsigned int _itemIndex = -1;
			bool _itemVisible = false;
	};

	typedef std::unique_ptr<ViewedFlowLayoutWidget> ViewedFlowLayoutWidgetPtr;
	typedef std::function<ViewedFlowLayoutWidgetPtr()> ViewedFlowLayoutWidgetFactory;

	class ViewedFlowLayout: public FlowLayout {
	public:
		using FlowLayout::FlowLayout;

		void setItemCount(unsigned int count) { _itemCount = count; }

		void setVisibleArea(QRect area) { _visibleArea = area; }
		void setWidgetFactory(ViewedFlowLayoutWidgetFactory func);

		unsigned int rows() const { return _rows; }
		unsigned int columns() const { return _columns; }
		QRect itemGeometry(unsigned int item) const;
		unsigned int firstVisibleItem() const { return _firstVisibleItem; }
		unsigned int lastVisibleItem() const { return _lastVisibleItem; }
		unsigned int numVisibleItems() const { return _numVisibleItems; }
		unsigned int firstBufferItem() const { return _firstBufferItem; }
		unsigned int lastBufferItem() const { return _lastBufferItem; }
		unsigned int numBufferItems() const { return _numBufferItems; }

		QSize sizeHint() const override;
		QSize minimumSize() const override;

		void updateWidgets();
	
	protected:
		virtual int doLayout(const QRect& rect, bool testOnly) const override;

	private:
		unsigned int _itemCount = 0;
		QRect _visibleArea;
		QSize _itemSize;
		mutable int _rows = 0;
		mutable int _columns = 0;
		mutable int _firstVisibleItem = -1;
		mutable int _lastVisibleItem = -1;
		mutable int _numVisibleItems = 0;
		mutable int _firstBufferItem = -1;
		mutable int _lastBufferItem = -1;
		mutable int _numBufferItems = 0;
		mutable std::vector<ViewedFlowLayoutWidgetPtr> _widgets;

		ViewedFlowLayoutWidgetFactory _widgetFactory = nullptr;
	};
}
