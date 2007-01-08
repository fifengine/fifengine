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

// Standard C++ library includes
#include <cstdio>

// 3rd party library includes
#include <QMouseEvent>
#include <QTimer>

// FIFE includes
#include "exception.h"
#include "log.h"

#include "fmainwindow.h"

using namespace FIFEDIT;

FMainWindow::FMainWindow() : QMainWindow() {
	Initialize();
}

FMainWindow::~FMainWindow(){
	
}

bool FMainWindow::Initialize() {
	bool all_succeed = true;
	if(	!initFIFE() )
		all_succeed = false;
	if( !initQt() )
		all_succeed = false;
	if( !loadPlugins() )
		all_succeed = false;
	if( !initSignals() )
		all_succeed = false;
	return all_succeed;
}

bool FMainWindow::initFIFE() {
	try {
		FIFE::Log::setLogLevel(FIFE::Log::LEVEL_MAX);
	} catch (FIFE::Exception exception) {
		std::cout << "Could not init FIFE: " << exception.getMessage() << std::endl;
		return false;
	}
	return true;
}

bool FMainWindow::initQt() {
	m_window_ui.setupUi(this);
	return false;
}

bool FMainWindow::loadPlugins(char* directory_name) {
	return false;
}

bool FMainWindow::initSignals() {
	m_signal_processor = new FSignalProcessor();
	m_signal_processor->initializeMainWindow(this);

	return false;
}

bool FMainWindow::setSelectedObject(FIFE::MapObjectInfo* object) {
	m_selected_object_list.clear();
	m_selected_object_list.push_front(object);
	
	return true;
}

bool FMainWindow::setSelectedObjectMultiple(std::list<FIFE::MapObjectInfo*> object_list) {
	m_selected_object_list.clear();
	m_selected_object_list = object_list;
	
	return true;
}

FIFE::MapObjectInfo* FMainWindow::getSelectedObject() {
	return m_selected_object_list.front();
	
}

FIFE::MapObjectInfo* FMainWindow::getSelectedObjectFromList(int position) {
	int counter = 0;
	for( std::list<FIFE::MapObjectInfo*>::iterator obj_iter = m_selected_object_list.begin();
	     obj_iter != m_selected_object_list.end() && counter <= position;
	     obj_iter++ ){
		if( counter == position ){
			return *obj_iter;
		}
		counter++;
	}
	return 0;
}

std::list<FIFE::MapObjectInfo*> FMainWindow::getSelectedObjectList() {
	return m_selected_object_list;
}

FSignalProcessor* FMainWindow::getSignalProcessor() {
	return m_signal_processor;
}

void FMainWindow::Show() {
	this->show();
}

