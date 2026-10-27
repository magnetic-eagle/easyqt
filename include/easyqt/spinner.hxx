#pragma once

#include <QColor>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QRectF>
#include <QSize>
#include <QTimer>
#include <QWidget>

namespace easyqt {
	class Spinner: public QWidget {
		Q_OBJECT

		public:
			Spinner(QWidget *parent = nullptr);

			void start();
			void stop();

			bool isSpinning() const {
				return _timer.isActive();
			}

			QSize sizeHint() const override {
				return QSize(24, 24);
			}
				
		protected:
			virtual void paintEvent(QPaintEvent *) override;

		private:
			QTimer _timer;
			int _angle = 0;
			QColor _spinnerColor = palette().color(QPalette::Highlight);
	};
}
