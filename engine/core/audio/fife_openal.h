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

#ifndef FIFE_FIFE_OPENAL_H
#define FIFE_FIFE_OPENAL_H

// Standard C++ library includes

// Platform specific includes
// Linux
#if defined( __unix__ )
#include <AL/al.h>
#include <AL/alc.h>
#endif

// Win32
#if defined( WIN32 )
#include <AL/al.h>
#include <AL/alc.h>
#endif

// Macintosh
#if defined( __APPLE_CC__ )
#include <al.h>
#include <alc.h>
#endif

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#ifdef LOG_ENABLED
#define CHECK_OPENAL_LOG(logger, level, msg) if (AL_NO_ERROR != alGetError()) { logger.log(level, msg);}

#define CHECK_OPENAL_LOG_DETAIL(logger, level, msg) {ALenum error; error = alGetError(); if (AL_NO_ERROR != error) { logger.log(level, LMsg() << msg << ", Error#: " << error);}}
#endif

#define CHECK_OPENAL_EXCEPTION(msg) if (AL_NO_ERROR != alGetError()) { throw Exception(msg); }

#endif
