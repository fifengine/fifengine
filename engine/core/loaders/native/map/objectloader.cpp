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

// Standard C++ library includes

// 3rd party library includes
#include <tinyxml.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "model/model.h"
#include "model/metamodel/object.h"
#include "model/metamodel/action.h"
#include "vfs/fife_boost_filesystem.h"
#include "vfs/vfs.h"
#include "vfs/raw/rawdata.h"
#include "view/visual.h"
#include "video/imagemanager.h"
#include "video/animationmanager.h"

#include "atlasloader.h"
#include "objectloader.h"
#include "animationloader.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_NATIVE_LOADERS);

	ObjectLoader::ObjectLoader(Model* model, VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager, const AnimationLoaderPtr& animationLoader, const AtlasLoaderPtr& atlasLoader)
	: m_model(model), m_vfs(vfs), m_imageManager(imageManager), m_animationManager(animationManager) {
		assert(m_model && m_vfs && m_imageManager && m_animationManager);

		if (animationLoader) {
			m_animationLoader = animationLoader;
		}
		else {
			m_animationLoader.reset(new AnimationLoader(m_vfs, m_imageManager, m_animationManager));
		}

		if (atlasLoader) {
			m_atlasLoader = atlasLoader;
		}
		else {
			m_atlasLoader.reset(new AtlasLoader(m_model, m_vfs, m_imageManager, m_animationManager));
		}
	}

	ObjectLoader::~ObjectLoader() {

	}

	void ObjectLoader::setAnimationLoader(const AnimationLoaderPtr& animationLoader) {
		assert(animationLoader);

		m_animationLoader = animationLoader;
	}

	AnimationLoaderPtr ObjectLoader::getAnimationLoader() {
		return m_animationLoader;
	}

	void ObjectLoader::setAtlasLoader(const AtlasLoaderPtr& atlasLoader) {
		assert(atlasLoader);

		m_atlasLoader = atlasLoader;
	}

	AtlasLoaderPtr ObjectLoader::getAtlasLoader() {
		return m_atlasLoader;
	}

	bool ObjectLoader::isLoadable(const std::string& filename) const {
		bfs::path objectPath(filename);

		TiXmlDocument objectFile;

		try {
			RawData* data = m_vfs->open(objectPath.string());

			if (data) {
				if (data->getDataLength() != 0) {
					objectFile.Parse(data->readString(data->getDataLength()).c_str());

					if (objectFile.Error()) {
						 std::ostringstream oss;
						oss << " Failed to load"
							<< objectPath.string()
							<< " : " << __FILE__
							<< " [" << __LINE__ << "]"
							<< std::endl;
						FL_ERR(_log, oss.str());

						return false;
					}
				}
				else {
					std::ostringstream oss;
					oss << " Failed to load"
						<< objectPath.string()
						<< " : " << __FILE__
						<< " [" << __LINE__ << "]"
						<< std::endl;
					FL_ERR(_log, oss.str());

					return false;
				}

				// done with data delete resource
				delete data;
				data = 0;
			}
			else {
				std::ostringstream oss;
				oss << " Failed to load"
					<< objectPath.string()
					<< " : " << __FILE__
					<< " [" << __LINE__ << "]"
					<< std::endl;
				FL_ERR(_log, oss.str());

				return false;
			}
		}
		catch (NotFound&) {
			std::ostringstream oss;
			oss << " Failed to load"
				<< objectPath.string()
				<< " : " << __FILE__
				<< " [" << __LINE__ << "]"
				<< std::endl;
			FL_ERR(_log, oss.str());

			// TODO - should we abort here
			//        or rethrow the exception
			//        or just keep going

			return false;
		}

		// if we get here then loading the file went well
		TiXmlElement* root = objectFile.RootElement();

		if (root && root->ValueStr() == "assets") {
			if (root->FirstChildElement("object")) {
				return true;
			}
		}
			
		return false;
	}

	void ObjectLoader::load(const std::string& filename) {
		bfs::path objectPath(filename);

		TiXmlDocument objectFile;

		try {
			RawData* data = m_vfs->open(objectPath.string());

			if (data) {
				if (data->getDataLength() != 0) {
					objectFile.Parse(data->readString(data->getDataLength()).c_str());

					if (objectFile.Error()) {
						return;
					}
				}

				// done with data delete resource
				delete data;
				data = 0;
			}
		}
		catch (NotFound&) {
			std::ostringstream oss;
			oss << " Failed to load"
				<< objectPath.string()
				<< " : " << __FILE__
				<< " [" << __LINE__ << "]"
				<< std::endl;
			FL_ERR(_log, oss.str());

			// TODO - should we abort here
			//        or rethrow the exception
			//        or just keep going

			return;
		}
		std::string objectDirectory = "";
		if (HasParentPath(objectPath)) {
			objectDirectory = GetParentPath(objectPath).string();
		}

		// if we get here then loading the file went well
		TiXmlElement* root = objectFile.RootElement();
		if (root) {
			for (const TiXmlElement *importElement = root->FirstChildElement("import"); importElement; importElement = importElement->NextSiblingElement("import")) {
				const std::string* importDir = importElement->Attribute(std::string("dir"));
				const std::string* importFile = importElement->Attribute(std::string("file"));

				std::string directory = "";
				if (importDir) {
					directory = *importDir;
				}

				std::string file = "";
				if (importFile) {
					file = *importFile;
				}

				if (importDir && !importFile) {
					bfs::path fullPath(objectDirectory);
					fullPath /= directory;
					loadImportDirectory(fullPath.string());
				} else if (importFile) {
					bfs::path fullFilePath(file);
					bfs::path fullDirPath(directory);
					if (importDir) {
						fullDirPath = bfs::path(objectDirectory);
						fullDirPath /= directory;
					} else {
						fullFilePath = bfs::path(objectDirectory);
						fullFilePath /= file;
					}
					loadImportFile(fullFilePath.string(), fullDirPath.string());
				}
			}
		}

		if (root && root->ValueStr() == "assets") {
			for (TiXmlElement* objectElem = root->FirstChildElement("object"); objectElem; objectElem = objectElem->NextSiblingElement("object")) {
				const std::string* objectId = objectElem->Attribute(std::string("id"));
				const std::string* namespaceId = objectElem->Attribute(std::string("namespace"));

				Object* obj = NULL;
				if (objectId && namespaceId) {
					const std::string* parentId = objectElem->Attribute(std::string("parent"));

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
					} else {
						// this will make sure the object has not already been loaded
						if (m_model->getObject(*objectId, *namespaceId) == NULL) {
							try {
								obj = m_model->createObject(*objectId, *namespaceId);
							}
							catch (NameClash &e) {
								FL_ERR(_log, e.what());

								// TODO - handle exception
								assert(false);
							}
						}
					}
				}

				if (obj) {
					obj->setFilename(objectPath.string());
					ObjectVisual::create(obj);

					int isBlocking = 0;
					objectElem->QueryIntAttribute("blocking", &isBlocking);
					obj->setBlocking(isBlocking!=0);

					int isStatic = 0;
					objectElem->QueryIntAttribute("static", &isStatic);
					obj->setStatic(isStatic!=0);

					const std::string* pather = objectElem->Attribute(std::string("pather"));

					if (pather) {
						obj->setPather(m_model->getPather(*pather));
					}
					else {
						obj->setPather(m_model->getPather("RoutePather"));
					}
				
					const std::string* costId = objectElem->Attribute(std::string("cost_id"));
					if (costId) {
						obj->setCostId(*costId);
						double cost = 1.0;
						int success = objectElem->QueryDoubleAttribute("cost", &cost);
						if (success == TIXML_SUCCESS) {
							obj->setCost(cost);
						}
					}
				
					const std::string* areaId = objectElem->Attribute(std::string("area_id"));
					if (areaId) {
						obj->setArea(*areaId);
					}

					double speed = 1.0;
					int success = root->QueryDoubleAttribute("speed", &speed);
					if (success == TIXML_SUCCESS) {
						obj->setSpeed(speed);
					}

					// loop over all walkable areas
					for (TiXmlElement* walkableElement = objectElem->FirstChildElement("walkable_area"); walkableElement; walkableElement = walkableElement->NextSiblingElement("walkable_area")) {
						const std::string* walkableId = walkableElement->Attribute(std::string("id"));
						if (walkableId) {
							obj->addWalkableArea(*walkableId);
						}
					}

					int cellStack = 0;
					objectElem->QueryIntAttribute("cellstack", &cellStack);
					obj->setCellStackPosition(cellStack);
				
					double ax = 0;
					double ay = 0;
					double az = 0;

					int xRetVal = objectElem->QueryValueAttribute("anchor_x", &ax);
					int yRetVal = objectElem->QueryValueAttribute("anchor_y", &ay);
					if (xRetVal == TIXML_SUCCESS && yRetVal == TIXML_SUCCESS) {
						obj->setRotationAnchor(ExactModelCoordinate(ax, ay, az));
					}

					int isRestrictedRotation = 0;
					objectElem->QueryIntAttribute("restricted_rotation", &isRestrictedRotation);
					obj->setRestrictedRotation(isRestrictedRotation!=0);

					int zStep = 0;
					int zRetVal = objectElem->QueryIntAttribute("z_step_limit", &zStep);
					if (zRetVal == TIXML_SUCCESS) {
						obj->setZStepRange(zStep);
					}

					// loop over all multi parts
					for (TiXmlElement* multiElement = objectElem->FirstChildElement("multipart"); multiElement; multiElement = multiElement->NextSiblingElement("multipart")) {
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
					for (TiXmlElement* imageElement = objectElem->FirstChildElement("image"); imageElement; imageElement = imageElement->NextSiblingElement("image")) {
						const std::string* sourceId = imageElement->Attribute(std::string("source"));

						if (sourceId) {
							bfs::path imagePath(filename);

							if (HasParentPath(imagePath)) {
								imagePath = GetParentPath(imagePath) / *sourceId;
							} else {
								imagePath = bfs::path(*sourceId);
							}

							if (!bfs::exists(imagePath)) {
								imagePath= bfs::path(*sourceId);
							}

							ImagePtr imagePtr;
							if(!m_imageManager->exists(imagePath.string())) {
								imagePtr = m_imageManager->create(imagePath.string());
							}
							else {
								imagePtr = m_imageManager->getPtr(imagePath.string());
							}

							if (imagePtr) {
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

					for (TiXmlElement* actionElement = objectElem->FirstChildElement("action"); actionElement; actionElement = actionElement->NextSiblingElement("action")) {
						const std::string* actionId = actionElement->Attribute(std::string("id"));
						
						if (actionId) {
							int isDefault = 0;
							actionElement->QueryIntAttribute("default", &isDefault);
							Action* action = obj->createAction(*actionId, (isDefault != 0));
							ActionVisual::create(action);

							for (TiXmlElement* animElement = actionElement->FirstChildElement("animation"); animElement; animElement = animElement->NextSiblingElement("animation")) {
								// Fetch already created animation
								const std::string* animationId = animElement->Attribute(std::string("animation_id"));
								if (animationId) {
									AnimationPtr animation = m_animationManager->getPtr(*animationId);
									if (animation) {
										ActionVisual* actionVisual = action->getVisual<ActionVisual>();
										if (actionVisual) {
											actionVisual->addAnimation(animation->getDirection(), animation);
											action->setDuration(animation->getDuration());
											continue;
										}
									}
								}

								// Create animated spritesheet
								const std::string* sourceId = animElement->Attribute(std::string("atlas"));
								if (sourceId) {
									bfs::path atlasPath(filename);

									if (HasParentPath(atlasPath)) {
										atlasPath = GetParentPath(atlasPath) / *sourceId;
									} else {
										atlasPath = bfs::path(*sourceId);
									}

									ImagePtr atlasImgPtr;
									// we need to load this since its shared image
									if (!m_imageManager->exists(atlasPath.string())) {
										atlasImgPtr = m_imageManager->create(atlasPath.string());
									} else {
										atlasImgPtr = m_imageManager->getPtr(atlasPath.string());
									}

									int animFrames = 0;
									int animDelay = 0;
									int animXoffset = 0;
									int animYoffset = 0;
									int frameWidth = 0;
									int frameHeight = 0;

									animElement->QueryValueAttribute("width", &frameWidth);
									animElement->QueryValueAttribute("height", &frameHeight);
									animElement->QueryValueAttribute("frames", &animFrames);
									animElement->QueryValueAttribute("delay", &animDelay);
									animElement->QueryValueAttribute("x_offset", &animXoffset);
									animElement->QueryValueAttribute("y_offset", &animYoffset);
									int nDir = 0;

									for (TiXmlElement* dirElement = animElement->FirstChildElement("direction");
										dirElement; dirElement = dirElement->NextSiblingElement("direction")) {
											int dir;
											dirElement->QueryIntAttribute("dir", &dir);

											static char tmp[64];
											snprintf(tmp, 64, "%03d", dir);
											std::string aniId = *objectId + ":" + *actionId + ":" + std::string(tmp);
											AnimationPtr animation = m_animationManager->get(aniId);

											int frames;
											int success = dirElement->QueryValueAttribute("frames", &frames);
											if(success != TIXML_SUCCESS) {
												frames = animFrames;
											}

											int delay;
											success = dirElement->QueryValueAttribute("delay", &delay);
											if(success != TIXML_SUCCESS) {
												delay = animDelay;
											}

											int xoffset;
											success = dirElement->QueryValueAttribute("x_offset", &xoffset);
											if(success != TIXML_SUCCESS) {
												xoffset = animXoffset;
											}

											int yoffset;
											success = dirElement->QueryValueAttribute("y_offset", &yoffset);
											if(success != TIXML_SUCCESS) {
												yoffset = animYoffset;
											}

											int action_frame;
											success = dirElement->QueryValueAttribute("action_frame", &action_frame);
											if(success == TIXML_SUCCESS) {
												animation->setActionFrame(action_frame);
											}

											for (int iframe = 0; iframe < frames; ++iframe) {
												static char tmpBuf[64];
												snprintf(tmpBuf, 64, "%03d:%04d", dir, iframe);

												std::string frameId = *objectId + ":" + *actionId + ":" + std::string(tmpBuf);
												Rect region(frameWidth * iframe, frameHeight * nDir, frameWidth, frameHeight);
												ImagePtr framePtr;
												if (!m_imageManager->exists(frameId)) {
													framePtr = m_imageManager->create(frameId);
													framePtr->useSharedImage(atlasImgPtr, region);
													framePtr->setXShift(xoffset);
													framePtr->setYShift(yoffset);
												} else {
													framePtr = m_imageManager->getPtr(frameId);
												}
												animation->addFrame(framePtr, delay);
											}

											ActionVisual* actionVisual = action->getVisual<ActionVisual>();
											if(actionVisual) {
												actionVisual->addAnimation(dir, animation);
												action->setDuration(animation->getDuration());
											}
											++nDir;
									}
									continue;
								}

								// Load animation.xml with frames
								sourceId = animElement->Attribute(std::string("source"));
								if (sourceId) {
									int direction = 0;
									int success = animElement->QueryValueAttribute("direction", &direction);
									
									bfs::path animPath(filename);

									if (HasParentPath(animPath)) {
										animPath = GetParentPath(animPath) / *sourceId;
									} else {
										animPath = bfs::path(*sourceId);
									}

									AnimationPtr animation;
									if (m_animationLoader && m_animationLoader->isLoadable(animPath.string())) {
										animation = m_animationLoader->load(animPath.string());
									}

									if (action && animation) {
										if (success != TIXML_SUCCESS) {
											direction = animation->getDirection();
										}
										ActionVisual* actionVisual = action->getVisual<ActionVisual>();
										if (actionVisual) {
											actionVisual->addAnimation(direction, animation);
											action->setDuration(animation->getDuration());
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void ObjectLoader::loadImportFile(const std::string& file, const std::string& directory) {
		if (!file.empty()) {
			bfs::path importFilePath(directory);
			importFilePath /= file;

			std::string importFileString = importFilePath.string();
			if (m_atlasLoader && m_atlasLoader->isLoadable(importFileString)) {
				m_atlasLoader->loadMultiple(importFileString);
			}
			if (m_animationLoader && m_animationLoader->isLoadable(importFileString)) {
				m_animationLoader->loadMultiple(importFileString);
			}
			if (isLoadable(importFileString)) {
				load(importFileString);
			}
		}
	}

	void ObjectLoader::loadImportDirectory(const std::string& directory) {
		if (!directory.empty()) {
			bfs::path importDirectory(directory);
			std::string importDirectoryString = importDirectory.string();

			std::set<std::string> files = m_vfs->listFiles(importDirectoryString);

			// load all xml files in the directory
			std::set<std::string>::iterator iter;
			for (iter = files.begin(); iter != files.end(); ++iter) {
				// TODO - vtchill - may need a way to allow clients to load things other
				// than .xml and .zip files
				std::string ext = bfs::extension(*iter);
				if (ext == ".xml" || ext == ".zip") {
					loadImportFile(*iter, importDirectoryString);
				}
			}

			std::set<std::string> nestedDirectories = m_vfs->listDirectories(importDirectoryString);
			for (iter  = nestedDirectories.begin(); iter != nestedDirectories.end(); ++iter) {
				// do not attempt to load anything from a .svn directory
				if ((*iter).find(".svn") == std::string::npos) {
					loadImportDirectory(importDirectoryString + "/" + *iter);
				}
			}
		}
	}
}
