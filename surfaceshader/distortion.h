#ifndef DISTORTION_H
#define DISTORTION_H

#include <QtGui/QVector3D>

class Mesh;

class Distortion
{
public:
	Distortion();
	~Distortion();

	const QVector3D &position() const;
	void setPosition(const QVector3D &v);

	/**
	 * Calculates translations for each vertex of the mesh
	 */
	QList<QVector3D> calcDistortion(const Mesh &mesh);

private:
	QVector3D v_;
};

#endif // DISTORTION_H
