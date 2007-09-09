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

#ifndef FIFEGUIMANAGER_H
#define FIFEGUIMANAGER_H

#include "singleton.h"

namespace gcn {
	class Gui;
	class Container;
	class Widget;
	class SDLInput;
}

namespace FIFE {

	class GCNImageLoader;
	class Console;
	class GUIManager : public DynamicSingleton<GUIManager> {
		public:
			GUIManager();
			virtual ~GUIManager();

			gcn::Gui* getGuichanGUI() const;

			void turn();
			void init();
			void resizeTopContainer(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
			void add(gcn::Widget* widget);
			void remove(gcn::Widget* widget);
			gcn::Container* getTopContainer() { return m_gcn_topcontainer; }
			Console* getConsole() { return m_console; };

		private:
			gcn::Gui* m_gcn_gui;
			gcn::Container* m_gcn_topcontainer;
			GCNImageLoader* m_gcn_imgloader;
			gcn::SDLInput *m_input;
			Console       *m_console;
	};

}

#endif
