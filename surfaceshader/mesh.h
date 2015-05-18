#ifndef MESH_H
#define MESH_H

#include <QtCore/QList>
#include "triangle.h"

/**
 * The Mesh class
 */
class Mesh
{
public:
	explicit Mesh(QList<Triangle> &&triangles);
	Mesh(Mesh &&other);
	~Mesh();

	QList<Triangle> polygons() const { return polygons_; }

private:
	QList<Triangle> polygons_;
};

#endif // MESH_H
