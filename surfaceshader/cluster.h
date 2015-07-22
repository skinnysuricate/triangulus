#ifndef CLUSTER_H
#define CLUSTER_H

#include <QVector>
#include <QtGui/QVector3D>

class Cluster
{
public:
	explicit Cluster(const QVector3D &v = {});
	Cluster(const Cluster &other);
	Cluster(Cluster &&other);

	Cluster &operator =(const Cluster &other);
	Cluster &operator =(Cluster &&other);

	void insert(quint64 id);
	Cluster &operator <<(quint64 id);

	void setPosition(const QVector3D &v);
	const QVector3D &position() const { return v_; }

	const QVector<quint64> ids() const { return ids_; }

private:
	QVector3D v_;
	QVector<quint64> ids_;
};

#endif // CLUSTER_H
