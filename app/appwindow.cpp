#include "appwindow.h"

#include <memory>
#include <QtCore/QSet>
#include <QtGui/QPainter>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QVariantAnimation>
#include <QtCore/QtMath>
#include <QtGui/QVector3D>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>

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
		points_ << new QVector3D(qreal(i), 1., 0.);
		points_ << new QVector3D(qreal(i), height() -1., 0.);
	}

	for (int i = -y_offset; i < height() + y_offset; i+=40+randomOffset(20)) {
		points_ << new QVector3D(1., qreal(i), 0.);
		points_ << new QVector3D(width() - 1., qreal(i), 0.);
	}

	auto isSuitable = [this] (const QVector3D &p) {
		for (QVector3D *existant: points_) {
			if (qAbs((p - *existant).toPointF().manhattanLength()) < 20)
				return false;
		}
		return true;
	};

	for (int i = 0; i < 100; ++i) {
		QVector3D new_p (bound.width() * (qrand() % 1001 / 1000.), bound.height() * (qrand() % 1001 / 1000.), qrand() % 1001 * 0.001 * 20 - 10);
		if (!isSuitable(new_p)) {
			--i;
			continue;
		}
		points_ << new QVector3D(new_p);
	}

	polygons_ = std::move(Triangulator::triangulatePersistant(points_));
	generateColors();
	recountAntigravityForces(QVector3D(mapFromGlobal(QCursor::pos())));
	update();
}

void AppWindow::paintEvent(QPaintEvent *e)
{
	QPainter p (this);
	p.setClipRegion(e->region());
	p.setRenderHint(QPainter::Antialiasing);
	p.fillRect(rect(), QColor(40, 40, 40));

	auto drawPolygon = [&] (int idx) {
		const LinkedTriangle &polygon = polygons_.at(idx);
		QPolygonF poly;
		poly << (*polygon.v1 + deltas_.value(polygon.v1) * progress_).toPointF()
			 << (*polygon.v2 + deltas_.value(polygon.v2) * progress_).toPointF()
			 << (*polygon.v3 + deltas_.value(polygon.v3) * progress_).toPointF();
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

	qreal max = 1.;
	qreal med = 0.;
	for (const QVector3D &delta: deltas_.values()) {
		qreal m_length = delta.toPointF().manhattanLength();
		med += m_length;
		if (m_length > max)
			max = m_length;
	}
	med /= qreal(deltas_.size());
	med = qMax(1., (max + med) * 0.8);

	for (QVector3D *point: points_) {
		if (deltas_.contains(point)) {
			const QVector3D delta = deltas_.value(point);
			p.setPen(Qt::NoPen);
			p.setBrush(Qt::white);
			p.setOpacity(delta.toPointF().manhattanLength() / max);
			p.drawEllipse((*point + delta).toPointF(), 1.5, 1.5);
		}
	}

	e->accept();
}

void AppWindow::mousePressEvent(QMouseEvent *e)
{
	reset();
	e->accept();
}

void AppWindow::mouseMoveEvent(QMouseEvent *e)
{
#ifdef UNUSED
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
#endif
	recountAntigravityForces(QVector3D(e->pos()));
	update();
	e->accept();
}

void AppWindow::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	// TODO: Do something
}

void AppWindow::generateColors()
{
	polygon_colors_.clear();

	const QColor base_clr ("#265080");
	auto generateColor = [&base_clr] () {
		const qreal base_v = base_clr.valueF();
		const bool prob_trigger = !bool((qrand()%15));
		const qreal s = prob_trigger * base_clr.saturationF();
		qreal v = base_v + (qrand()%200/200.-0.5) * 0.2;
		v = v - !prob_trigger * v * 0.7;
		return QColor::fromHsvF(base_clr.hueF(), s, v);
	};

	for (int i=0; i<polygons_.count(); ++i) {
		polygon_colors_ << generateColor();
	}
}

void AppWindow::recountAntigravityForces(const QVector3D &particle_pos)
{
	deltas_.clear();
	const qreal k = 200.;
	const qreal m_point = 2.;
	const qreal m_particle = 2.;
	std::for_each(points_.begin(), points_.end(), [&] (const QVector3D* p) {
		QVector3D v = *p - particle_pos;
		const qreal r = qSqrt(qPow(v.x(), 2.) + qPow(v.y(), 2.));
		const qreal force = k * m_point * m_particle / (r*qPow(r, 0.1) + 70);

		const QVector3D normalized_v = v / r;
		const QVector3D force_v = normalized_v * force;
		deltas_[p] = force_v;
	});
}
