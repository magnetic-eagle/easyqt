#ifndef EASYQT_FLOWLAYOUT_H
#define EASYQT_FLOWLAYOUT_H

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
			void setGeometry(const QRect& rect) override;
			QLayoutItem* takeAt(int index) override;

			void setHorizontalSpacing(int spacing) { _hSpace = spacing; }
			void setVerticalSpacing(int spacing) { _vSpace = spacing; }
			void setSpacing(int spacing) override { _hSpace = _vSpace = spacing; }
			void setMargin(int margin) { setContentsMargins(margin, margin, margin, margin); }

			int horizontalSpacing() const;
			int verticalSpacing() const;
			Qt::Orientations expandingDirections() const override;
			bool hasHeightForWidth() const override;
			int heightForWidth(int) const override;
			int count() const override;
			QLayoutItem* itemAt(int index) const override;
			QSize minimumSize() const override;
			QSize sizeHint() const override;

			inline size_t row(size_t index) const {
				if (index < 0 || index >= _itemList.size()) {
					return -1;
				}
				size_t row = -1;
				for (size_t rowLength: _rowLengths) {
					index -= rowLength;
					row += 1;
					if (index < 0) {
						break;
					}
				}
				return row;
			}
			inline size_t row(QLayoutItem* item) const {
				const auto& it = std::find(_itemList.begin(), _itemList.end(), item);
				if (it != _itemList.end()) {
					return row(std::distance(_itemList.begin(), it));
				} else {
					return -1;
				}
			}
			inline size_t row(QWidget* widget) const {
				for (QLayoutItem* item: _itemList) {
					if (item->widget() == widget) {
						return row(item);
					}
				}
				return -1;
			}
			inline size_t rows() const { return _rows; };
			inline size_t columns(size_t row = 0) const { return _rowLengths[row]; };

		protected:
			virtual int doLayout(const QRect &rect, bool testOnly) const;
			int smartSpacing(QStyle::PixelMetric pm) const;

		private:
			std::vector<QLayoutItem*> _itemList;
			int _hSpace = 0;
			int _vSpace = 0;
			mutable unsigned int _rows = 0;
			mutable std::vector<unsigned int> _rowLengths;
	};
}

#endif

