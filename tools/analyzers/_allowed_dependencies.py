# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

# the following dictionary defines the allowed dependencies checked while testing
# format = module -> allowed dependencies from this module
ALLOWED_MODULE_DEPS = {
	'controller': ('model', 'eventchannel', 'gui', 'video', 'loaders', 'view', 'audio', 'util', 'vfs', 'pathfinder'),
	'model': ('util', 'eventchannel'),
	'eventchannel': ('util',),
	'gui': ('eventchannel', 'util', 'video', 'vfs'),
	'video': ('util',),
	'loaders': ('audio', 'video', 'vfs', 'xml', 'util', 'view', 'model'),
	'view': ('model', 'video', 'util', 'audio', 'eventchannel'),
	'audio': ('util', 'vfs'),
	'util': ['engine'],
	'vfs': ('util',),
	'xml': ('vfs', 'util'),
	'pathfinder': ('util', 'model'),
}
