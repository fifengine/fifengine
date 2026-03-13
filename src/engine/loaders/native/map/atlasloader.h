// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ATLAS_LOADER_H
#define FIFE_ATLAS_LOADER_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"

#include "iatlasloader.h"

namespace tinyxml2
{
    class XMLElement;
}

namespace FIFE
{
    class VFS;
    class ImageManager;
    class AnimationManager;

    struct AtlasData
    {
        Rect rect;
        ImagePtr image;
    };

    class Atlas
    {
    public:
        explicit Atlas(const std::string& name) : m_name(name)
        {
            ;
        }
        ~Atlas()
        {
            ;
        }

        /** Returns the number of subimages that belongs to this atlas.
         */
        size_t getImageCount() const;

        /** Returns an (packed) Image for this atlas.
         */
        ImagePtr& getPackedImage();

        /** Return an Image of given id.
         */
        ImagePtr getImage(const std::string& id);

        /** Return an Image of given (serial) index in atlas
         */
        ImagePtr getImage(uint32_t index);

        /** Adds new information about subimage that belongs to this atlas.
         *  @remarks This is essential function in parsing atlas files.
         *  @returns True, when image of given name hasn't been added before.
         */
        bool addImage(const std::string& imagename, const AtlasData& data);

        /** Sets the image for atlas to use it for rendering.
         *  @remarks Should only be used during loading stage
         */
        void setPackedImage(const ImagePtr& image);

        const std::string& getName() const;

    protected:
        typedef std::map<std::string, AtlasData> SubimageMap;
        SubimageMap m_subimages;
        ImagePtr m_image;

        // Unique atlas name
        std::string m_name;
    };

    class AtlasLoader : public IAtlasLoader
    {
    public:
        AtlasLoader(Model* model, VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager);

        ~AtlasLoader() override;

        bool isLoadable(const std::string& filename) override;

        AtlasPtr load(const std::string& filename) override;

        std::vector<AtlasPtr> loadMultiple(const std::string& filename) override;

    private:
        AtlasPtr loadAtlas(const std::string& filename, tinyxml2::XMLElement* atlasElem);

        Model* m_model;
        VFS* m_vfs;
        ImageManager* m_imageManager;
        AnimationManager* m_animationManager;
    };

    /** convenience function for creating the default fife atlas loader
     *  deleting the object returned from this function is the
     *  responsibility of the caller
     */
    AtlasLoader* createDefaultAtlasLoader(
        Model* model, VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager);
} // namespace FIFE

#endif
