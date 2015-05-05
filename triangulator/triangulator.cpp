#include "triangulator.h"

#include <QtCore/QQueue>
#include <QtCore/QSet>
#include <QtCore/QLineF>
#include <QtCore/QtMath>
#include <QDebug>

bool pointSortLess(const QPointF &lhs, const QPointF &rhs)
{
	return lhs.x() < rhs.x();
}

bool pointPointersSortLess(const QPointF *lhs, const QPointF *rhs)
{
	return lhs->x() < rhs->x();
}

Triangulator::Triangulator()
{}

bool Triangulator::analyseCircumcircle(const QPointF &p, const QPointF &p1, const QPointF &p2, const QPointF &p3, QPointF &pc, qreal &radius)
{
	const qreal threshold = 0.00001;

	if (qAbs(p1.y() - p2.y()) < threshold
		&& qAbs(p2.y() - p3.y()) < threshold)
		return false;

	auto m = [] (const QPointF &p1, const QPointF &p2) -> qreal {
		return -(p2.x() - p1.x()) / (p2.y() - p1.y());
	};

	auto mx = [] (const QPointF &p1, const QPointF &p2) -> qreal {
		return (p2.x() + p1.x()) / 2.;
	};

	auto my = [] (const QPointF &p1, const QPointF &p2) -> qreal {
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
//		pc.setY((d12 > d23)
//				? m1 * (pc.x() - mx1) + my1
//				: m2 * (pc.x() - mx2) + my2);
	}

	auto sqrLength = [] (const QPointF &p1, const QPointF &p2) {
		const qreal dx = p1.x() - p2.x();
		const qreal dy = p1.y() - p2.y();
		return dx * dx + dy * dy;
	};

	qreal radius_sqr = sqrLength(p2, pc);
	radius = qSqrt(radius_sqr);
	qreal dr_sqr = sqrLength(p, pc);
	return dr_sqr - radius_sqr <= threshold;
}

QList<Triangle> Triangulator::triangulate(const QSet<QPointF> &points)
{
	QList<Triangle> triangles;

	if (points.isEmpty())
		return triangles;

	QList<bool> complete_flags = {false};					// false for super triangle
	QList<QPointF> process_points (points.toList());		// huh..

	qSort(process_points.begin(), process_points.end(), &pointSortLess);

//	qDebug() << "Vertexes sorted by x:" << process_points;

	// Find the maximum and minimum vertex bounds.
	// This is to allow calculation of the bounding triangle

	qreal xmin = process_points.first().x();
	qreal ymin = process_points.first().y();
	qreal xmax = xmin;
	qreal ymax = ymin;
	for (int i = 1; i < process_points.count(); ++i) {
		const QPointF &p = process_points.at(i);
		if (p.x() < xmin)
			xmin = p.x();
		if (p.x() > xmax)
			xmax = p.x();
		if (p.y() < ymin)
			ymin = p.y();
		if (p.y() > ymax)
			ymax = p.y();
	}

	qreal dx = xmax - xmin;
	qreal dy = ymax - ymin;
	qreal dmax = qMax(dx, dy);
	qreal xmid = xmin + dx * 0.5;
	qreal ymid = ymin + dy * 0.5;
	/*
	 Set up the supertriangle
	 his is a triangle which encompasses all the sample points.
	 The supertriangle coordinates are added to the end of the
	 vertex list. The supertriangle is the first triangle in
	 the triangle list.
	*/
	Triangle super_triangle;

	QPointF v1 (xmid - 20 * dmax, ymid - dmax);
	QPointF v2 (xmid, ymid + 20 * dmax);
	QPointF v3 (xmid + 20 * dmax, ymid - dmax);

	process_points.append(v1);
	process_points.append(v2);
	process_points.append(v3);

//	pxyz[nv+0].x = xmid - 20 * dmax;
//	pxyz[nv+0].y = ymid - dmax;
//	pxyz[nv+1].x = xmid;
//	pxyz[nv+1].y = ymid + 20 * dmax;
//	pxyz[nv+2].x = xmid + 20 * dmax;
//	pxyz[nv+2].y = ymid - dmax;

	super_triangle.v1 = v1;
	super_triangle.v2 = v2;
	super_triangle.v3 = v3;
	triangles.push_front(super_triangle);

//	v[0].p1 = nv;
//	v[0].p2 = nv+1;
//	v[0].p3 = nv+2;
//	complete[0] = false;
//	ntri = 1;

//	QList<TriangleF> ready_triangles;
	/*
	 Include each point one at a time into the existing mesh
	*/
	for (int i = 0; i < points.count(); ++i) {
		QPointF p (process_points.at(i));

		// Set up the edge buffer.
		// If the point (xp,yp) lies inside the circumcircle then the
		// three edges of that triangle are added to the edge buffer
		// and that triangle is removed.
		QList<QLineF> edges;

		for (int j = 0; j < triangles.count(); ++j) {
			if (complete_flags.at(j))
				continue;

			qreal r = 0;
			QPointF pc;
			Triangle t = triangles.at(j);
			const bool is_inside = analyseCircumcircle(p, t.v1, t.v2, t.v3, pc, r);

//			qDebug() << "Is Inside:" << is_inside << p;

//			if (pc.x() < p.x() && qPow(p.x() - pc.x(), 2.) > r)
			if (pc.x() + r + 0.000001 < p.x())
				complete_flags[j] = true;

			if (is_inside) {
				edges.append(QLineF(t.v1, t.v2));
				edges.append(QLineF(t.v2, t.v3));
				edges.append(QLineF(t.v3, t.v1));

				triangles.swap(j, triangles.count() - 1);
				triangles.removeLast();
				complete_flags.swap(j, complete_flags.count() - 1);
				complete_flags.removeLast();
				--j;
			}
		}

		// Tag multiple edges
		// Note: if all triangles are specified anticlockwise then all
		// interior edges are opposite pointing in direction.

//		qDebug() << "Edges Before" << edges.count();
		for (int j = 0; j < edges.count() - 1; ++j) {
			QQueue<int> remove_queue;
			bool removeJ = false;
			for (int k = j + 1; k < edges.count(); ++k) {
				if (((edges.at(j).p1() == edges.at(k).p2()) && (edges.at(j).p2() == edges.at(k).p1()))
					|| ((edges.at(j).p1() == edges.at(k).p1()) && (edges.at(j).p2() == edges.at(k).p2()))) {
					removeJ = true;
					remove_queue << k;
				}
			}
			if (removeJ) {
				remove_queue << j;
				--j;
			}
			while (!remove_queue.isEmpty())
				edges.removeAt(remove_queue.dequeue());
		}
//		qDebug() << "Edges After" << edges.count();

//		for (const QLineF &edge: edges)
//			qDebug() << edge;

		// Form new triangles for the current point
		// All edges are arranged in clockwise order.

		for (int j = 0; j < edges.count(); ++j) {
			triangles.append(Triangle(edges.at(j).p1(), edges.at(j).p2(), p));
			complete_flags.append(false);
		}
	}
	// Remove triangles with supertriangle vertices
	// These are triangles which have a vertex number greater than nv


	QSet<QPointF> super_points;
	super_points << super_triangle.v1 << super_triangle.v2 << super_triangle.v3;
//	qDebug() << "Triangles Before" << triangles.count();
	for (int i = 0; i < triangles.count(); ++i) {
		const Triangle &t = triangles.at(i);
		if (super_points.contains(t.v1) || super_points.contains(t.v2) || super_points.contains(t.v3)) {
			triangles.removeAt(i);
			--i;
		}
	}
//	qDebug() << "Triangles After" << triangles.count();

	return triangles;
}

QList<LinkedTriangle> Triangulator::triangulatePersistant(const QList<QPointF*> &persistant_points)
{
	QList<LinkedTriangle> triangles;

	if (persistant_points.isEmpty())
		return triangles;

	QList<bool> complete_flags = {false};					// false for super triangle
	QList<QPointF*> process_points (persistant_points);		// huh..

	qSort(process_points.begin(), process_points.end(), &pointPointersSortLess);

	qreal xmin = process_points.first()->x();
	qreal ymin = process_points.first()->y();
	qreal xmax = xmin;
	qreal ymax = ymin;
	for (int i = 1; i < process_points.count(); ++i) {
		const QPointF *p = process_points.at(i);
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

	LinkedTriangle super_triangle;

	QPointF v1 (xmid - 20 * dmax, ymid - dmax);
	QPointF v2 (xmid, ymid + 20 * dmax);
	QPointF v3 (xmid + 20 * dmax, ymid - dmax);

	process_points.append(&v1);
	process_points.append(&v2);
	process_points.append(&v3);

	super_triangle.v1 = &v1;
	super_triangle.v2 = &v2;
	super_triangle.v3 = &v3;
	triangles.push_front(super_triangle);

	for (int i = 0; i < persistant_points.count(); ++i) {
		const QPointF *p (process_points.at(i));

		QList<LinkedEdge> edges;

		for (int j = 0; j < triangles.count(); ++j) {
			if (complete_flags.at(j))
				continue;

			qreal r = 0;
			QPointF pc;
			LinkedTriangle t = triangles.at(j);
			const bool is_inside = analyseCircumcircle(*p, *t.v1, *t.v2, *t.v3, pc, r);

			if (pc.x() + r + 0.000001 < p->x())
				complete_flags[j] = true;

			if (is_inside) {
				edges.append(LinkedEdge(t.v1, t.v2));
				edges.append(LinkedEdge(t.v2, t.v3));
				edges.append(LinkedEdge(t.v3, t.v1));

				triangles.swap(j, triangles.count() - 1);
				triangles.removeLast();
				complete_flags.swap(j, complete_flags.count() - 1);
				complete_flags.removeLast();
				--j;
			}
		}

		for (int j = 0; j < edges.count() - 1; ++j) {
			QQueue<int> remove_queue;
			bool removeJ = false;
			for (int k = j + 1; k < edges.count(); ++k) {
				if (((edges.at(j).p1() == edges.at(k).p2()) && (edges.at(j).p2() == edges.at(k).p1()))
					|| ((edges.at(j).p1() == edges.at(k).p1()) && (edges.at(j).p2() == edges.at(k).p2()))) {
					removeJ = true;
					remove_queue << k;
				}
			}
			if (removeJ) {
				remove_queue << j;
				--j;
			}
			while (!remove_queue.isEmpty())
				edges.removeAt(remove_queue.dequeue());
		}

		for (int j = 0; j < edges.count(); ++j) {
			triangles.append(LinkedTriangle(edges.at(j).p1_, edges.at(j).p2_, p));
			complete_flags.append(false);
		}
	}

	QSet<const QPointF*> super_points;
	super_points << super_triangle.v1 << super_triangle.v2 << super_triangle.v3;
	for (int i = 0; i < triangles.count(); ++i) {
		const LinkedTriangle &t = triangles.at(i);
		if (super_points.contains(t.v1) || super_points.contains(t.v2) || super_points.contains(t.v3)) {
			triangles.removeAt(i);
			--i;
		}
	}

	return triangles;
}

Triangle::Triangle(const QPointF &p1, const QPointF &p2, const QPointF &p3)
	: v1(p1)
	, v2(p2)
	, v3(p3)
{}
