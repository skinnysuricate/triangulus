#include "surface.h"

#include <utility>

Surface::Surface(const QSize &size, const Material &material, qreal mesh_density)
	: size_(size)
	, mesh_(coveringRect(size), mesh_density)
	, material_(material)
{}

Surface::~Surface()
{}

QRect Surface::coveringRect(const QSize &size)
{
	const qreal x_margin = 0.05 * size.width();
	const qreal y_margin = 0.05 * size.height();
	return QRect({0, 0}, size).adjusted(-x_margin, -y_margin,
										x_margin, y_margin);
}

