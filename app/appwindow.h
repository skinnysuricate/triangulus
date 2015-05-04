#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QWidget>
#include "triangulator.h"

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
	QList<TriangleF> triangles_;
};

#endif // APPWINDOW_H
