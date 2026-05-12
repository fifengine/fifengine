// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "gui/guimanager.h"
#include "eventchannel/sdl/isdleventlistener.h"
%}

namespace FIFE {
	class IGUIManager :	public ISdlEventListener {
	public:
		virtual ~IGUIManager();
		virtual void turn() = 0;
		virtual void resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	};
}