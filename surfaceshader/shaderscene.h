#ifndef SHADERSCENE_H
#define SHADERSCENE_H

#include <memory>
#include <QObject>
#include <QList>
#include <QMap>
#include <QHash>

#include "surface.h"
#include "cluster.h"
#include "animator.h"
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

	quint64 add(const Light &l);
	quint64 add(const Distortion &d);
	quint64 add(std::unique_ptr<Cluster> cluster);
	void add(Surface s);

	void move(quint64 id, const QPoint &pos);

	void render(QPaintDevice *context) const;

signals:
	void invalidated();

private:
	void invalidate();
	void process() const;
	void processLights() const;
	void processDistortions() const;
	QPolygonF getPolygon(const Triangle &t) const;

	Surface surface_;

	QHash<quint64, Light> lights_;
	QHash<quint64, Distortion> distortions_;
	QHash<quint64, Cluster*> clusters_;
	QList<Animator> animators_;

	quint64 index_;

	bool locked_updates_;
	mutable bool processed_;

	std::unique_ptr<QVariantAnimation> animator_;
	QVector3D animator_pos_;

	mutable QHash<const QVector3D*, QVector3D> distortion_impacts_;
	mutable QList<QColor> polygon_colors_;
};

#endif // SHADERSCENE_H
