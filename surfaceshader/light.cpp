#include "light.h"

Light::Light(const QVector3D &v, const QColor &diffuse, const QColor &ambient)
	: v_(v)
	, clr_diffuse_(diffuse)
	, clr_ambient_(ambient)
{}

Light::~Light()
{}

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

