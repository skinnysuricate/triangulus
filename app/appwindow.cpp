#include "appwindow.h"

#include <memory>
#include <QtCore/QSet>
#include <QtGui/QPainter>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QVariantAnimation>

AppWindow::AppWindow(QWidget *parent)
	: QWidget(parent)
{
	animator_.reset(new QVariantAnimation);
	animator_->setLoopCount(-1);
	animator_->setStartValue(0.);
	animator_->setKeyValueAt(0.5, 1.);
	animator_->setEndValue(0.);
	animator_->setDuration(2000);
	connect(animator_.get(), QVariantAnimation::valueChanged, this, [this] (const QVariant &v) {
		progress_ = v.toReal();
		update();
	});
	connect(animator_.get(), QVariantAnimation::currentLoopChanged, this, AppWindow::recountDeltas);
	animator_->start();
}

AppWindow::~AppWindow()
{
	qDeleteAll(points_);
}

void AppWindow::reset()
{
//	QList<QPointF> points;

	qDeleteAll(points_);
	points_.clear();

	const QSize bound (width() * 1.2, height() * 1.2);
	const int x_offset = width() * 0.1;
	const int y_offset = height() * 0.1;

	qsrand(uint(&bound) + uint(QDateTime::currentMSecsSinceEpoch()));
	auto randomOffset = [] (int limit) {
		return qrand()%limit - limit;
	};

	for (int i = -x_offset; i < width() + x_offset; i+=40+randomOffset(20)) {
		points_ << new QPointF(qreal(i), 1.);
		points_ << new QPointF(qreal(i), height() -1.);
	}

	for (int i = -y_offset; i < height() + y_offset; i+=40+randomOffset(20)) {
		points_ << new QPointF(1., qreal(i));
		points_ << new QPointF(width() - 1., qreal(i));
	}

	auto isSuitable = [this] (const QPointF &p) {
		for (QPointF *existant: points_) {
			if (qAbs((p - *existant).manhattanLength()) < 20)
				return false;
		}
		return true;
	};

	for (int i = 0; i < 200; ++i) {
		QPointF new_p (bound.width() * (qrand() % 1001 / 1000.), bound.height() * (qrand() % 1001 / 1000.));
		if (!isSuitable(new_p)) {
			--i;
			continue;
		}

		points_ << new QPointF(new_p);
	}
		//	points_ << new QPointF(50., 50.) << new QPointF(300., 50.) << new QPointF(300., 400.) << new QPointF(50., 400.);

//	qDebug() << "Generated vertexes:" << points;

	polygons_ = std::move(Triangulator::triangulatePersistant(points_));
	generateColors();
	recountDeltas();

	update();
}

void AppWindow::paintEvent(QPaintEvent *e)
{
	QPainter p (this);
	p.setRenderHint(QPainter::Antialiasing);
//	p.translate(-width() * 0.1, -height() * 0.1);
	p.fillRect(rect(), QColor(40, 40, 40));

	int idx = 0;
	for (const LinkedTriangle &polygon: polygons_) {
		QPolygonF poly;
		poly << *polygon.v1 + deltas_.value(polygon.v1) * progress_
			 << *polygon.v2 + deltas_.value(polygon.v2) * progress_
			 << *polygon.v3 + deltas_.value(polygon.v3) * progress_;
		p.setBrush(polygon_colors_.at(idx));
		p.setPen(p.brush().color());
		p.drawPolygon(poly);
		++idx;
	}
}

void AppWindow::mousePressEvent(QMouseEvent *e)
{
	reset();
}

void AppWindow::generateColors()
{
	polygon_colors_.clear();

	const QColor base_clr ("#265080");
	auto generateColor = [&base_clr] () {
		const qreal base_v = base_clr.valueF();
		const bool prob_trigger = !bool((qrand()%10));
		const qreal s = prob_trigger * base_clr.saturationF();
		qreal v = base_v + (qrand()%200/200.-0.5) * 0.2;
		v = v - !prob_trigger * v * 0.5;
		return QColor::fromHsvF(base_clr.hueF(), s, v);
	};

	for (int i=0; i<polygons_.count(); ++i) {
		polygon_colors_ << generateColor();
	}
}

void AppWindow::recountDeltas()
{
	deltas_.clear();

	for (QPointF *p: points_) {
		deltas_[p] = QPointF(qrand()%11 - 5., qrand()%11 - 5.);
	}
}
