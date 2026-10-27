#include <algorithm>

#include <QTimer>
#include <QWidget>


#include "spinner.hxx"

namespace easyqt {
	Spinner::Spinner(QWidget *parent): QWidget(parent) {
		setMinimumSize(20, 20);
		connect(&_timer, &QTimer::timeout,
			this, [this]() {
			_angle = (_angle + 5) % 360;
			update();
		});

		_timer.setInterval(8); // Approximately 120 FPS
	}

	void Spinner::start() {
		if (!_timer.isActive()) {
			_timer.start();
		}
	}

	void Spinner::stop() {
		_timer.stop();
		update();
	}

	void Spinner::paintEvent(QPaintEvent* event) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		if (!_timer.isActive()) {
			return;
		}

		// Use the largest square that fits inside the widget.
		const qreal side = std::min(width(), height());
		const qreal lineWidth = 3.0;

		const QRectF circle(
			(width() - side) / 2.0 + lineWidth,
			(height() - side) / 2.0 + lineWidth,
			side - 2.0 * lineWidth,
			side - 2.0 * lineWidth
		);

		QPen pen(_spinnerColor, lineWidth, Qt::SolidLine, Qt::RoundCap);
		painter.setPen(pen);
		painter.setBrush(Qt::NoBrush);

		painter.drawArc(circle, _angle * 16, 270 * 16);
	}
}
