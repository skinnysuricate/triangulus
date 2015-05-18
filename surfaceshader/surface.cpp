#include "surface.h"

#include <utility>

Surface::Surface(const Material &material, Mesh &&mesh)
	: mesh_(mesh)
	, material_(material)
{}

Surface::~Surface()
{}

