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
#include "video/imagemanager.h"
#include "video/image.h"
#include "video/animation.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"

#include "animationloader.h"

namespace fs = boost::filesystem;

namespace FIFE {
    static Logger _log(LM_NATIVE_LOADERS);

    AnimationLoader::AnimationLoader(VFS* vfs, ImageManager* imageManager)
    : m_vfs(vfs), m_imageManager(imageManager) {

    }

    bool AnimationLoader::isLoadable(const std::string& filename) {
        fs::path animPath(filename);

        std::string animationFilename = animPath.file_string();

        try {
            RawData* data = m_vfs->open(animationFilename);

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
        }
        catch (NotFound&) {
            return false;
        }

        return true;
    }

    AnimationPtr AnimationLoader::load(const std::string& filename) {
        fs::path animPath(filename);

        std::string animationFilename = animPath.file_string();

        TiXmlDocument doc;

        AnimationPtr animation;

        try {
            RawData* data = m_vfs->open(animationFilename);

            if (data) {
                if (data->getDataLength() != 0) {
                    doc.Parse(reinterpret_cast<const char*>(&data->getDataInBytes()[0]));
                    //bool isFileLoaded = doc.LoadFile(animationFilename);
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
            int action = 0;
            root->QueryValueAttribute("x_offset", &animXoffset);
            root->QueryValueAttribute("y_offset", &animYoffset);
            root->QueryValueAttribute("action", &action);

            for (TiXmlElement* frameElement = root->FirstChildElement("frame"); frameElement; frameElement = frameElement->NextSiblingElement("frame")) {
                if (animation) {
                    animation->setActionFrame(action);

                    const std::string* sourceId = frameElement->Attribute(std::string("source"));

                    if (sourceId) {
                        std::string frameFile = *sourceId;

                        fs::path framePath(filename);

                        if (framePath.has_branch_path()) {
                            frameFile = framePath.branch_path().directory_string() + "/" + *sourceId;
                        }

                        framePath = fs::path(frameFile);

                        //ImagePtr imagePtr = m_imageManager->load(framePath.file_string());
                        ImagePtr imagePtr = m_imageManager->create(framePath.file_string());

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
