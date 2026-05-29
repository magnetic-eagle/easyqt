#ifndef EASYQT_FLOWLAYOUT_H
#define EASYQT_FLOWLAYOUT_H

#include <qobjectdefs.h>
#include <vector>

#include <QLayout>
#include <QRect>
#include <QStyle>

namespace easyqt {
	class FlowLayout: public QLayout {
		Q_OBJECT
		public:
			explicit FlowLayout(QWidget* parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
			explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
			~FlowLayout();

			void addItem(QLayoutItem* item) override;
			int horizontalSpacing() const;
			int verticalSpacing() const;
			Qt::Orientations expandingDirections() const override;
			bool hasHeightForWidth() const override;
			int heightForWidth(int) const override;
			int count() const override;
			QLayoutItem* itemAt(int index) const override;
			QSize minimumSize() const override;
			void setGeometry(const QRect& rect) override;
			QSize sizeHint() const override;
			QLayoutItem* takeAt(int index) override;
			inline size_t row(size_t index) {
				if (index < 0 || index >= _itemList.size()) {
					return -1;
				}
				size_t row = -1;
				for (size_t rowLength: _columns) {
					index -= rowLength;
					row += 1;
					if (index < 0) {
						break;
					}
				}
				return row;
			}
			inline size_t row(QLayoutItem* item) {
				return row(_itemList.indexOf(item));
			}
			inline size_t row(QWidget* widget) {
				for (QLayoutItem* item: _itemList) {
					if (item->widget() == widget) {
						return row(item);
					}
				}
				return -1;
			}
			inline size_t rows() { return _rows; };
			inline size_t columns(size_t row = 0) { return _columns[row]; };

		private:
			int doLayout(const QRect &rect, bool testOnly) const;
			int smartSpacing(QStyle::PixelMetric pm) const;

			QList<QLayoutItem*> _itemList;
			int _hSpace;
			int _vSpace;
			mutable size_t _rows;
			mutable std::vector<size_t> _columns;
	};
}

#endif

