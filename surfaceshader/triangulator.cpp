#include "triangulator.h"

#include <QtCore/QQueue>
#include <QtCore/QSet>
#include <QtCore/QLineF>
#include <QtCore/QtMath>
#include <QtGui/QVector3D>
#include <QtCore/QDebug>
#include "triangle.h"

class Edge
{
public:
	Edge() {}
	Edge(const QVector3D *v1, const QVector3D *v2) : v1_(v1), v2_(v2) {}
	const QVector3D &v1() const { return *v1_; }
	const QVector3D &v2() const { return *v2_; }
	const QVector3D *v1_;
	const QVector3D *v2_;
};

inline uint qHash(const QPointF &key)
{
	return qHash(static_cast <qint64>(key.x()) << 32 | static_cast<quint64>(key.y()));
}

bool vertexSortLess(const QVector3D *lhs, const QVector3D *rhs)
{
	return lhs->x() < rhs->x();
}

bool Triangulator::analyseCircumcircle(const QVector3D &p, const QVector3D &p1, const QVector3D &p2, const QVector3D &p3, QVector3D &pc, qreal &radius)
{
	const qreal threshold = 0.00001;

	if (qAbs(p1.y() - p2.y()) < threshold
		&& qAbs(p2.y() - p3.y()) < threshold)
		return false;

	auto m = [] (const QVector3D &p1, const QVector3D &p2) -> qreal {
		return -(p2.x() - p1.x()) / (p2.y() - p1.y());
	};

	auto mx = [] (const QVector3D &p1, const QVector3D &p2) -> qreal {
		return (p2.x() + p1.x()) / 2.;
	};

	auto my = [] (const QVector3D &p1, const QVector3D &p2) -> qreal {
		return (p2.y() + p1.y()) / 2.;
	};

	const qreal d12 = qAbs(p2.y() - p1.y());
	const qreal d23 = qAbs(p3.y() - p2.y());
	if (d12 < threshold) {
		const qreal m2 = m(p2, p3);
		const qreal mx2 = mx(p2, p3);
		const qreal my2 = my(p2, p3);
		pc.setX(mx(p1, p2));
		pc.setY(m2 * (pc.x() - mx2) + my2);
	} else if (d23 < threshold) {
		const qreal m1 = m(p1, p2);
		const qreal mx1 = mx(p1, p2);
		const qreal my1 = my(p1, p2);
		pc.setX(mx(p2, p3));
		pc.setY(m1 * (pc.x() - mx1) + my1);
	} else {
		const qreal m1 = m(p1, p2);
		const qreal m2 = m(p2, p3);
		const qreal mx1 = mx(p1, p2);
		const qreal mx2 = mx(p2, p3);
		const qreal my1 = my(p1, p2);
		const qreal my2 = my(p2, p3);
		pc.setX((m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2));
		pc.setY(m1 * (pc.x() - mx1) + my1);
	}

	auto sqrLength = [] (const QVector3D &p1, const QVector3D &p2) {
		const qreal dx = p1.x() - p2.x();
		const qreal dy = p1.y() - p2.y();
		return dx * dx + dy * dy;
	};

	qreal radius_sqr = sqrLength(p2, pc);
	radius = qSqrt(radius_sqr);
	qreal dr_sqr = sqrLength(p, pc);
	return dr_sqr - radius_sqr <= threshold;
}

QList<Triangle> Triangulator::buildMesh(const QList<QVector3D*> &vertexes)
{
	QList<Triangle> triangles;

	if (vertexes.isEmpty())
		return triangles;

	QList<bool> complete_flags = {false};					// false for super triangle
	QList<QVector3D*> process_points (vertexes);			// huh..

	qSort(process_points.begin(), process_points.end(), &vertexSortLess);

	qreal xmin = process_points.first()->x();
	qreal ymin = process_points.first()->y();
	qreal xmax = xmin;
	qreal ymax = ymin;
	for (int i = 1; i < process_points.count(); ++i) {
		const QVector3D *p = process_points.at(i);
		if (p->x() < xmin)
			xmin = p->x();
		if (p->x() > xmax)
			xmax = p->x();
		if (p->y() < ymin)
			ymin = p->y();
		if (p->y() > ymax)
			ymax = p->y();
	}

	qreal dx = xmax - xmin;
	qreal dy = ymax - ymin;
	qreal dmax = qMax(dx, dy);
	qreal xmid = xmin + dx * 0.5;
	qreal ymid = ymin + dy * 0.5;

	Triangle super_triangle;

	QVector3D v1 (xmid - 20 * dmax, ymid - dmax, 0.);
	QVector3D v2 (xmid, ymid + 20 * dmax, 0.);
	QVector3D v3 (xmid + 20 * dmax, ymid - dmax, 0.);

	process_points.append(&v1);
	process_points.append(&v2);
	process_points.append(&v3);

	super_triangle.v1 = &v1;
	super_triangle.v2 = &v2;
	super_triangle.v3 = &v3;
	triangles.push_front(super_triangle);

	for (int i = 0; i < vertexes.count(); ++i) {
		const QVector3D *p (process_points.at(i));

		QList<Edge> edges;

		for (int j = 0; j < triangles.count(); ++j) {
			if (complete_flags.at(j))
				continue;

			qreal r = 0;
			QVector3D pc;
			Triangle t = triangles.at(j);
			const bool is_inside = analyseCircumcircle(*p, *t.v1, *t.v2, *t.v3, pc, r);

			if (pc.x() + r + 0.000001 < p->x())
				complete_flags[j] = true;

			if (is_inside) {
				edges.append(Edge(t.v1, t.v2));
				edges.append(Edge(t.v2, t.v3));
				edges.append(Edge(t.v3, t.v1));

				triangles.swap(j, triangles.count() - 1);
				triangles.removeLast();
				complete_flags.swap(j, complete_flags.count() - 1);
				complete_flags.removeLast();
				--j;
			}
		}

		for (int j = 0; j < edges.count() - 1; ++j) {
			QQueue<int> remove_queue;
			bool remove_j = false;
			for (int k = j + 1; k < edges.count(); ++k) {
				if (((edges.at(j).v1() == edges.at(k).v2()) && (edges.at(j).v2() == edges.at(k).v1()))
					|| ((edges.at(j).v1() == edges.at(k).v1()) && (edges.at(j).v2() == edges.at(k).v2()))) {
					remove_j = true;
					remove_queue << k;
				}
			}
			if (remove_j) {
				remove_queue << j;
				--j;
			}
			while (!remove_queue.isEmpty())
				edges.removeAt(remove_queue.dequeue());
		}

		for (int j = 0; j < edges.count(); ++j) {
			triangles.append(Triangle(edges.at(j).v1_, edges.at(j).v2_, p));
			complete_flags.append(false);
		}
	}

	QSet<const QVector3D*> super_points;
	super_points << super_triangle.v1 << super_triangle.v2 << super_triangle.v3;
	for (int i = 0; i < triangles.count(); ++i) {
		const Triangle &t = triangles.at(i);
		if (super_points.contains(t.v1) || super_points.contains(t.v2) || super_points.contains(t.v3)) {
			triangles.removeAt(i);
			--i;
		}
	}

	return triangles;
}
