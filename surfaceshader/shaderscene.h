#ifndef SHADERSCENE_H
#define SHADERSCENE_H

#include <QObject>

class QPainter;
class Surface;
class Light;
class Distortion;

class ShaderScene : public QObject
{
	Q_OBJECT
public:
	explicit ShaderScene(QObject *parent = 0);
	~ShaderScene();

	ShaderScene &add(const Light &l);
	ShaderScene &add(const Distortion &d);
	ShaderScene &add(Surface s);

	void render(QPainter *p);

private:
	Surface surface_;
	QList<Light> lights_;
	QList<Distortion> distortions_;
};

#endif // SHADERSCENE_H
