#include "appwindow.h"

#include <memory>
#include <QtCore/QSet>
#include <QtGui/QPainter>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

AppWindow::AppWindow(QWidget *parent)
	: QWidget(parent)
{}

AppWindow::~AppWindow()
{}

void AppWindow::reset()
{
	QSet<QPointF> points;

	qsrand(uint(&points) + uint(QDateTime::currentMSecsSinceEpoch()));

	qDebug() << width() << height();

	const QSize bound (width() * 1.1, height() * 1.1);

	auto randomOffset = [] (int limit) {
		return qrand()%limit - limit;
	};

	for (int i = 0; i < width(); i+=40+randomOffset(20)) {
		points << QPointF(qreal(i), 1.);
		points << QPointF(qreal(i), height() -1.);
	}

	for (int i = 0; i < height(); i+=40+randomOffset(20)) {
		points << QPointF(1., qreal(i));
		points << QPointF(width() - 1., qreal(i));
	}

	points << QPointF(width(), height());

	for (int i = 0; i < 1000; ++i)
		points << QPointF(bound.width() * (qrand() % 1001 / 1000.), bound.height() * (qrand() % 1001 / 1000.));
//	points << QPointF(50., 50.) << QPointF(300., 50.) << QPointF(300., 400.) << QPointF(50., 400.);

//	qDebug() << "Generated vertexes:" << points;

	triangles_ = std::move(Triangulator::triangulate(points));

	update();
}

void AppWindow::paintEvent(QPaintEvent *e)
{
	QPainter p (this);
	p.setRenderHint(QPainter::Antialiasing);
	p.fillRect(rect(), QColor(40, 40, 40));

	const QColor base_clr ("#333333");
	auto generateColor = [&base_clr] () {
		const qreal base_v = base_clr.valueF();
		qreal v = base_v + (qrand()%200/200.-0.5) * 0.15;
		return QColor::fromHsvF(base_clr.hueF(), /*base_clr.saturationF()*/0, v);
	};


	for (const TriangleF &triangle: triangles_) {
		QPolygonF poly;
		poly << triangle.v1 << triangle.v2 << triangle.v3;
		p.setBrush(generateColor());
		p.setPen(p.brush().color());
		p.drawPolygon(poly);
	}
}

void AppWindow::mousePressEvent(QMouseEvent *e)
{
	reset();
}
