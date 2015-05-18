#include "material.h"

Material::Material(const QColor &diffuse, const QColor &ambient)
	: clr_diffuse_(diffuse)
	, clr_ambient_(ambient)
{}

Material::~Material()
{}

