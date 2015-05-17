#include "appwindow.h"

#include <memory>
#include <QtCore/QSet>
#include <QtCore/QRectF>
#include <QtGui/QPainter>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QVariantAnimation>
#include <QtCore/QtMath>
#include <QtGui/QVector3D>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>

//#define HOVER_POLYGON
//#define HIGHLIGHT_VERTEXES
//#define ANIMATED_MESH

AppWindow::AppWindow(QWidget *parent)
	: QWidget(parent)
	, light_sources_{Light(QVector3D(200, 100, 100), "#FFDDC2", "#333", 0.02),
					 Light(QVector3D(50, 150, 50), "#92F1FF", "#333", 0.03),
					 /*Light(QVector3D(0, 0, 200), "#0C98FF", "#333", 0.0511)*/}
{
	setMouseTracking(true);
#ifdef ANIMATED_MESH
	animator_.reset(new QVariantAnimation);
	animator_->setLoopCount(-1);
	animator_->setStartValue(0.);
	animator_->setEndValue(1.);
	animator_->setDuration(1000);
	connect(animator_.get(), QVariantAnimation::valueChanged, this, [this] (const QVariant &v) {
		progress_ = v.toReal();
		recountWaveState();
		recountColors();
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

	const int x_offset = width() * 0.1;
	const int y_offset = height() * 0.1;
	const QRectF bound (-x_offset, -y_offset, width() * 1.2, height() * 1.2);

	qsrand(uint(&bound) + uint(QDateTime::currentMSecsSinceEpoch()));
	auto randomOffset = [] (int limit) {
		return qrand()%limit - limit;
	};

	for (int i = -x_offset; i < width() + x_offset; i+=60+randomOffset(20)) {
		points_ << new QVector3D(qreal(i), 1., 0.);
		points_ << new QVector3D(qreal(i), height() -1., 0.);
	}

	for (int i = -y_offset; i < height() + y_offset; i+=60+randomOffset(20)) {
		points_ << new QVector3D(1., qreal(i), 0.);
		points_ << new QVector3D(width() - 1., qreal(i), 0.);
	}

	const quint32 min_distance = 20;
	const quint32 square = width() * height();
	const quint32 max_v_count = square / qPow(min_distance, 2.);
	const quint32 v_count = qMin(quint32(250), max_v_count);

	auto isSuitable = [this, &min_distance] (const QVector3D &p) {
		for (QVector3D *existant: points_) {
			if (qAbs((p - *existant).toPointF().manhattanLength()) < min_distance)
				return false;
		}
		return true;
	};

	for (uint i = 0; i < v_count; ++i) {
		QVector3D new_p (bound.width() * (qrand() % 1001 / 1000.),
						 bound.height() * (qrand() % 1001 / 1000.),
						 qrand() % 1001 * 0.001 * 1.1);
		if (!isSuitable(new_p)) {
			--i;
			continue;
		}
		points_ << new QVector3D(new_p);
	}

	polygons_ = std::move(Triangulator::triangulatePersistant(points_));
	recountColors();
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

#ifdef HIGHLIGHT_VERTEXES
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
#endif

	e->accept();
}

void AppWindow::mousePressEvent(QMouseEvent *e)
{
	reset();
	earthquake_pos_ = e->pos();
	e->accept();
}

void AppWindow::mouseMoveEvent(QMouseEvent *e)
{
#ifdef HOVER_POLYGON
	QPointF cursor_pos = e->pos();
	int idx = 0;
	for (const LinkedTriangle &polygon: polygons_) {
		QPolygonF poly;
		poly << polygon.v1->toPointF()
			 << polygon.v2->toPointF()
			 << polygon.v3->toPointF();
		if (poly.containsPoint(cursor_pos, Qt::OddEvenFill)) {
			hovered_idx_ = idx;
			break;
		}
		++idx;
	}
#endif
	recountAntigravityForces(QVector3D(e->pos()));
	light_sources_[0].setPosition(e->pos());
	recountColors();
	update();
	e->accept();
}

void AppWindow::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	// TODO: Do something
}

void AppWindow::recountColors()
{
	polygon_colors_.clear();

#ifdef UNUSED
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
#endif
//	blue scene
//	const QColor material_diffuse ("#0A446B");
//	const QColor material_ambient ("#003152");
	const QColor material_diffuse ("#777");
	const QColor material_ambient ("#777");

	auto centroid = [] (const LinkedTriangle &p) {
		return QVector3D((p.v1->x() + p.v2->x() + p.v3->x()) / 3.,
						 (p.v1->y() + p.v2->y() + p.v3->y()) / 3.,
						 (p.v1->z() + p.v2->z() + p.v3->z()) / 3.);
	};

	auto soft_light = [] (qreal a, qreal b) {
		return 2.*b*a + (1.-2.*b)*a*a;
	};

	auto blend_diffusion = [&] (const QColor &c1, const QColor &c2, qreal radiance, qreal brightness) {
		qreal k = radiance * brightness;
//		QColor tmp = QColor::fromHsvF(c2.hueF(), c2.saturationF()*brightness, c2.valueF()*brightness);
		return QColor::fromRgbF(soft_light(c1.redF(), c2.redF() * k),
								soft_light(c1.greenF(), c2.greenF() * k),
								soft_light(c1.blueF(), c2.blueF() * k));
	};

	auto blend_ambient = [&] (const QColor &c1, const QColor &c2) {
		return QColor::fromRgbF(soft_light(c1.redF(), c2.redF()),
								soft_light(c1.greenF(), c2.greenF()),
								soft_light(c1.blueF(), c2.blueF()));
	};

	auto add = [] (const QColor &c1, const QColor &c2) {
		return QColor::fromRgbF(qMin(c1.redF() + c2.redF(), 1.),
								qMin(c1.greenF() + c2.greenF(), 1.),
								qMin(c1.blueF() + c2.blueF(), 1.));
	};

	QColor scene_ambient = material_ambient;
	for (const Light &light: light_sources_) {
		scene_ambient = blend_ambient(scene_ambient, light.ambient());
	}

	for (const LinkedTriangle &polygon: polygons_) {
		const QVector3D c = centroid(polygon);
		const QVector3D normal = QVector3D::normal(*polygon.v1, *polygon.v2, *polygon.v3);
		QColor polygon_diffuse = material_diffuse;
		for (const Light &light: light_sources_) {
			const QVector3D ray = (c - light.position()).normalized();
			const qreal distance = c.distanceToPoint(light.position());
			qreal dot_product = qMax(0.f, QVector3D::dotProduct(normal, ray));
			polygon_diffuse = blend_diffusion(polygon_diffuse,
											  light.diffuse(),
											  dot_product,
											  1. - qMin(1., distance*light.fading()*0.01));
		}
		polygon_colors_ << add(polygon_diffuse, scene_ambient);
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

void AppWindow::recountWaveState()
{
	const qreal decay_ratio = 0.3 * 0.01; // % per 100px
	for (QVector3D *point: points_) {
		const qreal distance = QLineF(point->toPointF(), earthquake_pos_).length();
		const qreal amplitude = 5. * (1. - qMin(1., decay_ratio * distance));
		point->setZ(qMax(0., amplitude * (-0.8+qSin(2.*M_PI * distance * 0.011 - 2*M_PI*progress_))));
	}
}
