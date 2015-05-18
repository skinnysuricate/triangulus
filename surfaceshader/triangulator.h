#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H

#include "mesh.h"

template <class T>
class QList;
class QVector3D;

class Triangulator
{
public:
	static Mesh buildMesh(const QList<QVector3D *> &vertexes);

private:
	static bool analyseCircumcircle(const QVector3D &p,
									const QVector3D &p1, const QVector3D &p2, const QVector3D &p3,
									QVector3D &pc, qreal &radius);
};

#endif // TRIANGULATOR_H
