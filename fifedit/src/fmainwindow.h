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

#ifndef FIFEDIT_MAINWINDOW_H
#define FIFEDIT_MAINWINDOW_H

// Standard C++ library includes
#include <list>

// 3rd party library includes
#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QToolBar>
#include "ui_mainwindow.h"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/map.h"
#include "map/objectinfo.h"
#include "vfs/vfs.h"

#include "fdetailview.h"
#include "fmapcontentview.h"
#include "fobjectbrowser.h"
#include "fpane.h"
#include "fplugin.h"
#include "fsignalprocessor.h"
 
namespace FIFEDIT{
	/** Forward declarations */
	class FSignalProcessor;
	/** End forward declarations */
	/** \warning Right now, unless otherwise documented, each feature should be 
	 *  considered unimplimented
	 */
	class FMainWindow : public QMainWindow {
		Q_OBJECT

	public:
		FMainWindow();
		~FMainWindow();

		/** System initialization */
		/** \brief Initializes all of the subsystems. 
		 *  Initializes the subsystems in order.  Each subsystem should be 
		 *  implemented such that this can be called as a re-initialization 
		 *  if something has changed or the system is in need of reloading.
		 *  @return bool Returns true if all systems loaded properly, false if 
		 *  one or more failed.
		 */
		bool Initialize();
		bool initFIFE();
		bool initQt();
		bool loadPlugins(char* directory_name = 0);
		bool initSignals();
		
		/** Data Interface */
		/** \brief Sets the application wide selected object to the supplied object.
		 *  This clears all data about any currently selected objects and sets 
		 *  the current object to the one supplied.
		 *  @param object The object to set as selected.
		 *  @return bool Returns true if successful, false if something strange happened.
		 */
		bool setSelectedObject(FIFE::map::ObjectInfo* object);
		/** \brief Sets the application wide selected objects to the supplied list.
		 *  This clears all data about any currently selected objects and sets the 
		 *  current list of selected objects to the supplied list.
		 *  @param object_list The list of objects to set as selected.
		 *  @return bool Returns true if successful, false if something strange happened.
		 */
		bool setSelectedObjectMultiple(std::list<FIFE::map::ObjectInfo*> object_list);
		/** \brief Returns the currently selected object.
		 *  \warning If multiple objects are selected, this will simply return the first 
		 *  object.
		 */
		FIFE::map::ObjectInfo* getSelectedObject();
		/** \brief Returns the currently selected object from the specified position in
		 *  the list.
		 *  @param position The position to get the object from (0 is the first position).
		 *  @return FIFE::Object* returns the object or null if there is no object a that
		 *  position.
		 */
		FIFE::map::ObjectInfo* getSelectedObjectFromList(int position = 0);
		/** \brief Returns the list of currently selected objects. */
		std::list<FIFE::map::ObjectInfo*> getSelectedObjectList();
		/** \brief Returns the signal processor for use in connecting slots/signals. */
		FSignalProcessor* getSignalProcessor();
		/** \brief Access method to show() from Ui::MainWindow */
		void Show();
	signals:
		void fileOpened();
		void fileClosed();
		
	public slots:
		
	private:
		/** FIFE Data Items */
		FIFE::map* m_map_data;
		std::list<FIFE::map::ObjectInfo*> m_selected_object_list;
		/** Custom Qt Widget and Component Items */
		FObjectBrowser* m_object_browser;
		FMapContentView* m_map_content_view;
		FDetailView* m_detail_view;
		std::list<FPlugin*> m_plugins;
		/** Qt Widget, Display and Component Items */
		Ui::MainWindow m_window_ui;
		QMenu* m_main_menu;
		QToolBar* m_toolbar;
		/** App specific data */
		FSignalProcessor* m_signal_processor;
	}; 
}

#endif
