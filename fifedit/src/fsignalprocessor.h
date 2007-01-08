/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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
 
#ifndef FIFEDIT_FSIGNALPROCESSOR_H
#define FIFEDIT_FSIGNALPROCESSOR_H

// Standard C++ library includes

// 3rd party library includes
#include <QMainWindow>
#include <QObject>

// FIFE includes
#include "fmainwindow.h"

namespace FIFEDIT {
	class FMainWindow;
	class FSignalProcessor : public QObject {
		Q_OBJECT
	signals:
		void newObjectSelected();
		void multipleObjectsSelected();
		void noObjectSelected();
		void selectedObjectChanged();
		void fileOpened();
		void fileClosed();
		void displayAreaChanged();
	public slots:
		void selectedObject();
	public:
		FSignalProcessor();	
		~FSignalProcessor();

		void initializeMainWindow(FMainWindow* main_window);
		void initializeObject(QObject *object);	
	private:
		FMainWindow* win_main;
	};
}

#endif
