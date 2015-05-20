#include "distortion.h"
#include "mesh.h"

Distortion::Distortion(const QVector3D &v) : v_(v)
{}

Distortion::~Distortion()
{}

void Distortion::setPosition(const QVector3D &v)
{
	v_ = v;
}

QHash<QVector3D*, QVector3D> Distortion::calcDistortion(const Mesh &mesh)
{
	QHash<QVector3D*, QVector3D> offsets;
	const qreal k = 200.;
	const qreal m_point = 2.;
	const qreal m_particle = 2.;
	std::for_each(mesh.vertexes().begin(), mesh.vertexes().end(), [&] (const QVector3D* p) {
		QVector3D v = *p - particle_pos;
		const qreal r = qSqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
		const qreal force = k * m_point * m_particle / (r*qPow(r, 0.1) + 70);

		const QVector3D normalized_vec = v / r;
		const QVector3D force_vec = normalized_vec * force;
		offsets[p] = force_vec;
	});
}
