#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <memory>
#include <QWidget>
#include <QMap>
#include "triangulator.h"

class QVariantAnimation;

class AppWindow : public QWidget
{
	Q_OBJECT

public:
	AppWindow(QWidget *parent = 0);
	~AppWindow();
	void reset();

protected:
	void paintEvent(QPaintEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

private:
	void generateColors();
	void recountAntigravityForces(const QVector3D &particle_pos);

	QMap<const QVector3D*, QVector3D> deltas_;
	QList<QVector3D*> points_;
	QList<LinkedTriangle> polygons_;
	QList<QColor> polygon_colors_;
	std::unique_ptr<QVariantAnimation> animator_;
	qreal progress_ = 1.;
	int hovered_idx_ = -1;
};

#endif // APPWINDOW_H
