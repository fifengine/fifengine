/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

#ifndef FIFE_MODULES_H
#define FIFE_MODULES_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

/** Modules available for logging
 */
enum logmodule_t {
	LM_CORE = -1,
	LM_AUDIO,
	LM_CONTROLLER,
	LM_EVTCHANNEL,
	LM_GUI,
	LM_CONSOLE,
	LM_LOADERS,
	LM_NATIVE_LOADERS,
	LM_FO_LOADERS,
    LM_SAVERS,
    LM_NATIVE_SAVERS,
	LM_MODEL,
	LM_STRUCTURES,
	LM_INSTANCE,
	LM_LOCATION,
	LM_METAMODEL,
	LM_CELLGRID,
	LM_SQUAREGRID,
	LM_HEXGRID,
	LM_PATHFINDER,
	LM_UTIL,
	LM_RESMGR,
	LM_VFS,
	LM_VIDEO,
	LM_VIEW,
	LM_CAMERA,
	LM_VIEWVIEW,
	LM_XML,
	LM_EXCEPTION,
	LM_SCRIPT,
	LM_MODULE_MAX // sentinel
};

/** Module hierarchy definition + display strings
 * format = module, module parent, module display name
 */
#define MODULE_INFO_RELATIONSHIPS \
	ModuleInfo moduleInfos[] = { \
		{LM_AUDIO, LM_CORE, "Audio"}, \
		{LM_CONTROLLER, LM_CORE, "Controller"}, \
		{LM_EVTCHANNEL, LM_CORE, "Event Channel"}, \
		{LM_GUI, LM_CORE, "GUI"}, \
		  {LM_CONSOLE, LM_GUI, "Console"}, \
		{LM_LOADERS, LM_CORE, "Loaders"}, \
		  {LM_NATIVE_LOADERS, LM_LOADERS, "Native loaders"}, \
		  {LM_FO_LOADERS, LM_LOADERS, "Fallout loaders"}, \
		{LM_SAVERS, LM_CORE, "Savers"}, \
          {LM_NATIVE_SAVERS, LM_CORE, "Native savers"}, \
		{LM_MODEL, LM_CORE, "Model"}, \
		  {LM_STRUCTURES, LM_MODEL, "Structures"}, \
		    {LM_INSTANCE, LM_STRUCTURES, "Instance"}, \
		    {LM_LOCATION, LM_STRUCTURES, "Location"}, \
		  {LM_METAMODEL, LM_MODEL, "Metamodel"}, \
		    {LM_CELLGRID, LM_METAMODEL, "Cellgrid"}, \
		    {LM_SQUAREGRID, LM_METAMODEL, "Squaregrid"}, \
		    {LM_HEXGRID, LM_METAMODEL, "Hexgrid"}, \
		{LM_PATHFINDER, LM_CORE, "Pathfinder"}, \
		{LM_UTIL, LM_CORE, "Util"}, \
		  {LM_RESMGR, LM_UTIL, "Resource Manager"}, \
		{LM_VFS, LM_CORE, "VFS"}, \
		{LM_VIDEO, LM_CORE, "Video" }, \
		{LM_VIEW, LM_CORE, "View"}, \
		  {LM_CAMERA, LM_VIEW, "Camera"}, \
		  {LM_VIEWVIEW, LM_VIEW, "View::View"}, \
		{LM_XML, LM_CORE, "XML"}, \
		{LM_EXCEPTION, LM_CORE, "Exception"}, \
		{LM_SCRIPT, LM_CORE, "Script"} \
	};

#endif
