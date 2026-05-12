// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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