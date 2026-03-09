// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/action.h"
#include "model/metamodel/object.h"
#include "model/structures/instance.h"

#include "renderitem.h"
#include "visual.h"

namespace FIFE
{
    const int32_t STATIC_IMAGE_NOT_INITIALIZED = -1;

    OverlayData::OverlayData() : colorOverlay(nullptr), animationOverlayImages(nullptr), animationColorOverlays(nullptr)
    {
    }

    OverlayData::~OverlayData()
    {
        delete animationOverlayImages;
        delete animationColorOverlays;
        // don't delete colorOverlay here
    }

    RenderItem::RenderItem(Instance* parent) :
        instance(parent),
        screenpoint(),
        dimensions(),
        vertexZ(0),
        facingAngle(0),
        transparency(255),
        currentFrame(-1),
        m_overlay(nullptr),
        m_cachedStaticImgId(STATIC_IMAGE_NOT_INITIALIZED),
        m_cachedStaticImgAngle(0)
    {
    }

    RenderItem::~RenderItem()
    {
        delete m_overlay;
    }

    int32_t RenderItem::getStaticImageIndexByAngle(uint32_t angle, Instance* instance)
    {
        ObjectVisual* objVis = instance->getObject()->getVisual<ObjectVisual>();
        if (!objVis) {
            return STATIC_IMAGE_NOT_INITIALIZED;
        }
        if (static_cast<int32_t>(angle) != m_cachedStaticImgAngle) {
            m_cachedStaticImgId = STATIC_IMAGE_NOT_INITIALIZED;
        }
        if (objVis->isColorOverlay()) {
            if (!m_overlay) {
                m_overlay = new OverlayData();
            }
            m_overlay->colorOverlay = objVis->getStaticColorOverlay(angle);
        }
        if (m_cachedStaticImgId != STATIC_IMAGE_NOT_INITIALIZED) {
            return m_cachedStaticImgId;
        }

        m_cachedStaticImgId    = objVis->getStaticImageIndexByAngle(angle);
        m_cachedStaticImgAngle = angle;

        return m_cachedStaticImgId;
    }

    void RenderItem::setAnimationOverlay(std::vector<ImagePtr>* ao, std::vector<OverlayColors*>* aco)
    {
        if (m_overlay == nullptr) {
            m_overlay = new OverlayData();
        }
        m_overlay->animationOverlayImages = ao;
        m_overlay->animationColorOverlays = aco;
    }

    std::vector<ImagePtr>* RenderItem::getAnimationOverlay() const
    {
        if (m_overlay != nullptr) {
            return m_overlay->animationOverlayImages;
        }
        return nullptr;
    }

    std::vector<OverlayColors*>* RenderItem::getAnimationColorOverlay() const
    {
        if (m_overlay != nullptr) {
            return m_overlay->animationColorOverlays;
        }
        return nullptr;
    }

    void RenderItem::setColorOverlay(OverlayColors* co)
    {
        if (m_overlay == nullptr) {
            m_overlay = new OverlayData();
        }
        m_overlay->colorOverlay = co;
    }

    OverlayColors* RenderItem::getColorOverlay() const
    {
        if (m_overlay) {
            return m_overlay->colorOverlay;
        }
        return nullptr;
    }

    void RenderItem::deleteOverlayData()
    {
        if (m_overlay) {
            delete m_overlay;
            m_overlay = nullptr;
        }
    }

    void RenderItem::reset()
    {
        instance   = nullptr;
        dimensions = Rect();
        image.reset();
        transparency        = 255;
        currentFrame        = -1;
        m_cachedStaticImgId = STATIC_IMAGE_NOT_INITIALIZED;
        deleteOverlayData();
    }
} // namespace FIFE
