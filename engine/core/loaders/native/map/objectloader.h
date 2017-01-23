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

#ifndef FIFE_OBJECT_LOADER_H_
#define FIFE_OBJECT_LOADER_H_

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/sharedptr.h"

#include "iobjectloader.h"
#include "ianimationloader.h"
#include "iatlasloader.h"

namespace FIFE {

	class Model;
	class VFS;
	class ImageManager;
	class AnimationManager;

	class ObjectLoader : public IObjectLoader {
	public:
		ObjectLoader(Model* model, VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager, const AnimationLoaderPtr& animationLoader=AnimationLoaderPtr(), const AtlasLoaderPtr& atlasLoader=AtlasLoaderPtr());

		~ObjectLoader();

		/** 
		* @see IObjectLoader::setAnimationLoader
		*/
		virtual void setAnimationLoader(const AnimationLoaderPtr& animationLoader);

		/** 
		* @see IObjectLoader::getAnimationLoader
		*/
		virtual AnimationLoaderPtr getAnimationLoader();

		/** 
		* @see IObjectLoader::setAtlasLoader
		*/
		virtual void setAtlasLoader(const AtlasLoaderPtr& atlasLoader);

		/** 
		* @see IObjectLoader::getAtlasLoader
		*/
		virtual AtlasLoaderPtr getAtlasLoader();

		/** 
		* @see IObjectLoader::isLoadable
		*/
		virtual bool isLoadable(const std::string& filename) const;

		/** 
		* @see IObjectLoader::load
		*/
		virtual void load(const std::string& filename);

		/** used to load an object, atlas or animation file
		* if directory is provided then file is assumed relative to directory
		*/
		void loadImportFile(const std::string& file, const std::string& directory="");

		/** used to load a directory of object, atlas or animation  files recursively
		*/
		void loadImportDirectory(const std::string& directory);

	private:
		Model* m_model;
		VFS* m_vfs;
		ImageManager* m_imageManager;
		AnimationManager* m_animationManager;
		AnimationLoaderPtr m_animationLoader;
		AtlasLoaderPtr m_atlasLoader;
	};
}

#endif