/***************************************************************************
*   Copyright (C) 2005-2008 by the FIFE team                              *
*   http://www.fifengine.de                                               *
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

// Standard C++ library includes

// 3rd party library includes
#include "boost/filesystem.hpp"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "ext/tinyxml/fife_tinyxml.h"
#include "vfs/vfs.h"
#include "vfs/raw/rawdata.h"

#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"

#include "atlasloader.h"

namespace fs = boost::filesystem;

namespace FIFE {
	static Logger _log(LM_NATIVE_LOADERS);

	AtlasLoader::AtlasLoader(VFS* vfs, ImageManager* imageManager)
	: m_vfs(vfs), m_imageManager(imageManager) {
	}

	AtlasLoader::~AtlasLoader() {
	}

	bool AtlasLoader::isLoadable(const std::string& filename) {
		fs::path atlasPath(filename);
		std::string atlasFilename = atlasPath.file_string();

		try {
			RawData* data = m_vfs->open(atlasFilename);

			if (data) {
				if (data->getDataLength() != 0) {
					// TODO - this could be expanded to do more checks
					TiXmlDocument doc;
					doc.Parse(reinterpret_cast<const char*>(&data->getDataInBytes()[0]));

					if (doc.Error()) {
						return false;
					}
				}

				// done with data delete resource
				delete data;
				data = 0;
			}
		} catch (NotFound&) {
			return false;
		}

		return true;
	}

	Atlas* AtlasLoader::load(const std::string& filename) {
		fs::path atlasPath(filename);
		std::string atlasFilename = atlasPath.file_string();

		TiXmlDocument doc;
		Atlas* atlas = NULL;

		try {
			RawData* data = m_vfs->open(atlasFilename);

			if (data) {
				if (data->getDataLength() != 0) {
					doc.Parse(reinterpret_cast<const char*>(&data->getDataInBytes()[0]));
					//bool isFileLoaded = doc.LoadFile(atlasFilename);
					if (doc.Error()) {
						return atlas;
					}

					// done with data delete resource
					delete data;
					data = 0;
				}
			}
		}
		catch (NotFound& e) {
			FL_ERR(_log, e.what());

			// TODO - should we abort here
			//        or rethrow the exception
			//        or just keep going

			return atlas;
		}

		// if we get here then everything loaded properly
		// so we can just parse out the contents
		TiXmlElement* root = doc.RootElement();

		if (root) {
			const std::string* atlasName = root->Attribute(std::string("name"));
			if(atlasName) {
				atlas = new Atlas(*atlasName);

				int atlasWidth, atlasHeight;
				root->QueryValueAttribute("width", &atlasWidth);
				root->QueryValueAttribute("height", &atlasHeight);

				Atlas::SubimageMap& subimages = atlas->getSubimages();

				for (TiXmlElement* imageElem = root->FirstChildElement("image");
					imageElem != 0; imageElem = imageElem->NextSiblingElement("image")) {

						Rect region;
						imageElem->QueryValueAttribute("xpos", &region.x);
						imageElem->QueryValueAttribute("ypos", &region.y);
						imageElem->QueryValueAttribute("width", &region.w);
						imageElem->QueryValueAttribute("height", &region.h);

						const std::string* subimageName = imageElem->Attribute(std::string("source"));
						subimages.insert(std::make_pair(*subimageName, region));
				}

				atlas->setImage(m_imageManager->load(*atlasName));
				atlas->getImage()->forceLoadInternal();
				
				for (Atlas::SubimageMap::iterator it = subimages.begin(); it != subimages.end(); ++it) {
					ImagePtr subImage = m_imageManager->create(it->first);
					subImage->useSharedImage(atlas->getImage(), it->second, atlasWidth, atlasHeight);
				}
			}
		}

		return atlas;
	}

	AtlasLoader* createDefaultAtlasLoader(VFS* vfs, ImageManager* imageManager) {
		return new AtlasLoader(vfs, imageManager);
	}
}
