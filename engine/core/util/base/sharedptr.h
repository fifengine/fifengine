/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/
#ifndef FIFE_SHARED_PTR_H_
#define FIFE_SHARED_PTR_H_

// Standard C++ library includes
#include <cassert>
#include <functional>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fife_stdint.h"

namespace FIFE {
    /** shared pointer implementation to provide automatic
    * reference counting and deletion when last reference
    * falls out of scope.
    */
    template <typename T>
    class SharedPtr {
    public:

        /** Constructor
        * default constructor
        * creates a null shared pointer
        */
        SharedPtr()
        : m_ptr(0), m_refCount(0) {

		}

        /** Constructor
        * takes over ownership of the provided pointer
        * and will delete it automatically when last
        * reference falls out of scope.
        */
        template <typename U>
        explicit SharedPtr(U *ptr)
        : m_ptr(ptr), m_refCount(ptr ? new uint32_t(1) : 0) {

		}

        /** Copy Constructor
        * provides ability to properly copy a shared resource
        */
        SharedPtr(const SharedPtr& rhs)
        : m_ptr(rhs.m_ptr), m_refCount(rhs.m_refCount) {
            // increase reference count
            incRefCount();
        }

        /** Constructor
        * shares ownership with the value passed into rhs
        * the pointer type passed in must be convertible
        * to this shared pointer type
        */
        template <typename U>
        SharedPtr(const SharedPtr<U>& rhs) {
            m_ptr = rhs.get();
            m_refCount = rhs.useCountPtr();
            incRefCount();
        }

        /** Destructor
        * handles deletion of underlying pointer
        * if the reference count reaches 0
        */
        ~SharedPtr() {
            // decrement reference count
            decRefCount();

            // check to see if we need to delete
            if (m_refCount && *m_refCount == 0) {
                // delete and set pointers to null
                delete m_ptr;
                delete m_refCount;
                m_ptr = 0;
                m_refCount = 0;
            }
        }

        /** provides functionality for the equality operator
        */
        SharedPtr& operator=(const SharedPtr& rhs) {
            // handle self assignment
            if (rhs.get() == m_ptr) {
                return *this;
            }

            // store in temporary (which causes a ref count increase)
            // and swap with this object
            SharedPtr<T> temp(rhs);
            swap(temp);
            return *this;
        }

        /** provides functionality for the equality operator
        * the passed in pointer type must be convertible to
        * this pointer type
        */
        template <typename U>
        SharedPtr& operator=(const SharedPtr<U>& rhs) {
            // handle self assignment
            if (rhs.get() == m_ptr) {
                return *this;
            }

            // store in temporary (which causes a ref count increase)
            // and swap with this object
            SharedPtr<T> temp(rhs);
            swap(temp);
            return *this;
        }

        /** allows dereferencing of shared pointer to act
        * identical to dereferencing the underlying pointer
        */
        inline T& operator*() const {
            assert(m_ptr);
            return *m_ptr;
        }

        /** allows dereferencing of shared pointer to act
        * identical to dereferencing the underlying pointer
        */
        inline T* operator->() const {
            assert(m_ptr);
            return m_ptr;
        }

        /** allows direct access to underlying pointer
        */
        inline T* get() const {
            return m_ptr;
        }

        /** reset this pointer to a null shared pointer
        * this can be used to lower the reference count
        * of the shared resource or set the underlying pointer
        * to different pointer.
        */
        inline void reset(T* ptr = 0) {
			assert(ptr == 0 || ptr != m_ptr);
            SharedPtr<T>(ptr).swap(*this);
        }

        /** returns the current reference count
        * this should only be called on a non-null
        * shared pointer
        */
        inline uint32_t useCount() const {
            assert(m_refCount);

            if (!m_refCount) {
                return 0;
            }

            return *m_refCount;
        }

        /** returns the current reference count
        * provides direct access to the user count pointer
        * this should really only be used internally
        */
        inline uint32_t* useCountPtr() const {
            return m_refCount;
        }

        /** provides the ability to see if
        * a shared resource is currently only
        * held by a single shared pointer
        * this should only be called on a non-null
        * shared pointer
        */
        inline bool unique() const {
            assert(m_refCount);
            return (*m_refCount == 1);
        }

        /** provides the ability to convert a
        * shared pointer to a bool, this is
        * a convenience for checking validity
        * of a shared pointer in a conditional
        */
        operator bool() const {
            return (m_ptr != 0);
        }

        /** negation operator overload
        */
        bool operator!() const {
            return (m_ptr == 0);
        }

    private:

        /** provides swapping function between
        * two shared pointers, used internally
        */
        inline void swap(SharedPtr<T>& rhs) {
            std::swap(m_ptr, rhs.m_ptr);
            std::swap(m_refCount, rhs.m_refCount);
        }

        /** increases the reference count for
        * this shared resource, used internally
        */
        inline void incRefCount() {
            if (m_refCount) {
                ++(*m_refCount);
            }
        }

        /** decreases the reference count for
        * this shared resource, used internally
        */
        inline void decRefCount() {
            if (m_refCount) {
                --(*m_refCount);
            }
        }

    private:
        T* m_ptr;
        uint32_t* m_refCount;
    };

    /** provides equality operator for shared pointers
    */
    template <typename T, typename U>
    inline bool operator==(const SharedPtr<T>& lhs, const SharedPtr<U>& rhs) {
        return (lhs.get() == rhs.get());
    }

    /** provides inequality operator for shared pointers
    */
    template <typename T, typename U>
    inline bool operator!=(const SharedPtr<T>& lhs, const SharedPtr<U>& rhs) {
        return (lhs.get() != rhs.get());
    }

    /** provides less than operator for shared pointers
    */
    template<class T, class U>
    inline bool operator<(SharedPtr<T> const& lhs, SharedPtr<U> const& rhs) {
        return std::less<const void*>()(lhs.get(), rhs.get());
    }

    /** convenience function for making a shared pointer
    * can be used anytime a shared pointer should be created
    */
    template <typename T>
    SharedPtr<T> make_shared(T* ptr) {
        return SharedPtr<T>(ptr);
    }
} //FIFE

#endif //FIFE_SHARED_PTR_H_
