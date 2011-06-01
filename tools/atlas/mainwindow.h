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

#pragma once

#include <QMainWindow>

#include "glwidget.h"
#include "atlas.h"
#include "image.h"

#include <QVector>
#include <QPair>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
	void addTexturesPressed();
	void addDirectoryPressed();
	void removeTexturesPressed();
	void refreshPressed();
	void savePressed();
	void absolutePathChanged(int state);
	void showFullPathChanged(int state);
	void showSubImageChanged(int state);

private:
    Ui::MainWindow *ui;

	GLWidget* mGLWidget;

	QVector<Image*> subImgs;
	QVector<AtlasBlock> regions;
	QVector<Image> imgAtlases;

	QVector< QPair<QString, QString> > texturePaths;
	int numAtlases;
};
