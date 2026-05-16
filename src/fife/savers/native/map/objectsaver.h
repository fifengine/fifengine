// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_OBJECTSAVER_H_
#define FIFE_OBJECTSAVER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "iobjectsaver.h"

namespace FIFE
{
    class ImageManager;
    class Model;
    class Object;

    /**
     * Default object saver class implementing the IObjectSaver interface
     */
    class FIFE_API ObjectSaver : public IObjectSaver
    {
        public:
            /**
             * Constructor
             *
             * @param model Pointer to the model, must not be null.
             * @param imageManager Pointer to the image manager, must not be null.
             */
            ObjectSaver(Model* model, ImageManager* imageManager);

            /**
             * Destructor
             */
            ~ObjectSaver() override;

            void setAnimationSaver(AnimationSaverPtr const & animationSaver) override;

            AnimationSaverPtr getAnimationSaver();

            /**
             * Sets the object to be saved.
             * Must be called before save().
             */
            void setObject(Object const * object);

            void save(std::string const & filename) override;

        private:
            AnimationSaverPtr m_animationSaver;
            Object const * m_object;
            Model* m_model;
            ImageManager* m_imageManager;
    };
} // namespace FIFE

#endif
