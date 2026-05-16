// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ATLASSAVER_H_
#define FIFE_ATLASSAVER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "iatlassaver.h"
#include "loaders/native/map/atlasloader.h"

namespace FIFE
{
    class FIFE_API AtlasSaver : public IAtlasSaver
    {
        public:
            AtlasSaver();

            ~AtlasSaver() override;

            void setAtlas(Atlas* atlas);

            void save(std::string const & filename) override;

        private:
            Atlas* m_atlas;
    };
} // namespace FIFE

#endif
