#include "shaderscene.h"

#include <utility>
#include <QtGui/QPainter>
#include <QtGui/QPaintDevice>
#include <QtCore/QDebug>
#include <QtCore/QVariantAnimation>

ShaderScene::ShaderScene(const QSize &s, const Material &m, qreal density, QObject *parent)
	: QObject(parent)
	, surface_(s, m , density)
	, locked_updates_(false)
	, processed_(false)
{
	animator_.reset(new QVariantAnimation);
	animator_->setStartValue(0.);
	animator_->setEndValue(1.);
	animator_->setDuration(3000);
	animator_->setLoopCount(-1);
	connect(animator_.get(), &QVariantAnimation::valueChanged, this, [this] (const QVariant &value) {
		const qreal progress = value.toReal();

		const qreal R = 40;
		const qreal r = R * 0.25;
		const qreal h = R;
		const qreal phi = 2 * 3.14 * progress;
		const qreal x = (R - r) * cos(phi) + h * cos((R/r - 1) * phi);
		const qreal y = (R - r) * sin(phi) - h * sin((R/r - 1) * phi);
//		const qreal x = 50 * sin(4 * 3.14 * progress) * 0.5;

		for (uint idx: binded_distortion_indexes_) {
			QVector3D v = distortions_.at(idx).position();
			const int sign = 1;//-1 + (idx % 2) * 2;
			v.setX(animator_pos_.x() + x * sign);
			v.setY(animator_pos_.y() + y * sign);
			distortions_[idx].setPosition(v);
		}

		for (uint idx: binded_light_indexes_) {
			QVector3D v = lights_.at(idx).position();
			const int sign = 1;//-1 + (idx % 2) * 2;
			v.setX(animator_pos_.x() + x * sign);
			v.setY(animator_pos_.y() + y * sign);
			lights_[idx].setPosition(v);
		}

		invalidate();
	});
	animator_->start();
}

ShaderScene::~ShaderScene()
{}

void ShaderScene::beginBuildScene()
{
	locked_updates_ = true;
}

void ShaderScene::endBuildScene()
{
	locked_updates_ = false;
	invalidate();
}

ShaderScene &ShaderScene::add(const Light &l, bool mouse_binded)
{
	lights_.append(l);
	if (mouse_binded)
		binded_light_indexes_.append(lights_.count() - 1);
	if (!locked_updates_)
		invalidate();
	return *this;
}

ShaderScene &ShaderScene::add(const Distortion &d, bool mouse_binded)
{
	distortions_.append(d);
	if (mouse_binded)
		binded_distortion_indexes_.append(distortions_.count() - 1);
	if (!locked_updates_)
		invalidate();
	return *this;
}

ShaderScene &ShaderScene::add(Surface s)
{
	surface_ = std::move(s);
	if (!locked_updates_)
		invalidate();
	return *this;
}

void ShaderScene::render(QPaintDevice *context) const
{
	Q_ASSERT_X(!locked_updates_, Q_FUNC_INFO, "Called rendering while is being built. Abort.");
	if (locked_updates_)
		return;

	if (!processed_)
		process();

	QPainter p;
	p.begin(context);

	const qreal x_scale = context->width() / qreal(size().width());
	const qreal y_scale = context->height() / qreal(size().height());
	p.scale(x_scale, y_scale);

//	p.setRenderHint(QPainter::Antialiasing);
	p.fillRect(QRect(0, 0, context->width(), context->height()), QColor(40, 40, 40));

	auto drawPolygon = [&] (int idx) {
		const Triangle &triangle = surface_.mesh().polygons().at(idx);
		QPolygonF polygon = getPolygon(triangle);
		p.setBrush(polygon_colors_.at(idx));
		p.setPen(Qt::NoPen);
//		p.setPen(p.brush().color());
		p.drawPolygon(polygon);
	};

	for (int idx = 0; idx < surface_.mesh().polygons().count(); ++idx)
		drawPolygon(idx);

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
}

void ShaderScene::handleMouseMove(const QPoint &pos)
{
	if (binded_distortion_indexes_.isEmpty() && binded_light_indexes_.isEmpty())
		return;

	animator_pos_.setX(pos.x());
	animator_pos_.setY(pos.y());

//	for (uint idx: binded_distortion_indexes_) {
//		QVector3D v = distortions_.at(idx).position();
//		v.setX(pos.x());
//		v.setY(pos.y());
//		distortions_[idx].setPosition(v);
//	}

//	for (uint idx: binded_light_indexes_) {
//		QVector3D v = lights_.at(idx).position();
//		v.setX(pos.x());
//		v.setY(pos.y());
//		lights_[idx].setPosition(v);
//	}

//	invalidate();
}

void ShaderScene::invalidate()
{
	processed_ = false;
	emit invalidated();
}

void ShaderScene::process() const
{
	processLights();
	processDistortions();
	processed_ = true;
}

void ShaderScene::processLights() const
{
	polygon_colors_.clear();

#ifdef OLD_GENERATOR
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

	auto centroid = [] (const Triangle &p) {
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

	QColor scene_ambient = surface_.material().ambient();
	for (const Light &light: lights_) {
		scene_ambient = blend_ambient(scene_ambient, light.ambient());
	}

	for (const Triangle &polygon: surface_.mesh().polygons()) {
		const QVector3D c = centroid(polygon);
		const QVector3D normal = QVector3D::normal(*polygon.v1, *polygon.v2, *polygon.v3);
		QColor polygon_diffuse = surface_.material().diffuse();
		for (const Light &light: lights_) {
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

void ShaderScene::processDistortions() const
{
	distortion_impacts_.clear();
	for (const Distortion &d: distortions_) {
		auto impacts = std::move(d.calcDistortion(surface_.mesh()));
		auto it = impacts.cbegin();
		for (; it != impacts.cend(); ++it)
			distortion_impacts_[it.key()] += it.value();
	}
}

QPolygonF ShaderScene::getPolygon(const Triangle &t) const
{
	QPolygonF p;
	p << (*t.v1 + distortion_impacts_.value(t.v1)).toPointF()
	  << (*t.v2 + distortion_impacts_.value(t.v2)).toPointF()
	  << (*t.v3 + distortion_impacts_.value(t.v3)).toPointF();
	return p;
}

