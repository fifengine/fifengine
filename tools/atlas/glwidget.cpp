/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#include "glwidget.h"
#include "mainwindow.h"

#include <QMouseEvent>
#include <QMessageBox>

const int timerInterval = 20;

GLWidget::GLWidget(QWidget* parent)
	: QGLWidget(parent), mCam_xpos(-0.5f), mCam_ypos(-0.5f), mCam_zpos(-2.0f),
	texWidth(1), texHeight(1), currentTex(0)
{
	startTimer(timerInterval); //64-65fps
}

GLWidget::~GLWidget()
{
	glDeleteTextures(texId.size(), &texId[0]);
	glDeleteTextures(1, &alphaCheckedTex);
}

void GLWidget::atlasBookChanged(int index)
{
	if(index < texId.size())
	{
		currentTex = index;
		mCam_xpos = -(texWidth[currentTex]/2);
		mCam_ypos = -(texHeight[currentTex]/2);
		mCam_zpos = -texWidth[currentTex];
	}
}

void GLWidget::alphaBlendingChecked(int state)
{
	if(state == Qt::Checked)
	{
		// turn on the alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		// turn it off
		glDisable(GL_BLEND);
	}
}

void GLWidget::initializeGL()
{	
	// just basic set up
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glDisable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	texId.resize(1);
	glGenTextures(1, &texId[0]);
	glEnable(GL_TEXTURE_2D);

	char sqData[32*32*3];
	for(int y = 0; y < 32; ++y)
	{
		for(int x = 0; x < 32*3; x+=3)
		{
			if((y < 16 && x < 16*3) || (y >= 16 && x >= 16*3))
			{
				sqData[x + y*32*3 + 0] = 200;
				sqData[x + y*32*3 + 1] = 200;
				sqData[x + y*32*3 + 2] = 200;
			}
			else
			{
				sqData[x + y*32*3 + 0] = 100;
				sqData[x + y*32*3 + 1] = 100;
				sqData[x + y*32*3 + 2] = 100;
			}
		}
	}

	glGenTextures(1, &alphaCheckedTex);
	glBindTexture(GL_TEXTURE_2D, alphaCheckedTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, sqData);
}

void GLWidget::resizeGL(int width, int height)
{
	if(height == 0)
		height = 1;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//gluOrtho2D(-1,2, -2, 2);
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLWidget::refreshed(QVector<Image> const& tex)
{
	assert(!tex.empty());

	if(texId.size() < tex.size())
	{
		int tmp = texId.size();
		texId.resize(tex.size());
		glGenTextures(tex.size() - tmp, &texId[tmp]);
	}

	texWidth.resize(tex.size());
	texHeight.resize(tex.size());

	for(int i = 0; i < tex.size(); ++i)
	{

		glBindTexture(GL_TEXTURE_2D, texId[i]);

		// BGRA because FreeImage gives you that for every byte formats (different case for floats and so on)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex[i].getWidth(), tex[i].getHeight(),
					 0, 0x80E1 /* GL_BGRA */, GL_UNSIGNED_BYTE, tex[i].getData());

		GLenum q;
		if((q = glGetError())!= GL_NO_ERROR)
		{
			QMessageBox::critical(this, "Critical GL Error", QString::number(q, 16));
			return;
		}

		texWidth[i] = tex[i].getWidth();
		texHeight[i] = tex[i].getHeight();
	}

	currentTex = 0;
	mCam_xpos = -(texWidth[0]/2);
	mCam_ypos = -(texHeight[0]/2);
	mCam_zpos = -qMax(texWidth[0], texHeight[0]);
}

void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(mCam_xpos, mCam_ypos, mCam_zpos);
	glColor3f(1.0f, 1.0f, 1.0f);

	const float texWidthScale = texWidth[currentTex] * 0.03125;
	const float texHeightScale = texHeight[currentTex] * 0.03125;

	glBindTexture(GL_TEXTURE_2D, alphaCheckedTex);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, texHeightScale);
		glVertex3f(0.0f, texHeight[currentTex], 1.0f);

		glTexCoord2f(texWidthScale, texHeightScale);
		glVertex3f(texWidth[currentTex], texHeight[currentTex], 1.0f);

		glTexCoord2f(texWidthScale, 0.0f);
		glVertex3f(texWidth[currentTex], 0.0f, 1.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId[currentTex]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.0f, texHeight[currentTex], 0.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(texWidth[currentTex], texHeight[currentTex], 0.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(texWidth[currentTex], 0.0f, 0.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	mAnchor = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	QPoint diff = event->pos() - mAnchor;

	mCam_xpos += diff.x() * 1.0f;
	mCam_ypos -= diff.y() * 1.0f;

	mAnchor = event->pos();
	updateGL();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
	if(event->delta() > 0)
		mCam_zpos += 50.0f;
	else
		mCam_zpos -= 50.0f;
	updateGL();
}

void GLWidget::timerEvent(QTimerEvent* event)
{
	updateGL();
}
