#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H

#include <QtCore/QList>
#include <QtCore/QPointF>
#include <QtCore/QHash>

template <class T>
class QSet;
class Triangle;
class QVector3D;
class LinkedTriangle;

class Triangulator
{

public:
	Triangulator();

#ifdef UNUSED
	static QList<Triangle> triangulate(const QSet<QPointF> &points);
#endif
	static QList<LinkedTriangle> triangulatePersistant(const QList<QVector3D *> &persistant_points);

private:
	static bool analyseCircumcircle(const QVector3D &p,
									const QVector3D &p1, const QVector3D &p2, const QVector3D &p3,
									QVector3D &pc, qreal &radius);
};

#ifdef UNUSED
class Triangle
{
public:
	Triangle() {}
	Triangle(const QPointF &p1, const QPointF &p2, const QPointF &p3);
	QPointF v1;
	QPointF v2;
	QPointF v3;
};
#endif

class LinkedTriangle
{
public:
	LinkedTriangle() {}
	LinkedTriangle(const QVector3D *p1, const QVector3D *p2, const QVector3D *p3)
		: v1(p1), v2(p2), v3(p3) {}
	const QVector3D *v1;
	const QVector3D *v2;
	const QVector3D *v3;
};

class LinkedEdge
{
public:
	LinkedEdge() {}
	LinkedEdge(const QVector3D *v1, const QVector3D *v2) : v1_(v1), v2_(v2) {}
	const QVector3D &v1() const { return *v1_; }
	const QVector3D &v2() const { return *v2_; }
	const QVector3D *v1_;
	const QVector3D *v2_;
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
