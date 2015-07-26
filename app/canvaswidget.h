#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <memory>
#include <QWidget>

class ShaderScene;

class CanvasWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CanvasWidget(QWidget *parent = 0);
	~CanvasWidget();

protected:
	void paintEvent(QPaintEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

private:
	void buildDefaultScene();
	void buildBlueScene();
	void buildWierdScene();

	std::unique_ptr<ShaderScene> scene_;
	QList<quint64> binded_ids_;
};

#endif // CANVASWIDGET_H
