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

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QPair>
#include <QImage>

namespace Ui {
    class MainWindow;
}
class AtlasBlock;
class QListWidgetItem;

struct FifeObject
{
	QString objId;
	QString ns;
	bool isBlocking;
	bool isStatic;
	QString pather;

	struct ImageData
	{
		QString source;
		int xoffset;
		int yoffset;
		int direction;
	};

	QVector<ImageData> images;
};

class QNamedImage : public QImage
{
public:
	QNamedImage()
		: QImage() {}
	QNamedImage(const QString & fileName)
		: QImage(fileName) {}
	QNamedImage(int width, int height, Format format)
		: QImage(width, height ,format) {}
	QNamedImage(const QImage& img)
		: QImage(img) {}

	QString name;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	typedef QVector<QNamedImage> Images;
	const Images& images() const { return subImgs; }

private slots:
	void addTexturesPressed();
	void addDirectoryPressed();
	void removeTexturesPressed();
	void showFullPathChanged(int state);
	void textureSelectionChanged();
	void refreshPressed();
	void savePressed();	

	void addObjectPressed();
	void removeObjectsPressed();
	void objectDoubleClicked(QListWidgetItem* item);

	void disassemblePressed();

private:
    Ui::MainWindow *ui;

	// Subimages with their sources
	Images subImgs;

	// Atlas data
	QVector<AtlasBlock> regions;
	QImage imgAtlas;

	QVector< QPair<QString, QString> > texturePaths;
	int sharedPathNumChars;

	QVector<QString> fileExtAllowed;
	QString filter;
	QString lastLoadDir;
	QString lastSaveDir;

	QMap<QString, FifeObject> fifeObjects;

	void updateSubimage(QImage& dest, int xoffset, int yoffset, const QImage& src);
};

inline bool ImageGreater(QNamedImage const& a, QNamedImage const& b)
{
	quint32 aa = a.width() * a.height();
	quint32 bb = b.width() * b.height();

	if(aa > bb)
		return true;

	if(aa < bb)
		return false;

	// aa == bb from here
	// a is taller but narrower
	if(a.height() > b.height())
		return true;

	if(a.width() > b.width())
		return false;

	// equal sized images sort alphabetically
	return a.name < b.name;
}

