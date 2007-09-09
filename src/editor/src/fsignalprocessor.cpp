/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes
#include <QObject>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fsignalprocessor.h"

using namespace FIFEDIT;

FSignalProcessor::FSignalProcessor() {

}

FSignalProcessor::~FSignalProcessor() {

}

void FSignalProcessor::initializeMainWindow(FMainWindow *main_window){
	win_main = main_window;
	connect(win_main, SIGNAL(fileOpened()), this, SIGNAL(fileOpened()));
	connect(win_main, SIGNAL(fileClosed()), this, SIGNAL(fileClosed()));
}

void FSignalProcessor::selectedObject() {
	if( win_main->getSelectedObject() == NULL )
		emit noObjectSelected();
	if( win_main->getSelectedObjectFromList(1) == NULL )
		emit newObjectSelected();
	else
		emit multipleObjectsSelected();	
}

void FSignalProcessor::initializeObject(QObject *object) {
	connect(object, SIGNAL(newObjectSelected()), this, SLOT(selectedObject()));
	connect(object, SIGNAL(multipleObjectsSelected()), this, SLOT(selectedObject()));
	connect(object, SIGNAL(noObjectSelected()), this, SLOT(selectedObject()));
	connect(object, SIGNAL(selectedObjectChanged()), this, SIGNAL(selectedObjectChanged()));
	connect(object, SIGNAL(displayAreaChanged()), this, SIGNAL(displayAreaChanged()));
}
