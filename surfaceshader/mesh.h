#ifndef MESH_H
#define MESH_H

#include <QtCore/QRect>
#include <QtCore/QList>
#include "triangle.h"

/**
 * The Mesh class
 */
class Mesh
{
public:
	explicit Mesh(const QRect &area, qreal density);
	Mesh(Mesh &&other);
	~Mesh();

	const QVector<QVector3D*> &vertexes() const { return vertexes_; }
	const QList<Triangle> &polygons() const { return polygons_; }

private:

	/**
	 * [density] is a number of vertexes per rect sized [10% of width] * [10% of height]
	 */
	void generateVertexes(const QRect &area, qreal density);

	QList<QVector3D*> vertexes_;
	QList<Triangle> polygons_;
};

#endif // MESH_H
