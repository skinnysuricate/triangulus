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
	scene_->handleMouseMove(e->pos());
	e->accept();
}

void CanvasWidget::mousePressEvent(QMouseEvent *e)
{
	e->accept();
}

void CanvasWidget::buildDefaultScene()
{
	Material m (QColor("#555555"), QColor("#444444"));
	scene_.reset(new ShaderScene(QSize(500, 500), m, 5.));
	scene_->beginBuildScene();
	scene_->add(Light(QVector3D(200, 100, 100), "#FFDDC2", "#333", 0.02), true)
			.add(Light(QVector3D(50, 150, 400), "#4400EE", "#555", 0.07), true)
			.add(Light(QVector3D(50, 150, 50), "#EEEE00", "#444", 0.01), true)
			.add(Distortion(), true);
	scene_->endBuildScene();
	connect(scene_.get(), SIGNAL(invalidated()), this, SLOT(update()));
}

void CanvasWidget::buildBlueScene()
{
	Material m (QColor("#2E5A86"), QColor("#335f8a"));
	scene_.reset(new ShaderScene(QSize(500, 500), m, 5.));
	scene_->beginBuildScene();
	scene_->add(Light(QVector3D(200, 100, 50), "#fff", "#333", 0.02), true)
			.add(Light(QVector3D(50, 150, 200), "#808080", "#555", 0.01), true)
			.add(Light(QVector3D(50, 150, 25), "#C4E2FF", "#444", 0.001), true)
			.add(Distortion(), true)
			;
	scene_->endBuildScene();
	connect(scene_.get(), SIGNAL(invalidated()), this, SLOT(update()));
}

