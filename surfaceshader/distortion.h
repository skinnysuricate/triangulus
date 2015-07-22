#ifndef DISTORTION_H
#define DISTORTION_H

#include <QtCore/QHash>
#include <QtGui/QVector3D>

class Mesh;
class Cluster;

class Distortion
{
public:
	Distortion(const QVector3D &v = {}, Cluster *cluster = nullptr);
	~Distortion();

	QVector3D position() const;
	void setPosition(const QVector3D &v);

	/**
	 * Calculates translations for each vertex of the mesh
	 */
	QHash<const QVector3D *, QVector3D> calcDistortion(const Mesh &mesh) const;

private:
	QVector3D v_;
	Cluster *cluster_;
};

#endif // DISTORTION_H
