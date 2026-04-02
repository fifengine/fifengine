// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SINGLETON_H
#define FIFE_SINGLETON_H

#define SINGLEFRIEND(classname)                    \
    friend class FIFE::StaticSingleton<classname>; \
    classname();                                   \
    virtual ~classname();

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fifeclass.h"

namespace FIFE
{

    /** The "classic" Singleton.
     *
     * @see DynamicSingleton
     */
    template <typename T>
    class StaticSingleton
    {
    public:
        static T* instance()
        {
            static T inst;
            return &inst;
        }

    protected:
        StaticSingleton() { }

        virtual ~StaticSingleton() { }

    private:
        StaticSingleton(const StaticSingleton<T>& rhs)
        {
            static_cast<void>(rhs);
        }
        StaticSingleton<T>& operator=(const StaticSingleton<T>& rhs)
        {
            static_cast<void>(rhs);
            return this;
        }
    };

    /** Another Singleton.
     *
     * This implementations needs to be created and destroyed explicitly.
     * That way the order of construction and destruction is well defined, so we don't
     * get those nasty static initialization/destruction order problems.
     *
     * Engine will create all standard FIFE Singletons when created (just call Engine::instance())
     * and destroy them on exit.
     *
     * Maybe we'll change this one day to use one of those funny NiftyCounter implementations.
     *
     * @see StaticSingleton
     * @see Engine
     */
    template <typename T>
    class DynamicSingleton
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

        virtual ~DynamicSingleton()
        {
            m_instance = 0;
        }

    private:
        static T* m_instance;

        DynamicSingleton(const DynamicSingleton<T>& rhs)
        {
            static_cast<void>(rhs);
        }
        DynamicSingleton<T&> operator=(const DynamicSingleton<T>& rhs)
        {
            static_cast<void>(rhs);
        }
    };

    template <typename T>
    T* DynamicSingleton<T>::m_instance = 0;

} // namespace FIFE

#endif
