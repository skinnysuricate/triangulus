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
	Mesh &operator=(Mesh &&rhs);

	const QList<QVector3D*> &vertexes() const { return vertexes_; }
	const QList<Triangle> &polygons() const { return polygons_; }
	const QRect &boundingRect() const { return rect_; }

	void adjust(const QRect &area);

private:

	void generateBoundVertexes(const QRect &area);
	void generateVertexes(const QRect &area);
	void generateVertexesVoronoi(const QRect &area);
	void clearVertexes();

	/**
	 * [density] is a number of vertexes per rect sized [10% of width] * [10% of height]
	 */
	qreal density_;
	QRect rect_;
	QList<QVector3D*> vertexes_;
	QList<Triangle> polygons_;
};

#endif // MESH_H
