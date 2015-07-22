#include "light.h"

#include <QDebug>
#include "cluster.h"

Light::Light(const QVector3D &v, const QColor &diffuse, const QColor &ambient, qreal decay, Cluster *cluster)
	: v_(v)
	, clr_diffuse_(diffuse)
	, clr_ambient_(ambient)
	, fading_(decay)
	, cluster_(cluster)
{}

Light::~Light()
{}

QVector3D Light::position() const
{
	QVector3D v = v_;
	if (cluster_)
		v = v + cluster_->position();

//	qDebug() << __FUNCTION__ << v;

	return v;
}

void Light::setPosition(const QVector3D &v)
{
	v_ = v;
}

void Light::setPosition(const QPointF &pos)
{
	v_.setX(pos.x());
	v_.setY(pos.y());
}

void Light::setDiffuse(const QColor &diffuse)
{
	clr_diffuse_ = diffuse;
}

void Light::setAmbient(const QColor &ambient)
{
	clr_ambient_ = ambient;
}

