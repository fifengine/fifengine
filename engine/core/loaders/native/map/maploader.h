/**************************************************************************
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

#ifndef FIFE_MAPLOADER_H_
#define FIFE_MAPLOADER_H_

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "imaploader.h"
#include "ianimationloader.h"
#include "percentdonelistener.h"

namespace FIFE {
	class Model;
	class Map;
	class VFS;
	class Layer;
	class ImageManager;
	class AnimationManager;
	class RenderBackend;
	class PercentDoneListener;

	class MapLoader : public IMapLoader {
	public:
		MapLoader(Model* model, VFS* vfs, ImageManager* imageManager, RenderBackend* renderBackend);
		
		~MapLoader();

		/** 
		* @see IMapLoader::setObjectLoader
		*/
		void setObjectLoader(const FIFE::ObjectLoaderPtr& objectLoader);

		/** 
		* @see IMapLoader::getObjectLoader
		*/
		FIFE::ObjectLoaderPtr getObjectLoader();

		/** 
		* @see IMapLoader::setAnimationLoader
		*/
		void setAnimationLoader(const FIFE::AnimationLoaderPtr& animationLoader);

		/** 
		* @see IMapLoader::getAnimationLoader
		*/
		FIFE::AnimationLoaderPtr getAnimationLoader();

		/** 
		* @see IMapLoader::setAtlasLoader
		*/
		void setAtlasLoader(const FIFE::AtlasLoaderPtr& atlasLoader);

		/** 
		* @see IMapLoader::getAtlasLoader
		*/
		FIFE::AtlasLoaderPtr getAtlasLoader();

		/** 
		* @see IMapLoader::isLoadable
		*/
		bool isLoadable(const std::string& filename) const;

		/** 
		* @see IMapLoader::load
		*/
		Map* load(const std::string& filename);

		/** used to load an object file
		* if directory is provided then file is assumed relative to directory
		* if relativeToMap is true then the file/directory is assumed to be relative to
		* location of map file
		*/
		void loadImportFile(const std::string& file, const std::string& directory="");

		/** used to load a directory of object files recursively
		* if relativeToMap is true then the directory is assumed to be relative to
		* location of map file
		*/
		void loadImportDirectory(const std::string& directory);

		/** 
		* allows adding a listener to the map loader 
		* for percent completed events
		*/
		void addPercentDoneListener(PercentDoneListener* listener);

		/** returns the loader name associated with this
		* map file loader, this will only be populated
		* after the load function is called
		*/
		const std::string& getLoaderName() const;

	private:
		Model* m_model;
		VFS* m_vfs;
		ImageManager* m_imageManager;
		AnimationManager* m_animationManager;
		ObjectLoaderPtr m_objectLoader;
		RenderBackend* m_renderBackend;
		PercentDoneCallback m_percentDoneListener;

		std::string m_loaderName;
		std::string m_mapDirectory;
		std::vector<std::string> m_importDirectories;

	};

	/** convenience function for creating the default fife map loader
	* deleting the object returned from this function is the
	* responsibility of the caller
	*/
	MapLoader* createDefaultMapLoader(Model* model, VFS* vfs, ImageManager* imageManager, RenderBackend* renderBackend);
}

#endif
