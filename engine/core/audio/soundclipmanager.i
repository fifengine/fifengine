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

%module fife
%{
#include "audio/soundclip.h"
#include "audio/soundclipmanager.h"
%}

namespace FIFE {
	class SoundClip;

	class SoundClipManager : public IResourceManager {
	public:
		virtual ~SoundClipManager();
		
		virtual size_t getMemoryUsed() const;
		virtual size_t getTotalResourcesCreated() const;
		virtual size_t getTotalResourcesLoaded() const;
		virtual size_t getTotalResources() const;

		virtual SoundClipPtr create(const std::string& name, IResourceLoader* loader = 0);
		virtual SoundClipPtr load(const std::string& name, IResourceLoader* loader = 0);		
		virtual SoundClipPtr add(SoundClip* res);

		virtual bool exists(const std::string& name);
		virtual bool exists(ResourceHandle handle);

		virtual void reload(const std::string& name);
		virtual void reload(ResourceHandle handle);
		virtual void reloadAll();
		virtual void loadUnreferenced();

		virtual void free(const std::string& name);
		virtual void free(ResourceHandle handle);
		virtual void freeAll();
		virtual void freeUnreferenced();

		virtual void remove(SoundClipPtr& resource);
		virtual void remove(const std::string& name);
		virtual void remove(ResourceHandle handle);
		virtual void removeAll();
		virtual void removeUnreferenced();

		virtual SoundClipPtr get(const std::string& name);
		virtual SoundClipPtr get(ResourceHandle handle);

		virtual ResourceHandle getResourceHandle(const std::string& name);
	};
}
