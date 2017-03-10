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

#ifndef FIFE_EXPORT_H
#define FIFE_EXPORT_H

#if defined _WIN32 || defined __CYGWIN__
	#ifdef FIFE_EXPORTING
		#ifdef __GNUC__
			#define FIFE_PUBLIC __attribute__((dllexport))
		#else
			#define FIFE_PUBLIC __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define FIFE_PUBLIC __attribute__((dllimport))
		#else
			#define FIFE_PUBLIC __declspec(dllimport)
		#endif
	#endif
	#define FIFE_HIDDEN
#else
	#if __GNUC__ >= 4
		#define FIFE_PUBLIC __attribute__((visibility("default")))
		#define FIFE_HIDDEN __attribute__((visibility("hidden")))
	#else
		#define FIFE_PUBLIC
		#define FIFE_HIDDEN
	#endif
#endif


#endif //FIFE_EXPORT_H
