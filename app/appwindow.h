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
private:
	void generateColors();
	void recountDeltas();
	//QList<Triangle> triangles_;
	QMap<const QPointF*, QPointF> deltas_;
	QList<QPointF*> points_;
	QList<LinkedTriangle> polygons_;
	QList<QColor> polygon_colors_;
	std::unique_ptr<QVariantAnimation> animator_;
	qreal progress_;
};

#endif // APPWINDOW_H
