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

// Standard C++ library includes

// 3rd party library includes
#include <tinyxml.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/model.h"
#include "model/structures/layer.h"
#include "vfs/fife_boost_filesystem.h"
#include "vfs/vfs.h"
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"
#include "video/animationmanager.h" 
#include "view/visual.h"

#include "atlasloader.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
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

	const std::string& Atlas::getName() const {
		return m_name;
	}

	AtlasLoader::AtlasLoader(Model* model, VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager)
	: m_model(model), m_vfs(vfs), m_imageManager(imageManager), m_animationManager(animationManager) {
	}

	AtlasLoader::~AtlasLoader() {
	}

	bool AtlasLoader::isLoadable(const std::string& filename) {
		bfs::path atlasPath(filename);
		std::string atlasFilename = atlasPath.string();
		TiXmlDocument atlasFile;

		try {
			RawData* data = m_vfs->open(atlasFilename);

			if (data) {
				if (data->getDataLength() != 0) {
					atlasFile.Parse(data->readString(data->getDataLength()).c_str());

					if (atlasFile.Error()) {
						return false;
					}
				} else {
					return false;
				}

				// done with data delete resource
				delete data;
				data = 0;
			}
		} catch (NotFound&) {
			return false;
		}

		// if we get here then loading the file went well
		TiXmlElement* root = atlasFile.RootElement();

		if (root && root->ValueStr() == "assets") {
			if (root->FirstChildElement("atlas")) {
				return true;
			}
		}

		return false;
	}

	AtlasPtr AtlasLoader::load(const std::string& filename) {
		bfs::path atlasPath(filename);
		bfs::path atlasPathDirectory;
		std::string atlasFilename = atlasPath.string();

		if (HasParentPath(atlasPath)) {
			// save the directory where the atlas file is located
			atlasPathDirectory = GetParentPath(atlasPath);
		}

		TiXmlDocument doc;
		AtlasPtr atlas;

		try {
			RawData* data = m_vfs->open(atlasFilename);

			if (data) {
				if (data->getDataLength() != 0) {
					doc.Parse(data->readString(data->getDataLength()).c_str());

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

		if (root && root->ValueStr() == "assets") {
			atlas = loadAtlas(filename, root->FirstChildElement("atlas"));
		}

		return atlas;
	}

	std::vector<AtlasPtr> AtlasLoader::loadMultiple(const std::string& filename) {
		bfs::path atlasPath(filename);
		bfs::path atlasPathDirectory;
		std::string atlasFilename = atlasPath.string();

		if (HasParentPath(atlasPath)) {
			// save the directory where the atlas file is located
			atlasPathDirectory = GetParentPath(atlasPath);
		}

		TiXmlDocument doc;
		std::vector<AtlasPtr> atlasVector;

		try {
			RawData* data = m_vfs->open(atlasFilename);

			if (data) {
				if (data->getDataLength() != 0) {
					doc.Parse(data->readString(data->getDataLength()).c_str());

					if (doc.Error()) {
						return atlasVector;
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

			return atlasVector;
		}

		// if we get here then everything loaded properly
		// so we can just parse out the contents
		TiXmlElement* root = doc.RootElement();

		if (root && root->ValueStr() == "assets") {
			for (TiXmlElement* atlasElem = root->FirstChildElement("atlas"); atlasElem; atlasElem = atlasElem->NextSiblingElement("atlas")) {
				AtlasPtr atlas = loadAtlas(filename, atlasElem);
				if (atlas) {
					atlasVector.push_back(atlas);
				}
			}
		}

		return atlasVector;
	}

	AtlasPtr AtlasLoader::loadAtlas(const std::string& filename, TiXmlElement* atlasElem) {
		AtlasPtr atlas;
		if (!atlasElem) {
			return atlas;
		}

		const std::string* atlasSource = atlasElem->Attribute(std::string("source"));
		if (atlasSource) {
			const std::string* atlasId = atlasElem->Attribute(std::string("id"));

			bfs::path atlasPath(filename);
			bfs::path atlasPathDirectory;
			if (HasParentPath(atlasPath)) {
				// save the directory where the atlas file is located
				atlasPathDirectory = GetParentPath(atlasPath);
			}

			// Atlas itself doesn't have appended id
			bfs::path atlasImagePath = atlasPathDirectory / *atlasSource;
			atlas.reset(new Atlas(atlasImagePath.string()));

			// End-user could create the same atlas for the second time.
			// Since we don't hold any data for Atlases like ImageManager we need to recreate
			// atlas parameters (to return proper AtlasPtr) but don't reload pixel data (they are held by ImageManager).
			if (!m_imageManager->exists(atlas->getName())) {
				atlas->setPackedImage(m_imageManager->create(atlas->getName()));
			} else {
				atlas->setPackedImage(m_imageManager->getPtr(atlas->getName()));
			}
			// Create subimages with given id and individual position and size
			if (atlasElem->FirstChildElement("subimage")) {
				for (TiXmlElement* imageElem = atlasElem->FirstChildElement("subimage");
					imageElem != 0; imageElem = imageElem->NextSiblingElement("subimage")) {

					const std::string* subimageId = imageElem->Attribute(std::string("id"));
					if (subimageId) {
						Rect region;
						imageElem->QueryValueAttribute("xpos", &region.x);
						imageElem->QueryValueAttribute("ypos", &region.y);
						imageElem->QueryValueAttribute("width", &region.w);
						imageElem->QueryValueAttribute("height", &region.h);

						std::string finalname;
						// atlas id is optional here
						if (atlasId) {
							finalname = *atlasId + ":" + *subimageId;
						} else {
							finalname = *subimageId;
						}
						ImagePtr subImage;

						if (!m_imageManager->exists(finalname)) {
							subImage = m_imageManager->create(finalname);
						} else {
							subImage = m_imageManager->getPtr(finalname);
						}
						subImage->useSharedImage(atlas->getPackedImage(), region);

						AtlasData atlasData = {region, subImage};
						atlas->addImage(finalname, atlasData);
					}
				}
			} else {
				// Create subimages with automatic id and same size
				int frame = 0;
				int atlasWidth = 0;
				int atlasHeight = 0;
				int subimageWidth = 0;
				int subimageHeight = 0;
				atlasElem->QueryValueAttribute("atlas_width", &atlasWidth);
				atlasElem->QueryValueAttribute("atlas_height", &atlasHeight);
				atlasElem->QueryValueAttribute("subimage_width", &subimageWidth);
				atlasElem->QueryValueAttribute("subimage_height", &subimageHeight);
				// file extension of the atlas is also used as subimage extension
				std::string extension = bfs::extension(*atlasSource);
				// we need an atlas id
				if (!atlasId) {
					atlasId = atlasSource;
				}
				
				if (atlasWidth != 0 && atlasHeight != 0 && subimageWidth != 0 && subimageHeight != 0) {
					int x_rows = atlasWidth / subimageWidth;
					int y_rows = atlasHeight / subimageHeight;
					Rect region(0, 0, subimageWidth, subimageHeight);
					for (int y = 0; y < y_rows; ++y) {
						region.y = y * subimageHeight;
						for (int x = 0; x < x_rows; ++x) {
							region.x = x * subimageWidth;

							static char tmp[64];
							snprintf(tmp, 64, "%04d", frame);
							std::ostringstream finalname;
							finalname << *atlasId << ":" << std::string(tmp) << extension;

							ImagePtr subImage;
							if (!m_imageManager->exists(finalname.str())) {
								subImage = m_imageManager->create(finalname.str());
							} else {
								subImage = m_imageManager->getPtr(finalname.str());
							}
							subImage->useSharedImage(atlas->getPackedImage(), region);

							AtlasData atlasData = {region, subImage};
							atlas->addImage(finalname.str(), atlasData);

							++frame;
						}
					}
				}
			}
		}

		return atlas;
	}

	AtlasLoader* createDefaultAtlasLoader(Model* model, VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager) {
		return new AtlasLoader(model, vfs, imageManager, animationManager);
	}
}
