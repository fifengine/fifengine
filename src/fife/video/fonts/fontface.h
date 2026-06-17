// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_FONTS_FONTFACE_H
#define FIFE_VIDEO_FONTS_FONTFACE_H

#include <memory>

#include "fonttypes.h"
#include "platform.h"

namespace FIFE
{

    class FIFE_API FontFace
    {
        public:
            explicit FontFace(AssetHandle handle);
            virtual ~FontFace() = default;
            AssetHandle getAssetHandle() const
            {
                return m_assetHandle;
            }
            virtual bool supports(uint32_t codepoint) const;
            virtual int getPointSize() const
            {
                return 0;
            }

            virtual void setDPIScale(float /*factor*/)
            {
            }
            virtual float getDPIScale() const
            {
                return 1.0f;
            }

        protected:
            AssetHandle m_assetHandle;
    };

} // namespace FIFE

#endif
