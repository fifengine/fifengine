// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "animationloader.h"

// Standard C++ library includes
#include <cassert>
#include <memory>
#include <string>
#include <vector>

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"
#include "util/xml/xmlhelper.h"
#include "vfs/filesystem.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "video/animation.h"
#include "video/animationmanager.h"
#include "video/image.h"
#include "video/imagemanager.h"

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_NATIVE_LOADERS);
            return log;
        }
    } // namespace

    AnimationLoader::AnimationLoader(VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager) :
        m_vfs(vfs), m_imageManager(imageManager), m_animationManager(animationManager)
    {
    }

    bool AnimationLoader::isLoadable(std::string const & filename)
    {
        fs::path const animPath(filename);

        std::string const animationFilename = animPath.string();
        XML::Document animFile;

        try {
            auto data = m_vfs->open(animationFilename);

            if (data != nullptr) {
                if (data->getDataLength() != 0) {
                    // TODO - this could be expanded to do more checks
                    std::string const xml = data->readString(data->getDataLength());

                    if (!XML::Parse(animFile, xml)) {
                        return false;
                    }
                }
            }
        } catch (NotFound&) {
            return false;
        }

        // if we get here then loading the file went well
        XML::Element* root = animFile.RootElement();

        if (XML::HasName(root, "assets")) {
            if (root->FirstChildElement("animation") != nullptr) {
                return true;
            }
        }

        if (XML::HasName(root, "animation")) {
            return true;
        }

        return false;
    }

    AnimationPtr AnimationLoader::load(std::string const & filename)
    {
        std::string const & animationFilename = filename;

        XML::Document doc;

        AnimationPtr animation;

        try {
            auto data = m_vfs->open(animationFilename);

            if (data != nullptr) {
                if (data->getDataLength() != 0) {
                    std::string const xml = data->readString(data->getDataLength());

                    if (!XML::Parse(doc, xml)) {
                        return animation;
                    }
                }
            }
        } catch (NotFound& e) {
            FL_ERR(_log(), e.what());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return animation;
        }

        // if we get here then everything loaded properly
        // so we can just parse out the contents
        XML::Element* root = doc.RootElement();

        XML::Element* animElem = nullptr;
        if (XML::HasName(root, "assets")) {
            animElem = root->FirstChildElement("animation");
        } else if (XML::HasName(root, "animation")) {
            animElem = root;
        }
        if (animElem != nullptr) {
            animation = loadAnimation(filename, animElem);
        }

        return animation;
    }

    std::vector<AnimationPtr> AnimationLoader::loadMultiple(std::string const & filename)
    {
        fs::path const animPath(filename);

        std::string const animationFile = animPath.string();

        XML::Document doc;

        std::vector<AnimationPtr> animationVector;

        try {
            auto data = m_vfs->open(animationFile);

            if (data != nullptr) {
                if (data->getDataLength() != 0) {
                    std::string const xml = data->readString(data->getDataLength());

                    if (!XML::Parse(doc, xml)) {
                        return animationVector;
                    }
                }
            }
        } catch (NotFound& e) {
            FL_ERR(_log(), e.what());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return animationVector;
        }

        // if we get here then everything loaded properly
        // so we can just parse out the contents
        XML::Element* root = doc.RootElement();

        if (XML::HasName(root, "assets")) {
            for (XML::Element* animationElem = root->FirstChildElement("animation"); animationElem != nullptr;
                 animationElem               = animationElem->NextSiblingElement("animation")) {
                AnimationPtr const animation = loadAnimation(filename, animationElem);
                if (animation) {
                    animationVector.push_back(animation);
                }
            }
        }

        return animationVector;
    }

    AnimationPtr AnimationLoader::loadAnimation(std::string const & filename, tinyxml2::XMLElement* animationElem)
    {
        AnimationPtr animation;
        if (animationElem == nullptr) {
            return animation;
        }

        fs::path animPath(filename);
        std::string const animationFile = GetFilenameFromPath(animPath);

        bool alreadyLoaded = false;
        // first try to use the id, if no id exists it use the filename as fallback
        char const * animationId = XML::Attribute(animationElem, "id");
        if (animationId != nullptr) {
            if (!m_animationManager->exists(animationId)) {
                animation = m_animationManager->create(animationId);
            } else {
                animation     = m_animationManager->getPtr(animationId);
                alreadyLoaded = animation->getFrameCount() != 0;
            }
        } else {
            if (HasParentPath(animPath)) {
                animPath = GetParentPath(animPath) / animationFile;
            } else {
                animPath = fs::path(animationFile);
            }
            if (!m_animationManager->exists(animPath.string())) {
                animation = m_animationManager->create(animPath.string());
            } else {
                animation     = m_animationManager->getPtr(animPath.string());
                alreadyLoaded = animation->getFrameCount() != 0;
            }
        }

        if (alreadyLoaded) {
            return animation;
        }

        int direction   = 0;
        int actionFrame = -1;
        int animDelay   = 0;
        int animXoffset = 0;
        int animYoffset = 0;

        int success = XML::QueryAttribute(animationElem, "direction", &direction);
        if (success == XML::SUCCESS) {
            assert(direction >= 0);
            animation->setDirection(static_cast<uint32_t>(direction));
        }
        success = XML::QueryAttribute(animationElem, "action_frame", &actionFrame);
        if (success == XML::SUCCESS) {
            animation->setActionFrame(actionFrame);
        }
        XML::QueryAttribute(animationElem, "delay", &animDelay);
        XML::QueryAttribute(animationElem, "x_offset", &animXoffset);
        XML::QueryAttribute(animationElem, "y_offset", &animYoffset);

        // Check for atlas-based animation (shared sub-images from a sprite sheet)
        char const * atlasSource = XML::Attribute(animationElem, "atlas");
        if (atlasSource != nullptr) {
            fs::path atlasPath(filename);
            if (HasParentPath(atlasPath)) {
                atlasPath = GetParentPath(atlasPath) / atlasSource;
            } else {
                atlasPath = fs::path(atlasSource);
            }

            ImagePtr atlasImgPtr;
            if (!m_imageManager->exists(atlasPath.string())) {
                atlasImgPtr = m_imageManager->create(atlasPath.string());
            } else {
                atlasImgPtr = m_imageManager->getPtr(atlasPath.string());
            }

            int animFrameWidth  = 0;
            int animFrameHeight = 0;
            XML::QueryAttribute(animationElem, "width", &animFrameWidth);
            XML::QueryAttribute(animationElem, "height", &animFrameHeight);

            for (XML::Element* frameElement = animationElem->FirstChildElement("frame"); frameElement != nullptr;
                 frameElement               = frameElement->NextSiblingElement("frame")) {
                char const * sourceId = XML::Attribute(frameElement, "source");
                if (sourceId == nullptr) {
                    continue;
                }

                int xpos = 0;
                int ypos = 0;
                int fw   = animFrameWidth;
                int fh   = animFrameHeight;
                XML::QueryAttribute(frameElement, "xpos", &xpos);
                XML::QueryAttribute(frameElement, "ypos", &ypos);
                XML::QueryAttribute(frameElement, "width", &fw);
                XML::QueryAttribute(frameElement, "height", &fh);

                ImagePtr imagePtr;
                if (!m_imageManager->exists(sourceId)) {
                    imagePtr = m_imageManager->create(sourceId);
                    imagePtr->useSharedImage(atlasImgPtr, Rect(xpos, ypos, fw, fh));
                } else {
                    imagePtr = m_imageManager->getPtr(sourceId);
                }

                if (imagePtr) {
                    int frameXoffset = 0;
                    success          = XML::QueryAttribute(frameElement, "x_offset", &frameXoffset);
                    if (success == XML::SUCCESS) {
                        imagePtr->setXShift(frameXoffset);
                    } else {
                        imagePtr->setXShift(animXoffset);
                    }

                    int frameYoffset = 0;
                    success          = XML::QueryAttribute(frameElement, "y_offset", &frameYoffset);
                    if (success == XML::SUCCESS) {
                        imagePtr->setYShift(frameYoffset);
                    } else {
                        imagePtr->setYShift(animYoffset);
                    }

                    int frameDelay = 0;
                    success        = XML::QueryAttribute(frameElement, "delay", &frameDelay);
                    if (success == XML::SUCCESS) {
                        animation->addFrame(imagePtr, static_cast<uint32_t>(frameDelay));
                    } else {
                        animation->addFrame(imagePtr, static_cast<uint32_t>(animDelay));
                    }
                }
            }
        } else {
            for (XML::Element* frameElement = animationElem->FirstChildElement("frame"); frameElement != nullptr;
                 frameElement               = frameElement->NextSiblingElement("frame")) {
                char const * sourceId = XML::Attribute(frameElement, "source");
                if (sourceId != nullptr) {
                    fs::path framePath(filename);

                    if (HasParentPath(framePath)) {
                        framePath = GetParentPath(framePath) / sourceId;
                        if (!fs::exists(framePath)) {
                            framePath = fs::path(sourceId);
                        }
                    } else {
                        framePath = fs::path(sourceId);
                    }

                    ImagePtr imagePtr;
                    if (!m_imageManager->exists(framePath.string())) {
                        imagePtr = m_imageManager->create(framePath.string());
                    } else {
                        imagePtr = m_imageManager->getPtr(framePath.string());
                    }

                    if (imagePtr) {
                        int frameXoffset = 0;
                        success          = XML::QueryAttribute(frameElement, "x_offset", &frameXoffset);
                        if (success == XML::SUCCESS) {
                            imagePtr->setXShift(frameXoffset);
                        } else {
                            imagePtr->setXShift(animXoffset);
                        }

                        int frameYoffset = 0;
                        success          = XML::QueryAttribute(frameElement, "y_offset", &frameYoffset);
                        if (success == XML::SUCCESS) {
                            imagePtr->setYShift(frameYoffset);
                        } else {
                            imagePtr->setYShift(animYoffset);
                        }

                        int frameDelay = 0;
                        success        = XML::QueryAttribute(frameElement, "delay", &frameDelay);
                        if (success == XML::SUCCESS) {
                            animation->addFrame(imagePtr, static_cast<uint32_t>(frameDelay));
                        } else {
                            animation->addFrame(imagePtr, static_cast<uint32_t>(animDelay));
                        }
                    }
                }
            }
        }

        return animation;
    }
} // namespace FIFE
