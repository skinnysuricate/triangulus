#ifndef TRIANGLE_H
#define TRIANGLE_H

class QVector3D;

class Triangle
{
public:
	Triangle() {}
	Triangle(const QVector3D *p1, const QVector3D *p2, const QVector3D *p3)
		: v1(p1), v2(p2), v3(p3) {}
	const QVector3D *v1;
	const QVector3D *v2;
	const QVector3D *v3;
};

#endif // TRIANGLE_H
