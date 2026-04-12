// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIEW_RENDERITEM_H
#define FIFE_VIEW_RENDERITEM_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes

#include "visual.h"

namespace FIFE
{

    class Instance;

    class /*FIFE_API*/ OverlayData
    {
    public:
        OverlayData();
        ~OverlayData();

        // pointer to single color overlay
        OverlayColors* colorOverlay;
        // pointer to vector that holds animation overlay images
        std::vector<ImagePtr>* animationOverlayImages;
        // pointer to vector that holds color overlays for animation overlay
        std::vector<OverlayColors*>* animationColorOverlays;
    };

    class /*FIFE_API*/ RenderItem
    {
    public:
        explicit RenderItem(Instance* parent);
        ~RenderItem();
        Instance* instance;

        /** Returns closest matching static image for given angle
         * @return id for static image
         * @see ObjectVisual::getStaticImageIndexByAngle
         */
        int32_t getStaticImageIndexByAngle(int32_t angle, Instance* instance);

        /** Sets AnimationOverlay and if available AnimationOverlayColors.
         * Note: Ownership of the vectors do change.
         */
        void setAnimationOverlay(std::vector<ImagePtr>* ao, std::vector<OverlayColors*>* aco);

        /** Returns pointer to AnimationOverlay vector.
         * Returns 0 if no OverlayData or AnimationOverlay exists.
         */
        std::vector<ImagePtr>* getAnimationOverlay() const;

        /** Returns pointer to AnimationColorOverlay vector.
         * Returns 0 if no OverlayData or AnimationColorOverlay exists.
         */
        std::vector<OverlayColors*>* getAnimationColorOverlay() const;

        /** Sets single ColorOverlay.
         * Note: Ownership does not change.
         */
        void setColorOverlay(OverlayColors* co);

        /** Returns pointer to single ColorOverlay.
         * Returns 0 if no OverlayData or ColorOverlay exists.
         */
        OverlayColors* getColorOverlay() const;

        /** Deletes OverlayData.
         */
        void deleteOverlayData();

        /** Resets the important values.
         */
        void reset();

        // point where instance was drawn during the previous render
        DoublePoint3D screenpoint;

        // dimensions of this visual on the virtual screen
        Rect bbox;

        // dimensions of this visual during the previous render
        Rect dimensions;

        // z value for sorting or depth buffer rendering
        float vertexZ;

        // image used during previous render
        ImagePtr image;

        // current facing angle
        int32_t facingAngle;

        // current transparency
        uint8_t transparency;

        // current frame index (e.g. needed for action frame)
        int32_t currentFrame;

        // pointer to overlay data class
        OverlayData* m_overlay;

    private:
        int32_t m_cachedStaticImgId;
        int32_t m_cachedStaticImgAngle;
    };

    using RenderList = std::vector<RenderItem*>;
} // namespace FIFE

#endif
