#include "cluster.h"

Cluster::Cluster(const QVector3D &v)
	: v_(v)
{}

Cluster::Cluster(const Cluster &other)
	: v_(other.v_)
	, ids_(other.ids_)
{}

Cluster::Cluster(Cluster &&other)
	: v_(other.v_)
	, ids_(std::move(other.ids_))
{
	other.v_ = QVector3D();
}

Cluster &Cluster::operator =(const Cluster &other)
{
	v_ = other.v_;
	ids_ = other.ids_;
	return *this;
}

Cluster &Cluster::operator =(Cluster &&other)
{
	v_ = other.v_;
	ids_ = std::move(other.ids_);
	other.v_ = QVector3D();
	return *this;
}

void Cluster::insert(quint64 id)
{
	ids_.append(id);
}

Cluster &Cluster::operator <<(quint64 id)
{
	insert(id);
	return *this;
}

void Cluster::setPosition(const QVector3D &v)
{
	v_ = v;
}

