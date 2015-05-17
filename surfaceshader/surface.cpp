#include "surface.h"

#include <utility>

Surface::Surface(const Material &m, Geometry &&g)
	: material_(m)
	, geometry_(std::move(g))
{}

Surface::~Surface()
{}

