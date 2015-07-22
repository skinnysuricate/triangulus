#include "canvaswidget.h"

#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>
#include "shaderscene.h"
#include "material.h"

CanvasWidget::CanvasWidget(QWidget *parent)
	: QWidget(parent)
{
	setMouseTracking(true);
	buildBlueScene();
}

CanvasWidget::~CanvasWidget()
{}

void CanvasWidget::paintEvent(QPaintEvent *e)
{
	scene_->render(this);
	e->accept();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *e)
{
	scene_->beginBuildScene();
	for (const quint64 &id: binded_ids_)
		scene_->move(id, e->pos());
	scene_->endBuildScene();
	e->accept();
}

void CanvasWidget::mousePressEvent(QMouseEvent *e)
{
	e->accept();
}

void CanvasWidget::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	buildDefaultScene();//BlueScene();
}

void CanvasWidget::buildDefaultScene()
{
	Material m (QColor("#555555"), QColor("#444444"));
	scene_.reset(new ShaderScene(QSize(width(), height()), m, 5.));
	scene_->beginBuildScene();
	std::unique_ptr<Cluster> cluster (new Cluster);
	*cluster << scene_->add(Light(QVector3D(0, 0, 100), "#FFDDC2", "#333", 0.02, cluster.get()))
			<< scene_->add(Light(QVector3D(0, 0, 400), "#4400EE", "#555", 0.07, cluster.get()))
			<< scene_->add(Light(QVector3D(0, 0, 50), "#EEEE00", "#444", 0.01, cluster.get()))
			<< scene_->add(Distortion(QVector3D(), cluster.get()));
	binded_ids_ << scene_->add(std::move(cluster));
	scene_->endBuildScene();
	connect(scene_.get(), SIGNAL(invalidated()), this, SLOT(update()));
}

void CanvasWidget::buildBlueScene()
{
	Material m (QColor("#2E5A86"), QColor("#335f8a"));
	scene_.reset(new ShaderScene(QSize(width(), height()), m, 5.));
	scene_->beginBuildScene();

	std::unique_ptr<Cluster> cluster (new Cluster);
	*cluster << scene_->add(Light(QVector3D(0, 0, 100), "#FFF", "#333", 0.02, cluster.get()))
			<< scene_->add(Light(QVector3D(0, 0, 200), "#808080", "#777", 0.01, cluster.get()))
			<< scene_->add(Light(QVector3D(0, 0, 50), "#CCC", "#444", 0.005, cluster.get()))
			<< scene_->add(Distortion(QVector3D(), cluster.get()));
	binded_ids_ << scene_->add(std::move(cluster));
	scene_->endBuildScene();
	connect(scene_.get(), SIGNAL(invalidated()), this, SLOT(update()));
}

