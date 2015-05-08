#ifndef LIGHT_H
#define LIGHT_H

#include <QtGui/QColor>
#include <QtGui/QVector3D>

class Light
{
public:
	Light(const QVector3D &v, const QColor &diffuse, const QColor &ambient);
	~Light();

	const QVector3D &position() const { return v_; }
	const QColor &diffuse() const { return clr_diffuse_; }
	const QColor &ambient() const { return clr_ambient_; }

	void setPosition(const QVector3D &v);
	void setPosition(const QPointF &pos);
	void setDiffuse(const QColor &diffuse);
	void setAmbient(const QColor &ambient);

private:
	QVector3D v_;
	QColor clr_diffuse_;
	QColor clr_ambient_;
};

#endif // LIGHT_H
