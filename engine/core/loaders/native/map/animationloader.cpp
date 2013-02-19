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
#include "vfs/fife_boost_filesystem.h"
#include "vfs/vfs.h"
#include "vfs/raw/rawdata.h"
#include "video/imagemanager.h"
#include "video/image.h"
#include "video/animation.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"

#include "animationloader.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
    static Logger _log(LM_NATIVE_LOADERS);

    AnimationLoader::AnimationLoader(VFS* vfs, ImageManager* imageManager)
    : m_vfs(vfs), m_imageManager(imageManager) {

    }

    bool AnimationLoader::isLoadable(const std::string& filename) {
        bfs::path animPath(filename);

        std::string animationFilename = animPath.string();

        try {
            RawData* data = m_vfs->open(animationFilename);

            if (data) {
                if (data->getDataLength() != 0) {
                    // TODO - this could be expanded to do more checks
                    TiXmlDocument doc;
                    doc.Parse(data->readString(data->getDataLength()).c_str());

                    if (doc.Error()) {
                        return false;
                    }
                }

                // done with data delete resource
                delete data;
                data = 0;
            }
        }
        catch (NotFound&) {
            return false;
        }

        return true;
    }

    AnimationPtr AnimationLoader::load(const std::string& filename) {
        bfs::path animPath(filename);

        std::string animationFilename = animPath.string();

        TiXmlDocument doc;

        AnimationPtr animation;

        try {
            RawData* data = m_vfs->open(animationFilename);

            if (data) {
                if (data->getDataLength() != 0) {
                    doc.Parse(data->readString(data->getDataLength()).c_str());

                    if (doc.Error()) {
                        return animation;
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

            return animation;
        }

        // if we get here then everything loaded properly
        // so we can just parse out the contents
        TiXmlElement* root = doc.RootElement();

        if (root) {
            animation.reset(new Animation());

            int animDelay = 0;
            root->QueryValueAttribute("delay", &animDelay);

            int animXoffset = 0;
            int animYoffset = 0;
            int action = -1;
            root->QueryValueAttribute("x_offset", &animXoffset);
            root->QueryValueAttribute("y_offset", &animYoffset);
            root->QueryValueAttribute("action", &action);

            for (TiXmlElement* frameElement = root->FirstChildElement("frame"); frameElement; frameElement = frameElement->NextSiblingElement("frame")) {
                if (animation) {
                    animation->setActionFrame(action);

                    const std::string* sourceId = frameElement->Attribute(std::string("source"));

                    if (sourceId) {
                        bfs::path framePath(filename);

                        if (HasParentPath(framePath)) {
							framePath = GetParentPath(framePath) / *sourceId;
						} else {
							framePath = bfs::path(*sourceId);
						}

						ImagePtr imagePtr;
						if(!m_imageManager->exists(framePath.string())) {
                        	imagePtr = m_imageManager->create(framePath.string());
						}
						else {
							imagePtr = m_imageManager->getPtr(framePath.string());
						}

                        if (imagePtr) {
                            int frameXoffset = 0;
                            int frameYoffset = 0;

                            int success = root->QueryValueAttribute("x_offset", &frameXoffset);

                            if (success == TIXML_SUCCESS) {
                                imagePtr->setXShift(frameXoffset);
                            }
                            else {
                                imagePtr->setXShift(animXoffset);
                            }

                            success = root->QueryValueAttribute("y_offset", &frameYoffset);

                            if (success == TIXML_SUCCESS) {
                                imagePtr->setYShift(frameYoffset);
                            }
                            else {
                                imagePtr->setYShift(animYoffset);
                            }

                            int frameDelay = 0;
                            success = root->QueryValueAttribute("delay", &frameDelay);

                            if (success == TIXML_SUCCESS) {
                                animation->addFrame(imagePtr, frameDelay);
                            }
                            else {
                                animation->addFrame(imagePtr, animDelay);
                            }
                        }
                    }
                }
            }
        }

        return animation;
    }
}
