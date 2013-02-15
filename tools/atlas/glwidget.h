/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

#pragma once

#include <QGLWidget>
#include <QImage>
#include "atlas.h"

class MainWindow;

class GLWidget : public QGLWidget
{
	Q_OBJECT
public:
	GLWidget(QWidget* parent);
	virtual ~GLWidget();

	void resizeGL(int width, int height);
	void refreshed(QImage const& tex);
	void updateOccupation(QVector<AtlasBlock> const& regions);
	void highlightedChanged(const QVector<int>& vec);

private slots:
	void alphaBlendingChecked(int state);
	void showOccupationChecked(int state);

protected:
	void initializeGL();
	void paintGL();

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void timerEvent(QTimerEvent* event);

private:
	float mCam_xpos;
	float mCam_ypos;
	float mCam_zpos;

	int texWidth, texHeight;
	GLuint texId;
	GLuint alphaCheckedTex;

	QPoint mAnchor;

	struct OccupationVertex {
		GLfloat pos[3];
		GLubyte color[4];
	};

	QVector<OccupationVertex> regionsData;
	bool showOccupation;
	QVector<float> highlightedData;
	QVector<AtlasBlock> const* atlasBlocks;
};

