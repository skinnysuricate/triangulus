#ifndef MATERIAL_H
#define MATERIAL_H

#include <QtGui/QColor>

/**
 * The Material class describes surface color properties
 */
class Material
{
public:
	Material(const QColor &diffuse, const QColor &ambient);
	~Material();

	const QColor &diffuse() const { return clr_diffuse_; }
	const QColor &ambient() const { return clr_ambient_; }

private:
	QColor clr_diffuse_;
	QColor clr_ambient_;
};

#endif // MATERIAL_H
