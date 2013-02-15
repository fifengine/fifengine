/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "ext/tinyxml/fife_tinyxml.h"
#include "model/model.h"
#include "model/structures/layer.h"
#include "vfs/fife_boost_filesystem.h"
#include "vfs/vfs.h"
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"
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

	AtlasLoader::AtlasLoader(Model* model, VFS* vfs, ImageManager* imageManager)
	: m_model(model), m_vfs(vfs), m_imageManager(imageManager) {
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

        if (root && root->ValueStr() == "atlas") {
            return true;
        }
        else {
            return false;
        }
	}

	AtlasPtr AtlasLoader::load(const std::string& filename) {
		bfs::path atlasPath(filename);
		bfs::path atlasPathDirectory;
		m_atlasFilename = atlasPath.string();

		if (HasParentPath(atlasPath)) {
			// save the directory where the atlas file is located
			atlasPathDirectory = GetParentPath(atlasPath);
		}

		TiXmlDocument doc;
		AtlasPtr atlas;

		try {
			RawData* data = m_vfs->open(m_atlasFilename);

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

		if (root && root->ValueStr() == "atlas") {
			const std::string* atlasName = root->Attribute(std::string("name"));
			if(atlasName) {
				const std::string* namespaceId = root->Attribute(std::string("namespace"));
				if(!namespaceId) {
					namespaceId = atlasName;
				}

				// Atlas itself doesn't have appended namespace
				bfs::path atlasImagePath = atlasPathDirectory / *atlasName;
				atlas.reset(new Atlas(atlasImagePath.string()));

				// End-user could create the same atlas for the second time.
				// Since we don't hold any data for Atlases like ImageManager we need to recreate
				// atlas parameters (to return proper AtlasPtr) but don't reload pixel data (they are held by ImageManager).

				bool atlasExists = m_imageManager->exists(atlas->getName());

				if(!atlasExists) {
					atlas->setPackedImage(m_imageManager->create(atlas->getName()));
				}
				else {
					atlas->setPackedImage(m_imageManager->getPtr(atlas->getName()));
				}

				// We don't really need this now, though we could use it to assert if the loaded atlas is the same sized as these
				//int atlasWidth, atlasHeight;
				//root->QueryValueAttribute("width", &atlasWidth);
				//root->QueryValueAttribute("height", &atlasHeight);
				bool subsExists = true;
				for (TiXmlElement* imageElem = root->FirstChildElement("image");
					imageElem != 0; imageElem = imageElem->NextSiblingElement("image")) {

					Rect region;
					imageElem->QueryValueAttribute("xpos", &region.x);
					imageElem->QueryValueAttribute("ypos", &region.y);
					imageElem->QueryValueAttribute("width", &region.w);
					imageElem->QueryValueAttribute("height", &region.h);

					const std::string* subimageName = imageElem->Attribute(std::string("source"));

					if (subimageName) {
						std::string finalname = *namespaceId + ":" +*subimageName;
						ImagePtr subImage;

						bool subExists = m_imageManager->exists(finalname);
						if (!subExists) {
							subsExists = false;
							subImage = m_imageManager->create(finalname);
						}
						else {
							subImage = m_imageManager->getPtr(finalname);
						}
						subImage->useSharedImage(atlas->getPackedImage(), region);

						AtlasData atlas_data = {region, subImage};
						atlas->addImage(finalname, atlas_data);
					}
				}
				subsExists = subsExists && atlasExists;

				// Now parse object definition
				for(TiXmlElement* objElem = root->NextSiblingElement("object");
					objElem != 0; objElem = objElem->NextSiblingElement("object"))
				{
					// sanity check
					if(objElem->ValueStr() == "object") {
						parseObject(atlas.get(), objElem, subsExists);
					}
				}
			}
		}

		return atlas;
	}

	void AtlasLoader::parseObject(Atlas* atlas, TiXmlElement* root, bool exists) {
		const std::string* objectId = root->Attribute(std::string("id"));
		const std::string* namespaceId = root->Attribute(std::string("namespace"));

		Object* obj = NULL;
		if (objectId && namespaceId) {
			const std::string* parentId = root->Attribute(std::string("parent"));

			if (parentId) {
				Object* parent = m_model->getObject(*parentId, *namespaceId);
				if (parent) {
					try {
						obj = m_model->createObject(*objectId, *namespaceId, parent);
					}
					catch  (NameClash&) {
						// TODO - handle exception
						assert(false);
					}
				}
			}
			else {
				// this will make sure the object has not already been loaded
				if (m_model->getObject(*objectId, *namespaceId) == NULL) {
					try {
						obj = m_model->createObject(*objectId, *namespaceId);
					}
					catch (NameClash&) {
						// TODO - handle exception
						assert(false);
					}
				// if atlas or subimage was recreated then we have to update the ObjectVisual
				} else if (!exists) {
					obj = m_model->getObject(*objectId, *namespaceId);
					ObjectVisual* objVisual = obj->getVisual<ObjectVisual>();
					// make sure obj have visual
					if (!objVisual) {
						objVisual = ObjectVisual::create(obj);
					}

					for (TiXmlElement* imageElement = root->FirstChildElement("image"); imageElement; imageElement = imageElement->NextSiblingElement("image")) {
						const std::string* sourceId = imageElement->Attribute(std::string("source"));

						if (sourceId) {
							std::string source = *namespaceId + ":" + *sourceId;
							if(!m_imageManager->exists(source)) {
								throw NotFound(source + " couldn't be found.");
							}
							ImagePtr imagePtr = m_imageManager->getPtr(source);

							int xOffset = 0;
							int success = imageElement->QueryIntAttribute("x_offset", &xOffset);
							if (success == TIXML_SUCCESS) {
								imagePtr->setXShift(xOffset);
							}

							int yOffset = 0;
							success = imageElement->QueryIntAttribute("y_offset", &yOffset);
							if (success == TIXML_SUCCESS) {
								imagePtr->setYShift(yOffset);
							}

							int direction = 0;
							success = imageElement->QueryIntAttribute("direction", &direction);
							if (success == TIXML_SUCCESS) {
								if (objVisual) {
									objVisual->addStaticImage(direction, static_cast<int32_t>(imagePtr->getHandle()));
								}
							}
						}
					}
					return;
				}
			}
		}

		if (obj) {
			//obj->setFilename(atlas->getName());
            obj->setFilename(m_atlasFilename);
			ObjectVisual::create(obj);

			int isBlocking = 0;
			root->QueryIntAttribute("blocking", &isBlocking);
			obj->setBlocking(isBlocking!=0);

			int isStatic = 0;
			root->QueryIntAttribute("static", &isStatic);
			obj->setStatic(isStatic!=0);

			const std::string* pather = root->Attribute(std::string("pather"));

			if (pather) {
				obj->setPather(m_model->getPather(*pather));
			}
			else {
				obj->setPather(m_model->getPather("RoutePather"));
			}

			const std::string* costId = root->Attribute(std::string("cost_id"));
			if (costId) {
				obj->setCostId(*costId);
				double cost = 1.0;
				int success = root->QueryDoubleAttribute("cost", &cost);
				if (success == TIXML_SUCCESS) {
					obj->setCost(cost);
				}
			}
				
			const std::string* areaId = root->Attribute(std::string("area_id"));
			if (areaId) {
				obj->setArea(*areaId);
			}

			// loop over all walkable areas
            for (TiXmlElement* walkableElement = root->FirstChildElement("walkable_area"); walkableElement; walkableElement = walkableElement->NextSiblingElement("walkable_area")) {
                const std::string* walkableId = walkableElement->Attribute(std::string("id"));
				if (walkableId) {
					obj->addWalkableArea(*walkableId);
				}
			}

			int cellStack = 0;
			root->QueryIntAttribute("cellstack", &cellStack);
			obj->setCellStackPosition(cellStack);
				
			double ax = 0;
			double ay = 0;
			double az = 0;

			int xRetVal = root->QueryValueAttribute("anchor_x", &ax);
			int yRetVal = root->QueryValueAttribute("anchor_y", &ay);
			if (xRetVal == TIXML_SUCCESS && yRetVal == TIXML_SUCCESS) {
				obj->setRotationAnchor(ExactModelCoordinate(ax, ay, az));
			}

			int isRestrictedRotation = 0;
			root->QueryIntAttribute("restricted_rotation", &isRestrictedRotation);
			obj->setRestrictedRotation(isRestrictedRotation!=0);

			int zStep = 0;
			int zRetVal = root->QueryIntAttribute("z_step_limit", &zStep);
			if (zRetVal == TIXML_SUCCESS) {
				obj->setZStepRange(zStep);
			}

			// loop over all multi parts
            for (TiXmlElement* multiElement = root->FirstChildElement("multipart"); multiElement; multiElement = multiElement->NextSiblingElement("multipart")) {
                const std::string* partId = multiElement->Attribute(std::string("id"));
				if (partId) {
					obj->addMultiPartId(*partId);
				}
				for (TiXmlElement* multiRotation = multiElement->FirstChildElement("rotation"); multiRotation; multiRotation = multiRotation->NextSiblingElement("rotation")) {
					int rotation = 0;
					multiRotation->QueryIntAttribute("rot", &rotation);
					// relative coordinates which are used to position the object
					for (TiXmlElement* multiCoordinate = multiRotation->FirstChildElement("occupied_coord"); multiCoordinate; multiCoordinate = multiCoordinate->NextSiblingElement("occupied_coord")) {
						int x = 0;
						int y = 0;
						xRetVal = multiCoordinate->QueryValueAttribute("x", &x);
						yRetVal = multiCoordinate->QueryValueAttribute("y", &y);
						if (xRetVal == TIXML_SUCCESS && yRetVal == TIXML_SUCCESS) {
							int z = 0;
							multiCoordinate->QueryIntAttribute("z", &z);
							obj->addMultiPartCoordinate(rotation, ModelCoordinate(x, y, z));
						}
					}
				}
			}
			// loop over all image tags
			for (TiXmlElement* imageElement = root->FirstChildElement("image"); imageElement; imageElement = imageElement->NextSiblingElement("image")) {
				const std::string* sourceId = imageElement->Attribute(std::string("source"));

				if (sourceId) {
					std::string source = *namespaceId + ":" + *sourceId;
					if(!m_imageManager->exists(source)) {
						throw NotFound(source + " couldn't be found.");
					}
					ImagePtr imagePtr = m_imageManager->getPtr(source);
					int xOffset = 0;
					int success = imageElement->QueryIntAttribute("x_offset", &xOffset);

					if (success == TIXML_SUCCESS) {
						imagePtr->setXShift(xOffset);
					}

					int yOffset = 0;
					success = imageElement->QueryIntAttribute("y_offset", &yOffset);

					if (success == TIXML_SUCCESS) {
						imagePtr->setYShift(yOffset);
					}

					int direction = 0;
					success = imageElement->QueryIntAttribute("direction", &direction);

					if (success == TIXML_SUCCESS) {
						ObjectVisual* objVisual = obj->getVisual<ObjectVisual>();

						if (objVisual) {
							objVisual->addStaticImage(direction, static_cast<int32_t>(imagePtr->getHandle()));
						}
					}
				}
			}
		}
	}

	AtlasLoader* createDefaultAtlasLoader(Model* model, VFS* vfs, ImageManager* imageManager) {
		return new AtlasLoader(model, vfs, imageManager);
	}
}
