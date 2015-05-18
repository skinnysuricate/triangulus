#include "mesh.h"

#include <utility>

Mesh::Mesh(QList<Triangle> &&triangles)
	: polygons_(triangles)
{}

Mesh::Mesh(Mesh &&mesh)
	: polygons_(std::move(mesh.polygons_))
{}

Mesh::~Mesh()
{}

