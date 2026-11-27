// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_RENDERDOC_CAPTURE_H
#define FIFE_DEBUG_RENDERDOC_CAPTURE_H

#include <atomic>
#include <mutex>
#include <string>

#include "debug/renderdoc/renderdocapi.h"
#include "platform.h"

namespace FIFE
{
    /** Frame capture control built on the official RenderDoc in-app API.
     *
     * Only EndFrameCapture() returning 1 is treated as success. The API is
     * never called when it was not obtained, so a missing RenderDoc is a
     * harmless no-op.
     */
    class FIFE_API RenderDocCapture
    {
        public:
            explicit RenderDocCapture(RenderDocAPI const & api);

            RenderDocCapture(RenderDocCapture const &)            = delete;
            RenderDocCapture& operator=(RenderDocCapture const &) = delete;

            /** Override the wildcard device/window pair. Both default to nullptr. */
            void setTarget(RENDERDOC_DevicePointer device, RENDERDOC_WindowHandle window);

            /** Requests capture of the next presented frame. */
            bool triggerCapture();

            /** Starts an explicit capture region. Refuses nested captures. */
            bool startCapture();

            /** Ends an explicit capture region; reports API-reported success. */
            bool endCapture();

            bool isCaptureInProgress() const
            {
                return m_capturing.load();
            }

            /** Clears a stale in-progress flag during facade re-initialisation. */
            void reset()
            {
                m_capturing.store(false);
            }

            /** Sets the capture file path template (not a final filename). */
            void setCapturePath(std::string const & pathTemplate);

            /** Queries RenderDoc for the most recent capture path after success. */
            bool refreshLastCapturePath();

            std::string getLastCapturePath() const;
            std::string const & getPathTemplate() const
            {
                return m_pathTemplate;
            }

            void setLogEvents(bool enabled)
            {
                m_logEvents = enabled;
            }

        private:
            RenderDocAPI const & m_api;
            RENDERDOC_DevicePointer m_device{nullptr};
            RENDERDOC_WindowHandle m_window{nullptr};
            std::atomic<bool> m_capturing{false};
            mutable std::mutex m_mutex;
            std::string m_lastPath;
            std::string m_pathTemplate;
            bool m_logEvents{true};
    };
} // namespace FIFE

#endif
