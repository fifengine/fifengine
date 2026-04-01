// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cassert>
#include <limits>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "util/time/timemanager.h"
#include "video/imagemanager.h"

#include "animation.h"
#include "cursor.h"
#include "image.h"
#include "renderbackend.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_CURSOR);

    Cursor::Cursor(RenderBackend* renderbackend) :
        m_cursor_id(NC_ARROW),
        m_cursor_type(CURSOR_NATIVE),
        m_drag_type(CURSOR_NONE),
        m_native_cursor(nullptr),
        m_renderbackend(renderbackend),
        m_animtime(0),
        m_drag_animtime(0),
        m_drag_offset_x(0),
        m_drag_offset_y(0),
        m_mx(0),
        m_my(0),
        m_timemanager(TimeManager::instance()),
        m_invalidated(false),
        m_native_image_cursor_enabled(false)
    {
        assert(m_timemanager);
        set(m_cursor_id);
    }

    void Cursor::set(uint32_t cursor_id)
    {
        m_cursor_type = CURSOR_NATIVE;

        if (SDL_ShowCursor(1) == 0) {
            SDL_PumpEvents();
        }
        setNativeCursor(cursor_id);

        m_cursor_image.reset();
        m_cursor_animation.reset();
    }

    void Cursor::set(ImagePtr image)
    {
        assert(image != 0);

        m_cursor_image = image;
        m_cursor_type  = CURSOR_IMAGE;

        if (m_native_image_cursor_enabled) {
            if (!setNativeImageCursor(image)) {
                return;
            }
            if (SDL_ShowCursor(1) == 0) {
                SDL_PumpEvents();
            }
        } else if (SDL_ShowCursor(0) != 0) {
            SDL_PumpEvents();
        }

        m_cursor_id = NC_ARROW;
        m_cursor_animation.reset();
    }

    void Cursor::set(AnimationPtr anim)
    {
        assert(anim != 0);

        m_cursor_animation = anim;
        m_cursor_type      = CURSOR_ANIMATION;

        if (m_native_image_cursor_enabled) {
            if (!setNativeImageCursor(anim->getFrameByTimestamp(0))) {
                return;
            }
            if (SDL_ShowCursor(1) == 0) {
                SDL_PumpEvents();
            }
        } else if (SDL_ShowCursor(0) != 0) {
            SDL_PumpEvents();
        }
        m_animtime = m_timemanager->getTime();

        m_cursor_id = NC_ARROW;
        m_cursor_image.reset();
    }

    void Cursor::setDrag(ImagePtr image, int32_t drag_offset_x, int32_t drag_offset_y)
    {
        assert(image != 0);

        m_cursor_drag_image = image;
        m_drag_type         = CURSOR_IMAGE;
        m_drag_offset_x     = drag_offset_x;
        m_drag_offset_y     = drag_offset_y;

        m_cursor_drag_animation.reset();
    }

    void Cursor::setDrag(AnimationPtr anim, int32_t drag_offset_x, int32_t drag_offset_y)
    {
        assert(anim != 0);

        m_cursor_drag_animation = anim;
        m_drag_type             = CURSOR_ANIMATION;
        m_drag_offset_x         = drag_offset_x;
        m_drag_offset_y         = drag_offset_y;

        m_drag_animtime = m_timemanager->getTime();

        m_cursor_drag_image.reset();
    }

    void Cursor::resetDrag()
    {
        m_drag_type = CURSOR_NONE;

        m_drag_animtime = 0;
        m_drag_offset_x = 0;
        m_drag_offset_y = 0;

        m_cursor_drag_animation.reset();
        m_cursor_drag_image.reset();
    }

    void Cursor::setPosition(uint32_t x, uint32_t y)
    {
        assert(x <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
        assert(y <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
        m_mx = static_cast<int32_t>(x);
        m_my = static_cast<int32_t>(y);
        SDL_WarpMouseInWindow(RenderBackend::instance()->getWindow(), m_mx, m_my);
    }

    void Cursor::getPosition(int32_t* x, int32_t* y)
    {
        *x = m_mx;
        *y = m_my;
    }

    void Cursor::invalidate()
    {
        if (m_native_cursor != nullptr) {
            SDL_FreeCursor(m_native_cursor);
            m_native_cursor = nullptr;
            m_native_cursor_image.reset();

            m_invalidated = true;
        }
    }

    void Cursor::draw()
    {
        if (m_invalidated) {
            if (m_cursor_type == CURSOR_NATIVE) {
                set(m_cursor_id);
            } else if (m_native_image_cursor_enabled) {
                if (m_cursor_type == CURSOR_IMAGE) {
                    set(m_cursor_image);
                } else if (m_cursor_type == CURSOR_ANIMATION) {
                    set(m_cursor_animation);
                }
            }

            m_invalidated = false;
        }

        SDL_GetMouseState(&m_mx, &m_my);
        if ((m_cursor_type == CURSOR_NATIVE) && (m_drag_type == CURSOR_NONE)) {
            return;
        }

        // render possible drag image
        ImagePtr img;
        if (m_drag_type == CURSOR_IMAGE) {
            img = m_cursor_drag_image;
        } else if (m_drag_type == CURSOR_ANIMATION) {
            const int32_t animtime =
                (m_timemanager->getTime() - m_drag_animtime) % m_cursor_drag_animation->getDuration();
            img = m_cursor_drag_animation->getFrameByTimestamp(animtime);
        }

        if (img) {
            const uint32_t dragWidth  = img->getWidth();
            const uint32_t dragHeight = img->getHeight();
            assert(dragWidth <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
            assert(dragHeight <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
            const Rect area(
                m_mx + m_drag_offset_x + img->getXShift(),
                m_my + m_drag_offset_y + img->getYShift(),
                static_cast<int32_t>(dragWidth),
                static_cast<int32_t>(dragHeight));
            m_renderbackend->pushClipArea(area, false);
            img->render(area);
            m_renderbackend->renderVertexArrays();
            m_renderbackend->popClipArea();
        }

        ImagePtr img2;
        // render possible cursor image
        if (m_cursor_type == CURSOR_IMAGE) {
            img2 = m_cursor_image;
        } else if (m_cursor_type == CURSOR_ANIMATION) {
            const int32_t animtime = (m_timemanager->getTime() - m_animtime) % m_cursor_animation->getDuration();
            img2                   = m_cursor_animation->getFrameByTimestamp(animtime);
        }

        if (img2) {
            if (m_native_image_cursor_enabled) {
                setNativeImageCursor(img2);
            } else {
                const uint32_t cursorWidth  = img2->getWidth();
                const uint32_t cursorHeight = img2->getHeight();
                assert(cursorWidth <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
                assert(cursorHeight <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
                Rect area(
                    m_mx + img2->getXShift(),
                    m_my + img2->getYShift(),
                    static_cast<int32_t>(cursorWidth),
                    static_cast<int32_t>(cursorHeight));
                m_renderbackend->pushClipArea(area, false);
                img2->render(area);
                m_renderbackend->renderVertexArrays();
                m_renderbackend->popClipArea();
            }
        }
    }

    uint32_t Cursor::getNativeId(uint32_t cursor_id)
    {
        switch (cursor_id) {
        case NC_ARROW:
            return SDL_SYSTEM_CURSOR_ARROW;
        case NC_IBEAM:
            return SDL_SYSTEM_CURSOR_IBEAM;
        case NC_WAIT:
            return SDL_SYSTEM_CURSOR_WAIT;
        case NC_CROSS:
            return SDL_SYSTEM_CURSOR_CROSSHAIR;
        case NC_WAITARROW:
            return SDL_SYSTEM_CURSOR_WAITARROW;
        case NC_RESIZENWSE:
            return SDL_SYSTEM_CURSOR_SIZENWSE;
        case NC_RESIZENESW:
            return SDL_SYSTEM_CURSOR_SIZENESW;
        case NC_RESIZEWE:
            return SDL_SYSTEM_CURSOR_SIZEWE;
        case NC_RESIZENS:
            return SDL_SYSTEM_CURSOR_SIZENS;
        case NC_RESIZEALL:
            return SDL_SYSTEM_CURSOR_SIZEALL;
        case NC_NO:
            return SDL_SYSTEM_CURSOR_NO;
        case NC_HAND:
            return SDL_SYSTEM_CURSOR_HAND;
        default:
            return cursor_id;
        }
    }

    void Cursor::setNativeCursor(uint32_t cursor_id)
    {
        cursor_id          = getNativeId(cursor_id);
        SDL_Cursor* cursor = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor_id));
        if (cursor == nullptr) {
            FL_WARN(_log, "No cursor matching cursor_id was found.");
            return;
        }
        SDL_SetCursor(cursor);
        if (m_native_cursor != nullptr) {
            SDL_FreeCursor(m_native_cursor);
        }
        m_native_cursor = cursor;
    }

    bool Cursor::setNativeImageCursor(ImagePtr image)
    {
        if (image == m_native_cursor_image) {
            // we're already using this image
            return true;
        }
        if (image->getState() == IResource::RES_NOT_LOADED) {
            image->load();
        }

        // SDL only accepts whole surfaces here so if this image uses a shared surface
        // we need to prepare a temporary surface with just the relevant part
        ImagePtr temp_image = image;
        if (image->isSharedImage()) {
            temp_image = ImageManager::instance()->create();
            temp_image->copySubimage(0, 0, image);
        }

        SDL_Cursor* cursor = SDL_CreateColorCursor(temp_image->getSurface(), -image->getXShift(), -image->getYShift());
        if (cursor == nullptr) {
            FL_WARN(
                _log, LMsg("SDL_CreateColorCursor: \"") << SDL_GetError() << "\". Falling back to software cursor.");
            if (image->isSharedImage()) {
                ImageManager::instance()->remove(temp_image);
            }
            setNativeImageCursorEnabled(false);
            return false;
        }
        SDL_SetCursor(cursor);
        m_native_cursor_image = image;

        if (image->isSharedImage()) {
            ImageManager::instance()->remove(temp_image);
        }
        if (m_native_cursor != nullptr) {
            SDL_FreeCursor(m_native_cursor);
        }
        m_native_cursor = cursor;
        return true;
    }

    void Cursor::setNativeImageCursorEnabled(bool native_image_cursor_enabled)
    {
        if (m_native_image_cursor_enabled != native_image_cursor_enabled) {
            m_native_image_cursor_enabled = native_image_cursor_enabled;
            if (m_cursor_type == CURSOR_IMAGE) {
                set(m_cursor_image);
            } else if (m_cursor_type == CURSOR_ANIMATION) {
                set(m_cursor_animation);
            }
        }
    }

    bool Cursor::isNativeImageCursorEnabled() const
    {
        return m_native_image_cursor_enabled;
    }
} // namespace FIFE
