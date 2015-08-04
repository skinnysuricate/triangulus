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
	scene_->adjustSize(e->size());
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
	Material m (QColor("#3f607f"), QColor("#4580b8"));
	scene_.reset(new ShaderScene(QSize(width(), height()), m, 0.09));
	scene_->beginBuildScene();

	std::unique_ptr<Cluster> cluster (new Cluster);
	*cluster << scene_->add(Light(QVector3D(0, 0, 100), "#ffffff", "#333", 0.02, cluster.get()))
			<< scene_->add(Light(QVector3D(0, 0, 250), "#808080", "#777", 0.001, cluster.get()))
			<< scene_->add(Light(QVector3D(0, 0, 50), "#cecece", "#444", 0.005, cluster.get()))
			<< scene_->add(Distortion(QVector3D(), cluster.get()));
	binded_ids_ << scene_->add(std::move(cluster));
	scene_->endBuildScene();
	connect(scene_.get(), SIGNAL(invalidated()), this, SLOT(update()));
}

void CanvasWidget::buildWierdScene()
{
	Material m (QColor("#777"), QColor("#333"));
	scene_.reset(new ShaderScene(QSize(width(), height()), m, 5.));
	scene_->beginBuildScene();

	std::unique_ptr<Cluster> cluster (new Cluster);
	*cluster << scene_->add(Light(QVector3D(0, -40, 50), "#ff0000", "#777", 0.00001, cluster.get()))
			 << scene_->add(Light(QVector3D(-40, 40, 50), "#00dd00", "#777", 0.00001, cluster.get()))
			 << scene_->add(Light(QVector3D(40, 40, 50), "#0000ff", "#777", 0.00001, cluster.get()))
			 << scene_->add(Distortion(QVector3D(), cluster.get()));
	binded_ids_ << scene_->add(std::move(cluster));
	scene_->endBuildScene();
	connect(scene_.get(), SIGNAL(invalidated()), this, SLOT(update()));
}

