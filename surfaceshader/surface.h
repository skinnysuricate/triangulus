#ifndef SURFACE_H
#define SURFACE_H

#include "mesh.h"
#include "material.h"

class Surface
{
public:
	Surface(const Material &material, Mesh &&mesh);
	~Surface();

private:
	Mesh mesh_;
	Material material_;
};

#endif // SURFACE_H
