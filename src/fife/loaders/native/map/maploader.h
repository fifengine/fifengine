// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MAPLOADER_H_
#define FIFE_MAPLOADER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes

#include "ianimationloader.h"

namespace tinyxml2
{
    class XMLElement;
}
#include "imaploader.h"
#include "percentdonelistener.h"

namespace FIFE
{
    class Model;
    class Map;
    class VFS;
    class Layer;
    class ImageManager;
    class AnimationManager;
    class RenderBackend;
    class PercentDoneListener;

    class FIFE_API MapLoader : public IMapLoader
    {
        public:
            MapLoader(Model* model, VFS* vfs, ImageManager* imageManager, RenderBackend* renderBackend);

            ~MapLoader() override;

            /**
             * @see IMapLoader::setObjectLoader
             */
            void setObjectLoader(FIFE::ObjectLoaderPtr const & objectLoader) override;

            /**
             * @see IMapLoader::getObjectLoader
             */
            FIFE::ObjectLoaderPtr getObjectLoader();

            /**
             * @see IMapLoader::setAnimationLoader
             */
            void setAnimationLoader(FIFE::AnimationLoaderPtr const & animationLoader) override;

            /**
             * @see IMapLoader::getAnimationLoader
             */
            FIFE::AnimationLoaderPtr getAnimationLoader();

            /**
             * @see IMapLoader::setAtlasLoader
             */
            void setAtlasLoader(FIFE::AtlasLoaderPtr const & atlasLoader) override;

            /**
             * @see IMapLoader::getAtlasLoader
             */
            FIFE::AtlasLoaderPtr getAtlasLoader();

            /**
             * @see IMapLoader::isLoadable
             */
            bool isLoadable(std::string const & filename) const override;

            /**
             * @see IMapLoader::load
             */
            Map* load(std::string const & filename) override;

            /** used to load an object file
             * if directory is provided then file is assumed relative to directory
             * if relativeToMap is true then the file/directory is assumed to be relative to
             * location of map file
             */
            void loadImportFile(std::string const & file, std::string const & directory = "");

            /** used to load a directory of object files recursively
             * if relativeToMap is true then the directory is assumed to be relative to
             * location of map file
             */
            void loadImportDirectory(std::string const & directory);

            /**
             * allows adding a listener to the map loader
             * for percent completed events
             */
            void addPercentDoneListener(PercentDoneListener* listener);

            /** returns the loader name associated with this
             * map file loader, this will only be populated
             * after the load function is called
             */
            std::string const & getLoaderName() const;

        private:
            struct LightData
            {
                    std::string type;
                    std::string group;
                    std::string instanceName;
                    std::string layerName;
                    std::string cameraId;

                    // simple light
                    float radius     = 10.0F;
                    int intensity    = 128;
                    int subdivisions = 32;
                    float xstretch   = 1.0F;
                    float ystretch   = 1.0F;
                    int colorR       = 255;
                    int colorG       = 255;
                    int colorB       = 255;

                    // image light
                    std::string imagePath;

                    // animation light
                    std::string animationPath;

                    int x = 0;
                    int y = 0;

                    int srcBlend = -1;
                    int dstBlend = -1;

                    int sRef   = -1;
                    float aRef = 0.0F;
            };

            Model* m_model;
            VFS* m_vfs;
            ImageManager* m_imageManager;
            AnimationManager* m_animationManager;
            ObjectLoaderPtr m_objectLoader;
            RenderBackend* m_renderBackend;
            PercentDoneCallback m_percentDoneListener;

            std::string m_loaderName;
            std::string m_mapDirectory;
            std::vector<std::string> m_importDirectories;
            std::vector<LightData> m_lightData;

            void parseLights(tinyxml2::XMLElement const * layerElement, Layer* layer);
            void parseSounds(tinyxml2::XMLElement const * layerElement, Layer* layer);
            void createLightNodes(Map* map);
    };

    /** convenience function for creating the default fife map loader
     * deleting the object returned from this function is the
     * responsibility of the caller
     */
    FIFE_API MapLoader* createDefaultMapLoader(
        Model* model, VFS* vfs, ImageManager* imageManager, RenderBackend* renderBackend);
} // namespace FIFE

#endif
