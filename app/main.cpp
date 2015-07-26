#include "canvaswidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CanvasWidget w;
	w.resize(700, 700);
	w.show();

	return a.exec();
}
