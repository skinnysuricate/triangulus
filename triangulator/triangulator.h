#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H

#include <QtCore/QList>
#include <QtCore/QPointF>
#include <QtCore/QHash>

class QSet<int>;
class QPointF;
class TriangleF;

class Triangulator
{

public:
	Triangulator();

	static bool analyseCircumcircle(const QPointF &p,
									const QPointF &p1, const QPointF &p2, const QPointF &p3,
									QPointF &pc, qreal &radius);
	static QList<TriangleF> triangulate(const QSet<QPointF> &points);
};

class TriangleF
{
public:
	TriangleF() {}
	TriangleF(const QPointF &p1, const QPointF &p2, const QPointF &p3);
	QPointF v1;
	QPointF v2;
	QPointF v3;
};

inline uint qHash(const QPointF &key)
{
	return qHash(static_cast <qint64>(key.x()) << 32 | static_cast<quint64>(key.y()));
}

#ifdef UNUSED
#include <iostream>
#include <stdlib.h> // for C qsort
#include <cmath>
#include <time.h> // for random

const int MaxVertices = 500;
const int MaxTriangles = 1000;
const int n_MaxPoints = 10; // for the test programm
const double EPSILON = 0.000001;

struct ITRIANGLE{
  int p1, p2, p3;
};

struct IEDGE{
  int p1, p2;
};

struct XYZ{
  double x, y, z;
};

int XYZCompare(const void *v1, const void *v2);
int Triangulate(int nv, XYZ pxyz[], ITRIANGLE v[], int &ntri);
int CircumCircle(double, double, double, double, double, double, double,
double, double&, double&, double&);
#endif

#endif // TRIANGULATOR_H
