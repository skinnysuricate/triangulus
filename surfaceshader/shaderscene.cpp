#include "shaderscene.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintDevice>

ShaderScene::ShaderScene(QObject *parent)
	: QObject(parent)
{}

ShaderScene::~ShaderScene()
{

}

void ShaderScene::beginBuildScene()
{
	lock_updates_ = true;
}

void ShaderScene::endBuildScene()
{
	lock_updates_ = false;

}

ShaderScene &ShaderScene::add(const Light &l, bool mouse_binded)
{

}

ShaderScene &ShaderScene::add(const Distortion &d, bool mouse_binded)
{

}

ShaderScene &ShaderScene::add(Surface s)
{

}

void ShaderScene::render(QPaintDevice *context)
{
	QPainter p;
	p.begin(context);

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

