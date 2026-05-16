// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ANIMATIONSAVER_H_
#define FIFE_ANIMATIONSAVER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "ianimationsaver.h"
#include "video/animation.h"

namespace FIFE
{
    class FIFE_API AnimationSaver : public IAnimationSaver
    {
        public:
            AnimationSaver();

            ~AnimationSaver() override;

            void setAnimation(Animation* animation);

            void save(std::string const & filename) override;

        private:
            Animation* m_animation;
    };
} // namespace FIFE

#endif
