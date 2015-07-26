#include "surface.h"

#include <utility>

Surface::Surface(const QSize &size, const Material &material, qreal mesh_density)
	: size_(size)
	, mesh_(QRect(), mesh_density)
	, material_(material)
{}

Surface::~Surface()
{}

Surface::Surface(Surface &&other)
	: size_(other.size_)
	, mesh_(std::move(other.mesh_))
	, material_(other.material_)
{}

Surface &Surface::operator=(Surface &&other)
{
	size_ = other.size_;
	mesh_ = std::move(other.mesh_);
	material_ = other.material_;
	return *this;
}

void Surface::adjustSize(const QSize &s)
{
	mesh_.adjust(coveringRect(s));
	size_ = s;
}

QRect Surface::coveringRect(const QSize &size)
{
	const qreal x_margin = 50;
	const qreal y_margin = 50;
	return QRect({0, 0}, size).adjusted(-x_margin, -y_margin,
										x_margin, y_margin);
}

