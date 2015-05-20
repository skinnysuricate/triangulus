#ifndef DISTORTION_H
#define DISTORTION_H

#include <QtGui/QVector3D>

class Mesh;

class Distortion
{
public:
	Distortion(const QVector3D &v = {});
	~Distortion();

	const QVector3D &position() const { return v_; }
	void setPosition(const QVector3D &v);

	/**
	 * Calculates translations for each vertex of the mesh
	 */
	QHash<QVector3D*, QVector3D> calcDistortion(const Mesh &mesh);

private:
	QVector3D v_;
};

#endif // DISTORTION_H
