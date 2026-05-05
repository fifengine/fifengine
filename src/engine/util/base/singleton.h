// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SINGLETON_H
#define FIFE_SINGLETON_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
#include "fifeclass.h"

namespace FIFE
{

    /**
     * A classic Singleton.
     *
     * @see DynamicSingleton
     */
    template <typename T>
    class /*FIFE_API*/ StaticSingleton
    {
        public:
            static T* instance()
            {
                static T inst;
                return &inst;
            }

        protected:
            StaticSingleton() = default;

            virtual ~StaticSingleton() = default;

        private:
            StaticSingleton(StaticSingleton<T> const & rhs)
            {
                static_cast<void>(rhs);
            }
            StaticSingleton<T>& operator=(StaticSingleton<T> const & rhs)
            {
                static_cast<void>(rhs);
                return *this;
            }
    };

    /**
     * Another Singleton.
     *
     * This implementations needs to be created and destroyed explicitly.
     * That way the order of construction and destruction is well defined, so we don't
     * get those nasty static initialization/destruction order problems.
     *
     * Engine will create all standard FIFE Singletons when created (just call Engine::instance())
     * and destroy them on exit.
     *
     * Maybe we'll change this to use a Counter implementation.
     *
     * @see StaticSingleton
     * @see Engine
     */
    template <typename T>
    class /*FIFE_API*/ DynamicSingleton
    {
        public:
            static T* instance()
            {
                assert(m_instance);
                return m_instance;
            }

            DynamicSingleton()
            {
                assert(!m_instance);
                m_instance = static_cast<T*>(this);
            }

            ~DynamicSingleton()
            {
                m_instance = 0;
            }

        private:
            static T* m_instance;

            DynamicSingleton(DynamicSingleton<T> const & rhs)
            {
                static_cast<void>(rhs);
            }
            DynamicSingleton<T>& operator=(DynamicSingleton<T> const & rhs)
            {
                static_cast<void>(rhs);
                return *this;
            }
    };

    template <typename T>
    T* DynamicSingleton<T>::m_instance = nullptr;

} // namespace FIFE

#endif
