#include "canvaswidget.h"

#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>
#include "shaderscene.h"
#include "material.h"

CanvasWidget::CanvasWidget(QWidget *parent)
	: QWidget(parent)
{
	setMouseTracking(true);
	buildDefaultScene();
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
	Material m (QColor("#555555"), QColor("#222222"));
	scene_.reset(new ShaderScene(QSize(500, 500), m, 5.));
	scene_->beginBuildScene();
	scene_->add(Light(QVector3D(200, 100, 100), "#FFDDC2", "#333", 0.02), true)
			.add(Light(QVector3D(50, 150, 50), "#92F1FF", "#333", 0.03))
			.add(Distortion(), true);
	scene_->endBuildScene();
	connect(scene_.get(), SIGNAL(invalidated()), this, SLOT(update()));
}

