/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
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


	size_t Atlas::getImageCount() const { 
		return m_subimages.size();
	}

	ImagePtr& Atlas::getPackedImage() { 
		return m_image;
	}

	ImagePtr Atlas::getImage(const std::string& id) {
		SubimageMap::iterator iter = m_subimages.find(id);
		if(iter == m_subimages.end())
			return ImagePtr();
		return iter->second.image;
	}

	ImagePtr Atlas::getImage(uint32_t index) {
		if(index > getImageCount())
			return ImagePtr();

		SubimageMap::iterator iter = m_subimages.begin();
		for(uint32_t i = 0; i < index; ++i, ++iter);
		return iter->second.image;
	}

	bool Atlas::addImage(const std::string& imagename, const AtlasData& data) {
		return m_subimages.insert(std::pair<std::string, AtlasData>(imagename, data)).second;
	}

	void Atlas::setPackedImage(const ImagePtr& image) {
		m_image = image;
	}

	AtlasLoader::AtlasLoader(VFS* vfs, ImageManager* imageManager)
	: m_vfs(vfs), m_imageManager(imageManager) {
	}

	AtlasLoader::~AtlasLoader() {
	}

	bool AtlasLoader::isLoadable(const std::string& filename) {
		fs::path atlasPath(filename);
		std::string atlasFilename = atlasPath.string();

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
		std::string atlasFilename = atlasPath.string();

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
				fs::path atlasImagePath = atlasPath.parent_path() / *atlasName;
				atlas = new Atlas(atlasImagePath.string());

				// End-user could create the same atlas for the second time.
				// Since we don't hold any data for Atlases like ImageManager we need to recreate 
				// atlas parameters (to return proper AtlasPtr) but don't reload pixel data (they are held by ImageManager).

				bool atlasExists = m_imageManager->exists(atlas->getName());

				if(!atlasExists) {
					atlas->setPackedImage(m_imageManager->load(atlas->getName()));
					atlas->getPackedImage()->forceLoadInternal();
				}

				// We don't really need this now, though we could use it to assert if the loaded atlas is the same sized as these
				//int atlasWidth, atlasHeight;
				//root->QueryValueAttribute("width", &atlasWidth);
				//root->QueryValueAttribute("height", &atlasHeight);

				for (TiXmlElement* imageElem = root->FirstChildElement("image");
					imageElem != 0; imageElem = imageElem->NextSiblingElement("image")) {

					Rect region;
					imageElem->QueryValueAttribute("xpos", &region.x);
					imageElem->QueryValueAttribute("ypos", &region.y);
					imageElem->QueryValueAttribute("width", &region.w);
					imageElem->QueryValueAttribute("height", &region.h);

					const std::string* subimageName = imageElem->Attribute(std::string("source"));

					if(!atlasExists && subimageName) {
						fs::path subimagePath(filename);

						if(subimagePath.has_parent_path()) {
							subimagePath = subimagePath.parent_path() / *subimageName;
						} else {
							subimagePath = fs::path(*subimageName);
						}

						std::string finalname = subimagePath.string();
						ImagePtr subImage = m_imageManager->create(finalname);
						subImage->useSharedImage(atlas->getPackedImage(), region);
						
						Atlas::AtlasData atlas_data = {region, subImage};
						atlas->addImage(finalname, atlas_data);
					}
				}
			}
		}

		return atlas;
	}

	AtlasLoader* createDefaultAtlasLoader(VFS* vfs, ImageManager* imageManager) {
		return new AtlasLoader(vfs, imageManager);
	}
}
