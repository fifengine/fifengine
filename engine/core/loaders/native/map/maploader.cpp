/***************************************************************************
*   Copyright (C) 2005-2009 by the FIFE team                              *
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
#include <string>
#include <vector>

// 3rd party includes
#include "boost/filesystem.hpp"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "ext/tinyxml/fife_tinyxml.h"
#include "model/model.h"
#include "model/structures/layer.h"
#include "model/structures/instance.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/modelcoords.h"
#include "model/metamodel/action.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/structures/rect.h"
#include "video/imagemanager.h"
#include "video/image.h"
#include "video/renderbackend.h"
#include "view/visual.h"
#include "view/camera.h"
#include "view/renderers/instancerenderer.h"
#include "util/base/stringutils.h"

#include "maploader.h"
#include "animationloader.h"

namespace fs = boost::filesystem;

namespace FIFE
{
    static Logger _log(LM_NATIVE_LOADERS);

	MapLoader::MapLoader(Model* model, VFS* vfs, ImageManager* imageManager, RenderBackend* renderBackend)
	: m_model(model), m_vfs(vfs), m_imageManager(imageManager), m_renderBackend(renderBackend),
	  m_loaderName("fife"), m_mapDirectory("") {
		
	}

    MapLoader::~MapLoader() {

    }

    Map* MapLoader::load(const std::string& filename) {
		Map* map = NULL;

		// reset percent done listener just in case
		// it has residual data from last load
		m_percentDoneListener.reset();

		fs::path mapPath(filename);
		
		if (mapPath.has_branch_path()) {
            if (mapPath.branch_path().directory_string() != m_mapDirectory) {
			    // save the directory where the map file is located
			    m_mapDirectory = mapPath.branch_path().directory_string();

                // create a new animation loader
                m_animationLoader.reset(new AnimationLoader(m_vfs, m_imageManager));
            }
		}

		TiXmlDocument mapFile;
        
        std::string mapFilename = mapPath.file_string();

        try {
            RawData* data = m_vfs->open(mapFilename);

            if (data) {
                if (data->getDataLength() != 0) {
                    mapFile.Parse(reinterpret_cast<const char*>(&data->getDataInBytes()[0]));

                    if (mapFile.Error()) {
                        return map;
                    }
                }

                // done with data delete resource
                delete data;
                data = 0;
            }
        }
        catch (NotFound& e)
        {
            FL_ERR(_log, e.what());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return map;
        }

        // if we get here then everything loaded properly
        // so we can just parse out the contents
		const TiXmlElement* root = mapFile.RootElement();

		if (root) {
			const std::string* loaderName = root->Attribute(std::string("loaderName"));
			
			if (loaderName) {
				m_loaderName = *loaderName;
			}

			int numElements = 0;
			int numElementsRetVal = root->QueryValueAttribute("elements", &numElements);
			m_percentDoneListener.setTotalNumberOfElements(numElements);

			const std::string* mapName = root->Attribute(std::string("id"));

			if (mapName) {
				try {
					map = m_model->createMap(*mapName);
				}
				catch (NameClash& e) {
					FL_ERR(_log, e.what());

                    // just rethrow to client
                    throw;
				}

				if (map) {
                    map->setFilename(mapFilename);

					std::string ns = "";
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
                            fs::path fullPath(m_mapDirectory);
                            fullPath /= directory;
                            loadImportDirectory(fullPath.directory_string());
                        }
                        else if (importFile) {
                            fs::path fullFilePath(file);
                            fs::path fullDirPath(directory);
                            if (importDir) {
                                fullDirPath = fs::path(m_mapDirectory);
                                fullDirPath /= directory;
                            }
                            else {
                                fullFilePath = fs::path(m_mapDirectory);
                                fullFilePath /= file;
                            }
                            loadImportFile(fullFilePath.file_string(), fullDirPath.directory_string());
                        }
					}

					// iterate over elements looking for layers
					for (const TiXmlElement* layerElement = root->FirstChildElement("layer"); layerElement; layerElement = layerElement->NextSiblingElement("layer")) {
						// defaults
						double xOffset = 0.0;
						double yOffset = 0.0;
						double xScale = 1.0;
						double yScale = 1.0;
						double rotation = 0.0;

						int xOffsetRetVal = layerElement->QueryValueAttribute("x_offset", &xOffset);
						int yOffsetRetVal = layerElement->QueryValueAttribute("y_offset", &yOffset);
						int xScaleRetVal = layerElement->QueryValueAttribute("x_scale", &xScale);
						int yScaleRetVal = layerElement->QueryValueAttribute("y_scale", &yScale);
						int rotationRetVal = layerElement->QueryValueAttribute("rotation", &rotation);

						const std::string* layerName = layerElement->Attribute(std::string("id"));
						const std::string* pathing = layerElement->Attribute(std::string("pathing"));
						const std::string* gridType = layerElement->Attribute(std::string("grid_type"));

						if (xOffsetRetVal == TIXML_SUCCESS &&
							yOffsetRetVal == TIXML_SUCCESS &&
							xScaleRetVal == TIXML_SUCCESS && 
							yScaleRetVal == TIXML_SUCCESS &&
							rotationRetVal == TIXML_SUCCESS &&
							layerName && 
							pathing && 
							gridType) {
							PathingStrategy pathStrategy = CELL_EDGES_ONLY;

							if ("cell_edges_and_diagonals" == *pathing) {
								pathStrategy = CELL_EDGES_AND_DIAGONALS;
							}
							else if ("freeform" == *pathing) {
								pathStrategy = FREEFORM;
							}

							CellGrid* grid = NULL;
							if (gridType) {
								grid = m_model->getCellGrid(*gridType);
							}
							else {
								grid = m_model->getCellGrid("square");
							}

							if (grid) {
								grid->setXShift(xOffset);
								grid->setXScale(xScale);
								grid->setYShift(yOffset);
								grid->setYScale(yScale);
								grid->setRotation(rotation);

								Layer *layer = NULL;
								try {
									layer = map->createLayer(*layerName, grid);
								}
								catch (NameClash&) {
									// TODO - handle exception
									assert(false);
								}

								if (layer) {
									layer->setPathingStrategy(pathStrategy);

									double curr_x = 0;
									double curr_y = 0;
									
									for (const TiXmlElement* instances = layerElement->FirstChildElement("instances"); instances; instances = instances->NextSiblingElement("instances")) {
										for (const TiXmlElement* instance = instances->FirstChildElement("i"); instance; instance = instance->NextSiblingElement("i")) {
											double x = 0;
											double y = 0;
											double z = 0;
											int r = 0;
											int stackpos = 0;

											const std::string* instanceId = instance->Attribute(std::string("id"));
											const std::string* objectId = instance->Attribute(std::string("o"));

											if (!objectId) {
												objectId = instance->Attribute(std::string("object"));
											}

											if (!objectId) {
												objectId = instance->Attribute(std::string("obj"));
											}

											const std::string* namespaceId = instance->Attribute(std::string("ns"));

											if (!namespaceId) {
												namespaceId = instance->Attribute(std::string("namespace"));
											}

											int xRetVal = instance->QueryValueAttribute("x", &x);
											int yRetVal = instance->QueryValueAttribute("y", &y);
											int zRetVal = instance->QueryValueAttribute("z", &z);
											int rRetVal = instance->QueryValueAttribute("r", &r);

											if (xRetVal == TIXML_SUCCESS) {
												curr_x = x;
											}
											else {
												x = ++curr_x;		
											}

											if (yRetVal == TIXML_SUCCESS) {
												curr_y = y;
											}
											else {
												y = curr_y;
											}

											if (rRetVal != TIXML_SUCCESS) {
												rRetVal = instance->QueryValueAttribute("rotation", &r);
											}

											int stackRetVal = instance->QueryValueAttribute("stackpos", &stackpos);

											if (objectId) {
												if (namespaceId) {
													ns = *namespaceId;
												}

												Object* object = m_model->getObject(*objectId, ns);

												if (object) {
													Instance* inst = NULL;
													if (instanceId) {
														inst = layer->createInstance(object, ExactModelCoordinate(x,y,z), *instanceId);
													}
													else {
														inst = layer->createInstance(object, ExactModelCoordinate(x,y,z));
													}

													if (inst) {
														if (rRetVal != TIXML_SUCCESS) {
															ObjectVisual* objVisual = object->getVisual<ObjectVisual>();
															std::vector<int> angles;
															objVisual->getStaticImageAngles(angles);
															if (!angles.empty()) {
																r = angles[0];
															}
														}

														inst->setRotation(r);

														InstanceVisual* instVisual = InstanceVisual::create(inst);
														
														if  (instVisual && (stackRetVal == TIXML_SUCCESS)) {
															instVisual->setStackPosition(stackpos);
														}

														if (object->getAction("default")) {
															Location target(layer);

															inst->act("default", target, true);
														}
													}
												}
											}

											// increment % done counter
											m_percentDoneListener.incrementCount();
										}
									}
								}
							}
						}

						// increment % done counter
						m_percentDoneListener.incrementCount();
					}

					for (const TiXmlElement* cameraElement = root->FirstChildElement("camera"); cameraElement; cameraElement = cameraElement->NextSiblingElement("camera")) {
						const std::string* cameraId = cameraElement->Attribute(std::string("id"));
						const std::string* refLayerId = cameraElement->Attribute(std::string("ref_layer_id"));
						
						int refCellWidth = 0;
						int refCellHeight = 0;
						int success = cameraElement->QueryIntAttribute("ref_cell_width", &refCellWidth);
						success &= cameraElement->QueryIntAttribute("ref_cell_height", &refCellHeight);

						if (cameraId && refLayerId && success == TIXML_SUCCESS) {
							double tilt = 0.0;
							double zoom = 1.0;
							double rotation = 0.0;
							cameraElement->QueryDoubleAttribute("tilt", &tilt);
							cameraElement->QueryDoubleAttribute("zoom", &zoom);
							cameraElement->QueryDoubleAttribute("rotation", &rotation);

							const std::string* viewport = cameraElement->Attribute(std::string("viewport"));

							Layer* layer = NULL;
							try {
								layer = map->getLayer(*refLayerId);
							}
							catch (NotFound&) {
								// TODO - handle exception
								assert(false);
							}

							Camera* cam = NULL;
							if (layer) {
								if (viewport) {
									// parse out the viewport parameters
									IntVector viewportParameters = tokenize(*viewport, ','); 
									
									// make sure the right number of viewport parameters were parsed
									if (viewportParameters.size() == 4) {
										Rect rect(viewportParameters[0], viewportParameters[1], 
													viewportParameters[2], viewportParameters[3]);

										try {
											cam = map->addCamera(*cameraId, layer, rect);
										}
										catch (NameClash&) {
											// TODO - handle exception
											assert(false);
										}
									}
								}
								else {
									Rect rect(0, 0, m_renderBackend->getScreenWidth(), m_renderBackend->getScreenHeight());

									try {
										cam = map->addCamera(*cameraId, layer, rect);
									}
									catch (NameClash&) {
										// TODO - handle exception
										assert(false);
									}
								}
							}

							if (cam) {
								cam->setCellImageDimensions(refCellWidth, refCellHeight);
								cam->setRotation(rotation);
								cam->setTilt(tilt);
								cam->setZoom(zoom);

                                // active instance renderer for camera
                                InstanceRenderer* instanceRenderer = InstanceRenderer::getInstance(cam);
                                if (instanceRenderer)
                                {
                                    instanceRenderer->activateAllLayers(map);
                                }
							}
						}

						// increment % done counter
						m_percentDoneListener.incrementCount();
					}
				}
			}
		}

		return map;
	}

    bool MapLoader::isLoadable(const std::string& filename) const {
		fs::path mapPath(filename);

		TiXmlDocument mapFile;

        std::string mapFilename = mapPath.file_string();

        try {
            RawData* data = m_vfs->open(mapFilename);

            if (data) {
                if (data->getDataLength() != 0) {
                    mapFile.Parse(reinterpret_cast<const char*>(&data->getDataInBytes()[0]));

                    if (mapFile.Error()) {
                        return false;
                    }

                    const TiXmlElement* root = mapFile.RootElement();

                    if (root) {
                        const std::string* loaderName = root->Attribute(std::string("loader"));

                        // if the file does not specify a loader but was opened and parsed
                        // correctly then we know we have a compatible extension so we will
                        // attempt to load it, if it does specify a loader then the loader
                        // name will be checked
                        if (!loaderName || (loaderName && *loaderName == getLoaderName())) {
                            return true;
                        }
                    }
                }

                // done with file delete the resource
                delete data;
                data = 0;
            }
        }
        catch (NotFound& e) {
            FL_ERR(_log, e.what());

            return false;
        }

        return false;
	}

    void MapLoader::loadImportFile(const std::string& file, const std::string& directory) {
        if (!file.empty()) {
            fs::path importFile(directory);
            importFile /= file;
            loadObjectFile(importFile.file_string());
        }
    }

    void MapLoader::loadImportDirectory(const std::string& directory) {
        if (!directory.empty()) {
            fs::path importDirectory(directory);
            loadObjectDirectory(importDirectory.file_string());
            m_importDirectories.push_back(importDirectory.file_string());
        }
    }

    std::vector<std::string> MapLoader::getImportDirectories() const {
        return m_importDirectories;
    }

    void MapLoader::addPercentDoneListener(PercentDoneListener* listener) {
        m_percentDoneListener.addListener(listener);
    }

	const std::string& MapLoader::getLoaderName() const {
		return m_loaderName;
	}

	void MapLoader::loadObjectFile(const std::string& file) {
		fs::path objectPath(file);

        TiXmlDocument objectFile;

        try {
            RawData* data = m_vfs->open(objectPath.file_string());

            if (data) {
                if (data->getDataLength() != 0) {
                    objectFile.Parse(reinterpret_cast<const char*>(&data->getDataInBytes()[0]));

                    if (objectFile.Error()) {
                        return;
                    }
                }

                // done with data delete resource
                delete data;
                data = 0;
            }
        }
        catch (NotFound& e)
        {
            FL_ERR(_log, e.what());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return;
        }

        // if we get here then loading the file went well
		TiXmlElement* root = objectFile.RootElement();

		if (root && root->ValueStr() == "object") {
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
					}
				}
			}

			if (obj) {
                obj->setFilename(file);
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

				// loop over all image tags
				for (TiXmlElement* imageElement = root->FirstChildElement("image"); imageElement; imageElement = imageElement->NextSiblingElement("image")) {
					const std::string* sourceId = imageElement->Attribute(std::string("source"));

					if (sourceId) {					
						std::string imageFile = *sourceId;

						fs::path imagePath(file);

						if (imagePath.has_branch_path()) {
							imageFile = imagePath.branch_path().string() + "/" + *sourceId;
						}

						imagePath = fs::path(imageFile);

                        ImagePtr imagePtr = m_imageManager->create(imagePath.file_string());

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

					// increment % done counter
					m_percentDoneListener.incrementCount();
				}

				for (TiXmlElement* actionElement = root->FirstChildElement("action"); actionElement; actionElement = actionElement->NextSiblingElement("action")) {
					const std::string* actionId = actionElement->Attribute(std::string("id"));

					if (actionId) {						
						Action* action = obj->createAction(*actionId);
						ActionVisual::create(action);

						for (TiXmlElement* animElement = actionElement->FirstChildElement("animation"); animElement; animElement = animElement->NextSiblingElement("animation")) {
							const std::string* sourceId = animElement->Attribute(std::string("atlas"));
							if(sourceId) {
								std::string atlasFile = *sourceId;
								fs::path atlasPath(file);
								if (atlasPath.has_branch_path()) {
									atlasFile = atlasPath.branch_path().string() + "/" + *sourceId;
								}
								atlasPath = fs::path(atlasFile);
								// we need to load this since its shared image
								ImagePtr atlasImgPtr = m_imageManager->load(atlasPath.file_string());
								atlasImgPtr->forceLoadInternal();

								// TODO: kto trzyma wskaznik na dzielony obraz? action?

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
									AnimationPtr animation(new Animation);

									int dir;
									dirElement->QueryIntAttribute("dir", &dir);

									int frames;
									int success;

									success = dirElement->QueryValueAttribute("frames", &frames);
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

									for (int iframe = 0; iframe < frames; ++iframe) {
										static char tmpBuf[64];
										sprintf(tmpBuf, "%03d:%04d", dir, iframe);

										std::string frameId = *objectId + ":" + *actionId + ":" + std::string(tmpBuf);
										ImagePtr framePtr = m_imageManager->create(frameId);
										Rect region(
											frameWidth * iframe, frameHeight * nDir, frameWidth, frameHeight
										);
										framePtr->useSharedImage(atlasImgPtr, region);
										framePtr->setXShift(xoffset);
										framePtr->setXShift(yoffset);
										animation->addFrame(framePtr, delay);
									}

									ActionVisual* actionVisual = action->getVisual<ActionVisual>();
									if(actionVisual) {
										actionVisual->addAnimation(dir, animation);
										action->setDuration(animation->getDuration());
									}
									++nDir;
								}

							} else {
								sourceId = animElement->Attribute(std::string("source"));
								if (sourceId) {
									std::string animFile = *sourceId;

									fs::path animPath(file);

									if (animPath.has_branch_path()) {
										animFile = animPath.branch_path().string() + "/" + *sourceId;
									}

									animPath = fs::path(animFile);

									AnimationPtr animation;
									if (m_animationLoader && m_animationLoader->isLoadable(animPath.file_string())) {
										animation = m_animationLoader->load(animPath.file_string());    
									}

									int direction = 0;
									int success = animElement->QueryIntAttribute("direction", &direction);

									if (action && animation) {
										ActionVisual* actionVisual = action->getVisual<ActionVisual>();

										if (actionVisual) {
											actionVisual->addAnimation(direction, animation);
											action->setDuration(animation->getDuration());
										}
									}
								}
							}

							// increment % done counter
							m_percentDoneListener.incrementCount();
						}
					}
				}
			}

			// increment % done counter
			m_percentDoneListener.incrementCount();
		}
	}

	void MapLoader::loadObjectDirectory(const std::string& directory) {
		fs::path fullDirPath(directory);

		std::set<std::string> files = m_vfs->listFiles(fullDirPath.directory_string());

        // load all xml files in the directory
		std::set<std::string>::iterator iter;
		for (iter = files.begin(); iter != files.end(); ++iter) {
			if (fs::extension(*iter) == ".xml") {
                fs::path objectFilePath(fullDirPath.directory_string());
                objectFilePath /= *iter;
				loadObjectFile(objectFilePath.file_string());
			}	
		}

		std::set<std::string> setNestedDirectories = m_vfs->listDirectories(fullDirPath.directory_string());

		// had to copy std::set contents to a vector to get around
		// the problem of std::remove not compiling because of a
		// problem with how set defines its iterators on gcc
		std::vector<std::string> nestedDirectories(setNestedDirectories.begin(), setNestedDirectories.end());

		// don't like this, but need to make sure to not include the .svn directory
		nestedDirectories.erase(std::remove(nestedDirectories.begin(), nestedDirectories.end(), ".svn"), nestedDirectories.end());

		std::vector<std::string>::iterator vecIter;
		for (vecIter = nestedDirectories.begin(); vecIter != nestedDirectories.end(); ++vecIter) {
			loadObjectDirectory(fullDirPath.directory_string() + *vecIter);
		}
	}

	MapLoader* createDefaultMapLoader(Model* model, VFS* vfs, ImageManager* imageManager, RenderBackend* renderBackend) {
		return (new MapLoader(model, vfs, imageManager, renderBackend));
	}
}
