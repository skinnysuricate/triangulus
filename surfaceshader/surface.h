#ifndef SURFACE_H
#define SURFACE_H

#include "mesh.h"
#include "material.h"

class Surface
{
public:
	Surface(const QSize &size, const Material &material, qreal mesh_density);
	~Surface();

	const QSize &size() const { return size_; }
	const Mesh &mesh() const { return mesh_; }
	const Material &material() const { return material_; }

private:
	static QRect coveringRect(const QSize &size);

	QSize size_;
	Mesh mesh_;
	Material material_;
};

#endif // SURFACE_H
