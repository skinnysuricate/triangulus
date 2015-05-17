#ifndef LIGHT_H
#define LIGHT_H

#include <QtGui/QColor>
#include <QtGui/QVector3D>

class Light
{
public:
	Light(const QVector3D &v, const QColor &diffuse, const QColor &ambient, qreal decay = 0.);
	~Light();

	const QVector3D &position() const { return v_; }
	const QColor &diffuse() const { return clr_diffuse_; }
	const QColor &ambient() const { return clr_ambient_; }
	qreal fading() const { return fading_; }

	void setPosition(const QVector3D &v);
	void setPosition(const QPointF &pos);
	void setDiffuse(const QColor &diffuse);
	void setAmbient(const QColor &ambient);
	void setFading(qreal fading);

private:
	QVector3D v_;
	QColor clr_diffuse_;
	QColor clr_ambient_;
	qreal fading_;				// brightness decay per 100px of distance,
								// e.g. 0.05 of 1. per 100px
};

#endif // LIGHT_H
