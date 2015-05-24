#ifndef SHADERSCENE_H
#define SHADERSCENE_H

#include <memory>
#include <QObject>
#include <QList>
#include <QVector>
#include <QHash>
#include "surface.h"
#include "light.h"
#include "distortion.h"

class QPaintDevice;
class QVariantAnimation;
class Material;
class Triangle;

class ShaderScene : public QObject
{
	Q_OBJECT
public:
	explicit ShaderScene(const QSize &s, const Material &m, qreal density, QObject *parent = 0);
	~ShaderScene();

	QSize size() const { return surface_.size(); }

	void beginBuildScene();
	void endBuildScene();

	ShaderScene &add(const Light &l, bool mouse_binded = false);
	ShaderScene &add(const Distortion &d, bool mouse_binded = false);
	ShaderScene &add(Surface s);

	void render(QPaintDevice *context) const;
	void handleMouseMove(const QPoint &pos);

signals:
	void invalidated();

private:
	void invalidate();
	void process() const;
	void processLights() const;
	void processDistortions() const;
	QPolygonF getPolygon(const Triangle &t) const;

#ifdef TEMPLATES
	template <class SceneElement>
	QList<SceneElement> &container();
	template <>
	QList<Light> &container<Light>() { return lights_; }
	template <>
	QList<Distortion> &container<Distortion>() { return distortions_; }
#endif

	Surface surface_;
	QList<Light> lights_;
	QList<Distortion> distortions_;
	QVector<uint> binded_light_indexes_;
	QVector<uint> binded_distortion_indexes_;
	bool locked_updates_;
	mutable bool processed_;

	mutable QHash<const QVector3D*, QVector3D> distortion_impacts_;
	mutable QList<QColor> polygon_colors_;
};

#endif // SHADERSCENE_H
