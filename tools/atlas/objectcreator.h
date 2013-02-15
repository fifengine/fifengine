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
#include <QDialog>

namespace Ui {
    class ObjectCreator;
}

struct FifeObject;

class ObjectCreator : public QDialog
{
    Q_OBJECT

public:
	explicit ObjectCreator(FifeObject& fobj, QWidget *parent = 0);
    ~ObjectCreator();

private slots:
	void addDirectionPressed();
	void removeDirectionPressed();
	void directionIndexChanged(int index);
	void previewImageIndexChanged(int index);

	void savePressed();
	void cancelPressed();

private:
    Ui::ObjectCreator *ui;
	FifeObject& obj;
};
