#include "appwindow.h"

#include <memory>
#include <QtCore/QSet>
#include <QtGui/QPainter>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QVariantAnimation>
#include <QtCore/QtMath>
#include <QtGui/QMouseEvent>

AppWindow::AppWindow(QWidget *parent)
	: QWidget(parent)
{
	setMouseTracking(true);
#ifdef SOME_FUTURE
	animator_.reset(new QVariantAnimation);
	animator_->setStartValue(0.);
	animator_->setEndValue(1.);
	animator_->setDuration(100);
	connect(animator_.get(), QVariantAnimation::valueChanged, this, [this] (const QVariant &v) {
		progress_ = v.toReal();
		update();
	});
	animator_->start();
#endif
}

AppWindow::~AppWindow()
{
	qDeleteAll(points_);
}

void AppWindow::reset()
{
	qDeleteAll(points_);
	points_.clear();
	deltas_.clear();

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

	for (int i = 0; i < 100; ++i) {
		QPointF new_p (bound.width() * (qrand() % 1001 / 1000.), bound.height() * (qrand() % 1001 / 1000.));
		if (!isSuitable(new_p)) {
			--i;
			continue;
		}
		points_ << new QPointF(new_p);
	}

	polygons_ = std::move(Triangulator::triangulatePersistant(points_));
	generateColors();
	recountAntigravityForces(mapFromGlobal(QCursor::pos()));
	update();
}

void AppWindow::paintEvent(QPaintEvent *e)
{
	QPainter p (this);
	p.setRenderHint(QPainter::Antialiasing);
	p.fillRect(rect(), QColor(40, 40, 40));

	auto drawPolygon = [&] (int idx) {
		const LinkedTriangle &polygon = polygons_.at(idx);
		QPolygonF poly;
		poly << *polygon.v1 + deltas_.value(polygon.v1) * progress_
			 << *polygon.v2 + deltas_.value(polygon.v2) * progress_
			 << *polygon.v3 + deltas_.value(polygon.v3) * progress_;
		p.setBrush(polygon_colors_.at(idx));
		p.setPen(p.brush().color());
		if (idx == hovered_idx_) {
			p.setBrush(polygon_colors_.at(idx).lighter(130));
			p.setPen(QPen(p.brush().color().lighter(130), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		}
		p.drawPolygon(poly);
	};

	for (int idx = 0; idx < polygons_.count(); ++idx) {
		if (idx == hovered_idx_)
			continue;
		drawPolygon(idx);
	}
	if (hovered_idx_ >= 0)
		drawPolygon(hovered_idx_);

	e->accept();
}

void AppWindow::mousePressEvent(QMouseEvent *e)
{
	reset();
	e->accept();
}

void AppWindow::mouseMoveEvent(QMouseEvent *e)
{
	QPointF cursor_pos = e->pos();
	int idx = 0;
	for (const LinkedTriangle &polygon: polygons_) {
		QPolygonF poly;
		poly << *polygon.v1
			 << *polygon.v2
			 << *polygon.v3;
		if (poly.containsPoint(cursor_pos, Qt::OddEvenFill)) {
			hovered_idx_ = idx;
			break;
		}
		++idx;
	}
	recountAntigravityForces(e->pos());
	update();
	e->accept();
}

void AppWindow::generateColors()
{
	polygon_colors_.clear();

	const QColor base_clr ("#265080");
	auto generateColor = [&base_clr] () {
		const qreal base_v = base_clr.valueF();
		const bool prob_trigger = !bool((qrand()%15));
		const qreal s = prob_trigger * base_clr.saturationF();
		qreal v = base_v + (qrand()%200/200.-0.5) * 0.25;
		v = v - !prob_trigger * v * 0.7;
		return QColor::fromHsvF(base_clr.hueF(), s, v);
	};

	for (int i=0; i<polygons_.count(); ++i) {
		polygon_colors_ << generateColor();
	}
}

void AppWindow::recountAntigravityForces(const QPointF &particle_pos)
{
	deltas_.clear();
	const qreal k = 200.;
	const qreal m_point = 2.;
	const qreal m_particle = 2.;
	std::for_each(points_.begin(), points_.end(), [&] (const QPointF* p) {
		QPointF v = *p - particle_pos;
		const qreal r = qSqrt(qPow(v.x(), 2.) + qPow(v.y(), 2.));
		const qreal force = k * m_point * m_particle / (r + 50);

		const QPointF normalized_v = v / r;
		const QPointF force_v = normalized_v * force;
		deltas_[p] = force_v;
	});
}
