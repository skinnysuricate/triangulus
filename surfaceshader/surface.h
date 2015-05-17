#ifndef SURFACE_H
#define SURFACE_H

#include "geometry.h"
#include "material.h"

class Surface
{
public:
	Surface(const Material &m, Geometry &&g);
	~Surface();

private:
	Geometry geometry_;
	Material material_;
};

#endif // SURFACE_H
