/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <iostream>
#include "log.h"

#define DEBUG_PRINT(msg) std::cerr << "DEBUG: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
#define PANIC_PRINT(msg)                                                        \
do {                                                                            \
	std::cerr << "-------- PANIC --------\n" << __PRETTY_FUNCTION__ << "\n" \
                  << __FILE__ << ":" << __LINE__ << "\n" << msg << std::endl;   \
	abort();                                                                \
} while(0) 

#endif
