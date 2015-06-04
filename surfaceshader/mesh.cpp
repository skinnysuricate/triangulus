#include "mesh.h"

#include <utility>
#include <QtCore/QDateTime>
#include <QtGui/QVector3D>
#include "triangulator.h"

Mesh::Mesh(const QRect &area, qreal density)
{
	generateVertexes(area, density);
	polygons_ = std::move(Triangulator::buildMesh(vertexes_));
}

Mesh::Mesh(Mesh &&mesh)
	: vertexes_(std::move(mesh.vertexes_))
	, polygons_(std::move(mesh.polygons_))
{}

Mesh::~Mesh()
{
	qDeleteAll(vertexes_);
}

Mesh &Mesh::operator=(Mesh &&other)
{
	vertexes_ = std::move(other.vertexes_);
	polygons_ = std::move(other.polygons_);
	return *this;
}

void Mesh::generateVertexes(const QRect &area, qreal /*density*/)
{
	qDeleteAll(vertexes_);
	vertexes_.clear();

	qsrand(uint(&area) + uint(QDateTime::currentMSecsSinceEpoch()));

	auto randomOffset = [] (int limit) {
		return qrand()%limit - limit;
	};

	for (int i = area.left(); i < area.right(); i+=60+randomOffset(20)) {
		vertexes_ << new QVector3D(qreal(i), 1., 0.);
		vertexes_ << new QVector3D(qreal(i), area.height() -1., 0.);
	}

	for (int i = area.top(); i < area.bottom(); i+=60+randomOffset(20)) {
		vertexes_ << new QVector3D(1., qreal(i), 0.);
		vertexes_ << new QVector3D(area.width() - 1., qreal(i), 0.);
	}

	const quint32 min_distance = 20;
	const quint32 square = area.width() * area.height();
	const quint32 max_v_count = square / (min_distance * min_distance);
	const quint32 v_count = qMin(quint32(300), max_v_count);

	auto isSuitable = [this, &min_distance] (const QVector3D &p) {
		for (QVector3D *existant: vertexes_) {
			if (qAbs((p - *existant).toPointF().manhattanLength()) < min_distance)
				return false;
		}
		return true;
	};

	for (uint i = 0; i < v_count; ++i) {
		QVector3D new_p (area.width() * (qrand() % 1001 / 1000.),
						 area.height() * (qrand() % 1001 / 1000.),
						 qrand() % 1001 * 0.001 * 2.1);
		if (!isSuitable(new_p)) {
			--i;
			continue;
		}
		vertexes_ << new QVector3D(new_p);
	}
}
