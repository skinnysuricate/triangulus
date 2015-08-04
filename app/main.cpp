#include "canvaswidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CanvasWidget w;
	w.resize(245, 1100);
	w.show();

	return a.exec();
}
