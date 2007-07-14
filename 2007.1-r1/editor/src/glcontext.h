#ifndef FIFE_GLCONTEXT_H
#define FIFE_GLCONTEXT_H

#include <../QtOpenGL/QGLWidget>

class glContext : public QGLWidget
{
public:

	glContext(QWidget* parent = 0);
	~glContext();

protected:

	void initializeGL();
	void paintGL();
};

#endif
