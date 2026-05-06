// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "gui_imageloader.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "video/atlasbook.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

static uint32_t const ATLAS_SIZE = 512;

namespace FIFE
{
    GuiImageLoader::GuiImageLoader() : m_atlasbook(new AtlasBook(ATLAS_SIZE, ATLAS_SIZE))
    {
    }

    GuiImageLoader::~GuiImageLoader()
    {
        delete m_atlasbook;
    }

    fcn::Image* GuiImageLoader::load(std::string const & filename, [[maybe_unused]] bool convertToDisplayFormat)
    {
        ImageManager* imgManager = ImageManager::instance();

        if (imgManager->exists(filename)) {
            return new GuiImage(imgManager->get(filename));
        }
        // load demanded image
        ImagePtr tmpimg = imgManager->load(filename);
        if (tmpimg->getWidth() >= ATLAS_SIZE || tmpimg->getHeight() >= ATLAS_SIZE) {
            return new GuiImage(tmpimg);
        }
        // look for a place for an image of given size
        AtlasBlock* block = m_atlasbook->getBlock(tmpimg->getWidth(), tmpimg->getHeight());

        // if it can't fit, we need to add new 'page'
        if (block->page >= m_atlases.size()) {
            m_atlases.push_back(imgManager->loadBlank(ATLAS_SIZE, ATLAS_SIZE));

            // because we gonna update texture on-the fly (via TexSubImage)
            // we cant really use compressed texture
            RenderBackend* rb = RenderBackend::instance();
            bool const prev   = rb->isImageCompressingEnabled();
            rb->setImageCompressingEnabled(false);
            m_atlases[block->page]->forceLoadInternal();
            rb->setImageCompressingEnabled(prev);
        }

        // update atlas page with given image
        m_atlases[block->page]->copySubimage(block->left, block->top, tmpimg);

        // we dont really need this image anymore
        tmpimg->free();
        imgManager->remove(tmpimg);

        // create shared image and return it
        ImagePtr const img = imgManager->create(filename);
        Rect const region(block->left, block->top, block->getWidth(), block->getHeight());
        img->useSharedImage(m_atlases[block->page], region);

        return new GuiImage(img);
    }
} // namespace FIFE
