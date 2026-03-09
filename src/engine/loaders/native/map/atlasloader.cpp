// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdio>
#include <string>
#include <vector>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/model.h"
#include "model/structures/layer.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"
#include "util/xml/xmlhelper.h"
#include "vfs/filesystem.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "video/animationmanager.h"
#include "view/visual.h"

#include "atlasloader.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_NATIVE_LOADERS);

    size_t Atlas::getImageCount() const
    {
        return m_subimages.size();
    }

    ImagePtr& Atlas::getPackedImage()
    {
        return m_image;
    }

    ImagePtr Atlas::getImage(const std::string& id)
    {
        auto iter = m_subimages.find(id);
        if (iter == m_subimages.end()) {
            return {};
        }
        return iter->second.image;
    }

    ImagePtr Atlas::getImage(uint32_t index)
    {
        if (index > getImageCount()) {
            return {};
        }

        auto iter = m_subimages.begin();
        for (uint32_t i = 0; i < index; ++i, ++iter) {
            ;
        }
        return iter->second.image;
    }

    bool Atlas::addImage(const std::string& imagename, const AtlasData& data)
    {
        return m_subimages.insert(std::pair<std::string, AtlasData>(imagename, data)).second;
    }

    void Atlas::setPackedImage(const ImagePtr& image)
    {
        m_image = image;
    }

    const std::string& Atlas::getName() const
    {
        return m_name;
    }

    AtlasLoader::AtlasLoader(Model* model, VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager) :
        m_model(model), m_vfs(vfs), m_imageManager(imageManager), m_animationManager(animationManager)
    {
    }

    AtlasLoader::~AtlasLoader() = default;

    bool AtlasLoader::isLoadable(const std::string& filename)
    {
        fs::path atlasPath(filename);
        std::string atlasFilename = atlasPath.string();
        XML::Document atlasFile;

        try {
            RawData* data = m_vfs->open(atlasFilename);

            if (data != nullptr) {
                if (data->getDataLength() != 0) {
                    const std::string xml = data->readString(data->getDataLength());

                    if (!XML::Parse(atlasFile, xml)) {
                        delete data;
                        return false;
                    }
                } else {
                    delete data;
                    return false;
                }

                // done with data delete resource
                delete data;
                data = nullptr;
            }
        } catch (NotFound&) {
            return false;
        }

        // if we get here then loading the file went well
        XML::Element* root = atlasFile.RootElement();

        if (XML::HasName(root, "assets")) {
            if (root->FirstChildElement("atlas") != nullptr) {
                return true;
            }
        }

        return false;
    }

    AtlasPtr AtlasLoader::load(const std::string& filename)
    {
        fs::path atlasPath(filename);
        fs::path atlasPathDirectory;
        std::string atlasFilename = atlasPath.string();

        if (HasParentPath(atlasPath)) {
            // save the directory where the atlas file is located
            atlasPathDirectory = GetParentPath(atlasPath);
        }

        XML::Document doc;
        AtlasPtr atlas;

        try {
            RawData* data = m_vfs->open(atlasFilename);

            if (data != nullptr) {
                if (data->getDataLength() != 0) {
                    const std::string xml = data->readString(data->getDataLength());

                    if (!XML::Parse(doc, xml)) {
                        delete data;
                        return atlas;
                    }

                    // done with data delete resource
                    delete data;
                    data = nullptr;
                }
            }
        } catch (NotFound& e) {
            FL_ERR(_log, e.what());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return atlas;
        }

        // if we get here then everything loaded properly
        // so we can just parse out the contents
        XML::Element* root = doc.RootElement();

        if (XML::HasName(root, "assets")) {
            atlas = loadAtlas(filename, root->FirstChildElement("atlas"));
        }

        return atlas;
    }

    std::vector<AtlasPtr> AtlasLoader::loadMultiple(const std::string& filename)
    {
        fs::path atlasPath(filename);
        fs::path atlasPathDirectory;
        std::string atlasFilename = atlasPath.string();

        if (HasParentPath(atlasPath)) {
            // save the directory where the atlas file is located
            atlasPathDirectory = GetParentPath(atlasPath);
        }

        XML::Document doc;
        std::vector<AtlasPtr> atlasVector;

        try {
            RawData* data = m_vfs->open(atlasFilename);

            if (data != nullptr) {
                if (data->getDataLength() != 0) {
                    const std::string xml = data->readString(data->getDataLength());

                    if (!XML::Parse(doc, xml)) {
                        delete data;
                        return atlasVector;
                    }

                    // done with data delete resource
                    delete data;
                    data = nullptr;
                }
            }
        } catch (NotFound& e) {
            FL_ERR(_log, e.what());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return atlasVector;
        }

        // if we get here then everything loaded properly
        // so we can just parse out the contents
        XML::Element* root = doc.RootElement();

        if (XML::HasName(root, "assets")) {
            for (XML::Element* atlasElem = root->FirstChildElement("atlas"); atlasElem != nullptr;
                 atlasElem               = atlasElem->NextSiblingElement("atlas")) {
                AtlasPtr atlas = loadAtlas(filename, atlasElem);
                if (atlas) {
                    atlasVector.push_back(atlas);
                }
            }
        }

        return atlasVector;
    }

    AtlasPtr AtlasLoader::loadAtlas(const std::string& filename, tinyxml2::XMLElement* atlasElem)
    {
        AtlasPtr atlas;
        if (atlasElem == nullptr) {
            return atlas;
        }

        const char* atlasSource = XML::Attribute(atlasElem, "source");
        if (atlasSource != nullptr) {
            const char* atlasId = XML::Attribute(atlasElem, "id");

            fs::path atlasPath(filename);
            fs::path atlasPathDirectory;
            if (HasParentPath(atlasPath)) {
                // save the directory where the atlas file is located
                atlasPathDirectory = GetParentPath(atlasPath);
            }

            // Atlas itself doesn't have appended id
            fs::path atlasImagePath = atlasPathDirectory / atlasSource;
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
            if (atlasElem->FirstChildElement("subimage") != nullptr) {
                for (XML::Element* imageElem = atlasElem->FirstChildElement("subimage"); imageElem != nullptr;
                     imageElem               = imageElem->NextSiblingElement("subimage")) {

                    const char* subimageId = XML::Attribute(imageElem, "id");
                    if (subimageId != nullptr) {
                        Rect region;
                        XML::QueryAttribute(imageElem, "xpos", &region.x);
                        XML::QueryAttribute(imageElem, "ypos", &region.y);
                        XML::QueryAttribute(imageElem, "width", &region.w);
                        XML::QueryAttribute(imageElem, "height", &region.h);

                        std::string finalname;
                        // atlas id is optional here
                        if (atlasId != nullptr) {
                            finalname = std::string(atlasId) + ":" + subimageId;
                        } else {
                            finalname = subimageId;
                        }
                        ImagePtr subImage;

                        if (!m_imageManager->exists(finalname)) {
                            subImage = m_imageManager->create(finalname);
                        } else {
                            subImage = m_imageManager->getPtr(finalname);
                        }
                        subImage->useSharedImage(atlas->getPackedImage(), region);

                        AtlasData atlasData = {.rect=region, .image=subImage};
                        atlas->addImage(finalname, atlasData);
                    }
                }
            } else {
                // Create subimages with automatic id and same size
                int frame          = 0;
                int atlasWidth     = 0;
                int atlasHeight    = 0;
                int subimageWidth  = 0;
                int subimageHeight = 0;
                XML::QueryAttribute(atlasElem, "atlas_width", &atlasWidth);
                XML::QueryAttribute(atlasElem, "atlas_height", &atlasHeight);
                XML::QueryAttribute(atlasElem, "subimage_width", &subimageWidth);
                XML::QueryAttribute(atlasElem, "subimage_height", &subimageHeight);
                // file extension of the atlas is also used as subimage extension
                std::string extension = GetExtension(std::string(atlasSource));
                // we need an atlas id
                if (atlasId == nullptr) {
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
                            finalname << atlasId << ":" << std::string(tmp) << extension;

                            ImagePtr subImage;
                            if (!m_imageManager->exists(finalname.str())) {
                                subImage = m_imageManager->create(finalname.str());
                            } else {
                                subImage = m_imageManager->getPtr(finalname.str());
                            }
                            subImage->useSharedImage(atlas->getPackedImage(), region);

                            AtlasData atlasData = {.rect=region, .image=subImage};
                            atlas->addImage(finalname.str(), atlasData);

                            ++frame;
                        }
                    }
                }
            }
        }

        return atlas;
    }

    AtlasLoader* createDefaultAtlasLoader(
        Model* model, VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager)
    {
        return new AtlasLoader(model, vfs, imageManager, animationManager);
    }
} // namespace FIFE
