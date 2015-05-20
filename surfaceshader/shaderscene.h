#ifndef SHADERSCENE_H
#define SHADERSCENE_H

#include <QObject>

class QPaintDevice;
class Surface;
class Light;
class Distortion;

class ShaderScene : public QObject
{
	Q_OBJECT
public:
	explicit ShaderScene(QObject *parent = 0);
	~ShaderScene();

	QSize size() const { return surface_.size(); }

	void beginBuildScene();
	void endBuildScene();

	ShaderScene &add(const Light &l, bool mouse_binded = false);
	ShaderScene &add(const Distortion &d, bool mouse_binded = false);
	ShaderScene &add(Surface s);

	void render(QPaintDevice *context);

signals:
	void invalidated();

private:
	Surface surface_;
	QList<Light> lights_;
	QList<Distortion> distortions_;

	bool lock_updates_;
};

#endif // SHADERSCENE_H
