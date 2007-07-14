#include <QtGui>
#include <QtOpenGL>

#include "glcontext.h"

glContext::glContext(QWidget *parent)
	: QGLWidget(parent)
{ }

glContext::~glContext()
{
	makeCurrent();
}

void glContext::initializeGL()
{
	qglClearColor(QColor::fromCmykF(0.40, 0.0, 1.0, 0.0));
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
}

void glContext::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}
