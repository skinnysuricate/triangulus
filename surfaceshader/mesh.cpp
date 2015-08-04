#include "mesh.h"

#include <utility>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtGui/QRegion>
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
	const bool adjust_left =	true;//area.left() != rect_.left();
	const bool adjust_top =		true;//area.top() != rect_.top();
	const bool adjust_right =	true;//area.right() != rect_.right();
	const bool adjust_bottom =	true;//area.bottom() != rect_.bottom();

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

	fix(area);

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
		for (int i = area.left(); i < area.right(); i+=30+randomOffset(20))
			vertexes_ << new QVector3D(qreal(i), area.top(), qrand() % 1001 * 0.001 * 4.1);

	if (adjust_bottom)
		for (int i = area.left(); i < area.right(); i+=30+randomOffset(20))
			vertexes_ << new QVector3D(qreal(i), area.bottom(), qrand() % 1001 * 0.001 * 4.1);

	rect_ = area;

	// build polygons
	polygons_ = std::move(Triangulator::buildMesh(vertexes_));
}

#ifdef UNUSED
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
	const qint32 v_count = qMin(quint32(270), max_v_count) - vertexes_.size();

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

#endif

QList<QVector3D*> Mesh::generateVertexesVoronoi(const QRect &area, qreal density) const
{
	qsrand(uint(QDateTime::currentMSecsSinceEpoch()));

	const int point_count = density * (area.width() / 100.) * (area.height() / 100.);

	QList<QVector3D*> vertexes;
	for (int i = 0; i < point_count; ++i)
		vertexes.append(new QVector3D(area.left() + area.width() * (qrand() % 1001 / 1000.),
									   area.top() + area.height() * (qrand() % 1001 / 1000.),
									   qrand() % 1001 * 0.001 * 4.));

	QList<QPoint> density_points;
	for (int i = 0; i < point_count * 50; ++i) {
		density_points << QPoint(area.left() + area.width() * (qrand() % 1001 / 1000.),
								 area.top() + area.height() * (qrand() % 1001 / 1000.));
	}

	for (int n = 0; n < 20; ++n) {
		// 30 iterations
		QHash<QVector3D*, QVector<QPoint>> clusters;
		for (int i = 0; i < density_points.size(); ++i) {
			const QPoint p (density_points.at(i));
			QVector3D *closest = vertexes.first();
			for (int j = 1; j < point_count; ++j) {
				QVector3D *v = vertexes.at(j);
				const quint32 curr_dist = qAbs((closest->x() - p.x()) * (closest->y() - p.y()));
				const quint32 dist = qAbs((v->x() - p.x()) * (v->y() - p.y()));
				if (dist < curr_dist) closest = v;
			}
			clusters[closest].append(p);
		}

		for (int i = 0; i < point_count; ++i) {
			QVector3D *v = vertexes.at(i);
			QVector<QPoint> points (clusters.value(v));
			if (points.empty())
				continue;

			QPoint centroid;
			for (const QPoint &p: points)
				centroid += p;
			centroid = centroid / points.size();
			v->setX(centroid.x());
			v->setY(centroid.y());
		}
	}

	for (int i = 0; i < vertexes.size(); ++i)
		for (int j = i + 1; j < vertexes.size() - 1; ++j) {
			if ((*vertexes.at(i) - *vertexes.at(j)).toPointF().manhattanLength() < 20) {
				delete vertexes.at(j);
				vertexes.removeAt(j);
				--j;
			}
		}

	return vertexes;
}

QList<QVector3D*> Mesh::generateVertexes(const QRect &area, uint count) const
{
	qsrand(uint(QDateTime::currentMSecsSinceEpoch()));

	QList<QVector3D*> vertexes;
	for (uint i = 0; i < count; ++i)
		vertexes.append(new QVector3D(area.left() + area.width() * (qrand() % 1001 / 1000.),
									   area.top() + area.height() * (qrand() % 1001 / 1000.),
									   qrand() % 1001 * 0.001 * 6.));
	return vertexes;
}

QList<QPoint> Mesh::generateDensityPoints(const QRect &area, uint count) const
{
	qsrand(uint(QDateTime::currentMSecsSinceEpoch()));

	QList<QPoint> density_points;
	for (uint i = 0; i < count; ++i) {
		density_points << QPoint(area.left() + area.width() * (qrand() % 1001 / 1000.),
								 area.top() + area.height() * (qrand() % 1001 / 1000.));
	}

	return density_points;
}

void Mesh::performCentroidalTesselation(const QList<QPoint> &density_points, QList<QVector3D *> &vertexes, uint iterations) const
{
	const int point_count = vertexes.size();

	for (uint n = 0; n < iterations; ++n) {
		QHash<QVector3D*, QVector<QPoint>> clusters;
		for (int i = 0; i < density_points.size(); ++i) {
			const QPoint p (density_points.at(i));
			QVector3D *closest = vertexes.first();
			for (int j = 1; j < point_count; ++j) {
				QVector3D *v = vertexes.at(j);
				const quint32 curr_dist = qAbs((closest->x() - p.x()) * (closest->y() - p.y()));
				const quint32 dist = qAbs((v->x() - p.x()) * (v->y() - p.y()));
				if (dist < curr_dist) closest = v;
			}
			clusters[closest].append(p);
		}

		for (int i = 0; i < point_count; ++i) {
			QVector3D *v = vertexes.at(i);
			QVector<QPoint> points (clusters.value(v));
			if (points.empty())
				continue;

			QPoint centroid;
			for (const QPoint &p: points)
				centroid += p;
			centroid = centroid / points.size();
			v->setX(centroid.x());
			v->setY(centroid.y());
		}
	}
}

void Mesh::completeVertexes(QVector<QRect> cells)
{
	// group all vertexes by cells
	QHash<int, QList<QVector3D*>> grouped_by_cells;
	QList<QVector3D*> current = vertexes_;
	while (!current.empty()) {
		QVector3D *v = current.takeFirst();
		for (int i = 0; i < cells.size(); ++i)
			if (cells.at(i).contains(v->x(), v->y()))
				grouped_by_cells[i].append(v);
	}

	// complete every group individually
	for (int i = 0; i < cells.size(); ++i) {
		QList<QVector3D*> vertexes (grouped_by_cells.value(i));
		const QRect cell (cells.at(i));
		uint expected_count = density_ * cell.width() / 20. * cell.height() / 20.;
		uint existent_count = vertexes.size();
		// - at least one point for cell
		if (!expected_count)
			++expected_count;

		if (existent_count >= expected_count)
			continue;

		vertexes.append(generateVertexes(cell, expected_count - existent_count));

		QList<QPoint> density_points = generateDensityPoints(cell, expected_count * 60);
		performCentroidalTesselation(std::move(density_points), vertexes, 30);

		for (uint j = existent_count; j < expected_count; ++j)
			vertexes_.append(vertexes.at(j));
	}

	// fix up close points
	for (int i = 0; i < vertexes_.size(); ++i)
		for (int j = i + 1; j < vertexes_.size() - 1; ++j) {
			if ((*vertexes_.at(i) - *vertexes_.at(j)).toPointF().manhattanLength() < 20) {
				delete vertexes_.at(j);
				vertexes_.removeAt(j);
				--j;
			}
		}
}

void Mesh::fix(const QRect &area)
{
	const QRegion new_region (area);

	if (!new_region.contains(rect_))
		return;

	const int cell_width = area.width() / 1.;
	const int cell_height = area.width() / 6;

	QVector<QRect> cells;

	for (int x = area.left(); x < area.width(); x+=cell_width)
		for (int y = area.top(); y < area.height(); y+=cell_height) {
			cells.append( {x, y, cell_width, cell_height} );
			qDebug() << cells.last().x() << cells.last().y() << cells.last().width() << cells.last().height();
		}

	completeVertexes(std::move(cells));
}

void Mesh::clearVertexes()
{
	qDeleteAll(vertexes_);
	vertexes_.clear();
}
