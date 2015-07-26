#include "mesh.h"

#include <utility>
#include <QtCore/QDateTime>
#include <QtGui/QVector3D>
#include "triangulator.h"

Mesh::Mesh(const QRect &area, qreal density)
	: density_(density)
	, rect_(area)
{
#ifdef DISABLED
	generateBoundVertexes(area);
	generateVertexes(area);
	polygons_ = std::move(Triangulator::buildMesh(vertexes_));
#endif
}

Mesh::Mesh(Mesh &&other)
	: density_(other.density_)
	, rect_(other.rect_)
	, vertexes_(std::move(other.vertexes_))
	, polygons_(std::move(other.polygons_))
{}

Mesh::~Mesh()
{
	qDeleteAll(vertexes_);
}

Mesh &Mesh::operator=(Mesh &&rhs)
{
	density_ = rhs.density_;
	rect_ = rhs.rect_;
	vertexes_ = std::move(rhs.vertexes_);
	polygons_ = std::move(rhs.polygons_);
	return *this;
}

void Mesh::adjust(const QRect &area)
{
	if (area == rect_) return;

	polygons_.clear();

	// check sides
	const bool adjust_left = area.left() != rect_.left();
	const bool adjust_top = area.top() != rect_.top();
	const bool adjust_right = area.right() != rect_.right();
	const bool adjust_bottom = area.bottom() != rect_.bottom();

	// remove vertexes beyond the area or on prev area bound
	for (auto it = vertexes_.begin(); it != vertexes_.end();) {
		QVector3D *v = *it;
		if (   (adjust_left		&&		(v->x() <= area.left()		|| v->x() == rect_.left()))
			|| (adjust_top		&&		(v->y() <= area.top()		|| v->y() == rect_.top()))
			|| (adjust_right	&&		(v->x() >= area.right()		|| v->x() == rect_.right()))
			|| (adjust_bottom	&&		(v->y() >= area.bottom()	|| v->y() == rect_.bottom())) ) {
			it = vertexes_.erase(it);
			delete v;
			continue;
		}
		++it;
	}

	// add border vertexes, if where removed
	qsrand(uint(&area) + uint(QDateTime::currentMSecsSinceEpoch()));
	auto randomOffset = [] (int limit) {
		return qrand()%limit - limit;
	};

	if (adjust_left)
		for (int i = area.top(); i < area.bottom(); i+=60+randomOffset(20))
			vertexes_ << new QVector3D(area.left(), qreal(i), qrand() % 1001 * 0.001 * 4.1);

	if (adjust_right)
		for (int i = area.top(); i < area.bottom(); i+=60+randomOffset(20))
			vertexes_ << new QVector3D(area.right(), qreal(i), qrand() % 1001 * 0.001 * 4.1);

	if (adjust_top)
		for (int i = area.left(); i < area.right(); i+=60+randomOffset(20))
			vertexes_ << new QVector3D(qreal(i), area.top(), qrand() % 1001 * 0.001 * 4.1);

	if (adjust_bottom)
		for (int i = area.left(); i < area.right(); i+=60+randomOffset(20))
			vertexes_ << new QVector3D(qreal(i), area.bottom(), qrand() % 1001 * 0.001 * 4.1);

	// generate vertexes
	generateVertexes(area);

	rect_ = area;

	// build polygons
	polygons_ = std::move(Triangulator::buildMesh(vertexes_));
}

void Mesh::generateBoundVertexes(const QRect &area)
{
	qsrand(uint(&area) + uint(QDateTime::currentMSecsSinceEpoch()));

	auto randomOffset = [] (int limit) {
		return qrand()%limit - limit;
	};

	for (int i = area.left(); i < area.right(); i+=60+randomOffset(20)) {
		vertexes_ << new QVector3D(qreal(i), area.top(), 0.);
		vertexes_ << new QVector3D(qreal(i), area.bottom(), 0.);
	}

	for (int i = area.top(); i < area.bottom(); i+=60+randomOffset(20)) {
		vertexes_ << new QVector3D(area.left(), qreal(i), 0.);
		vertexes_ << new QVector3D(area.right(), qreal(i), 0.);
	}
}

void Mesh::generateVertexes(const QRect &area)
{
	qsrand(uint(&area) + uint(QDateTime::currentMSecsSinceEpoch()));

	const quint32 min_distance = 20;
	const quint32 square = area.width() * area.height();
	const quint32 max_v_count = square / (min_distance * min_distance);
	const qint32 v_count = qMin(quint32(70), max_v_count) - vertexes_.size();

	// current vertexes size >= v_count, no need to generate
	if (v_count <= 0)
		return;

	auto isSuitable = [this, &min_distance] (const QVector3D &p) {
		for (QVector3D *existant: vertexes_) {
			if (qAbs((p - *existant).toPointF().manhattanLength()) < min_distance)
				return false;
		}
		return true;
	};

	for (int i = 0; i < v_count; ++i) {
		QVector3D new_p (area.width() * (qrand() % 1001 / 1000.),
						 area.height() * (qrand() % 1001 / 1000.),
						 qrand() % 1001 * 0.001 * 4.1);
		if (!isSuitable(new_p)) {
			--i;
			continue;
		}
		vertexes_ << new QVector3D(new_p);
	}
}

void Mesh::clearVertexes()
{
	qDeleteAll(vertexes_);
	vertexes_.clear();
}
