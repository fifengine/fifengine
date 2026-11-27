// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "debug/renderdoc/renderdoccapture.h"

#include <format>
#include <string>

#include "util/log/logger.h"

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_VIDEO);
            return log;
        }

        bool supportsIsFrameCapturing(RenderDocVersion const & version)
        {
            // IsFrameCapturing() became reliable for hotkey/TriggerCapture
            // captures in API 1.0.1; the function pointer exists in all versions.
            return version.major > 1 || (version.major == 1 && (version.minor > 0 || version.patch >= 1));
        }
    } // namespace

    RenderDocCapture::RenderDocCapture(RenderDocAPI const & api) : m_api(api)
    {
    }

    void RenderDocCapture::setTarget(RENDERDOC_DevicePointer device, RENDERDOC_WindowHandle window)
    {
        std::scoped_lock const lock(m_mutex);
        m_device = device;
        m_window = window;
    }

    bool RenderDocCapture::triggerCapture()
    {
        RENDERDOC_API_1_7_0 const * const api = m_api.api();
        if (api == nullptr || api->TriggerCapture == nullptr) {
            return false;
        }
        if (m_capturing.load()) {
            if (m_logEvents) {
                FL_WARN(_log(), "RenderDoc capture requested while an explicit capture is in progress");
            }
            return false;
        }
        api->TriggerCapture();
        if (m_logEvents) {
            FL_LOG(_log(), "RenderDoc capture requested for the next presented frame");
        }
        return true;
    }

    bool RenderDocCapture::startCapture()
    {
        RENDERDOC_API_1_7_0 const * const api = m_api.api();
        if (api == nullptr || api->StartFrameCapture == nullptr) {
            return false;
        }
        std::scoped_lock const lock(m_mutex);
        if (m_capturing.load()) {
            if (m_logEvents) {
                FL_WARN(_log(), "RenderDoc StartCapture ignored: a capture is already in progress");
            }
            return false;
        }
        if (supportsIsFrameCapturing(m_api.version()) && api->IsFrameCapturing != nullptr &&
            api->IsFrameCapturing() != 0) {
            if (m_logEvents) {
                FL_WARN(_log(), "RenderDoc StartCapture ignored: RenderDoc reports a capture already active");
            }
            return false;
        }
        api->StartFrameCapture(m_device, m_window);
        m_capturing.store(true);
        if (m_logEvents) {
            FL_LOG(_log(), "RenderDoc frame capture started");
        }
        return true;
    }

    bool RenderDocCapture::endCapture()
    {
        RENDERDOC_API_1_7_0 const * const api = m_api.api();
        if (api == nullptr || api->EndFrameCapture == nullptr) {
            return false;
        }
        {
            std::scoped_lock const lock(m_mutex);
            if (!m_capturing.load()) {
                if (m_logEvents) {
                    FL_WARN(_log(), "RenderDoc EndCapture ignored: no explicit capture is in progress");
                }
                return false;
            }
            uint32_t const result = api->EndFrameCapture(m_device, m_window);
            m_capturing.store(false);
            if (result != 1) {
                if (m_logEvents) {
                    FL_ERR(_log(), "RenderDoc frame capture failed (EndFrameCapture returned 0)");
                }
                return false;
            }
            if (m_logEvents) {
                FL_LOG(_log(), "RenderDoc frame capture completed");
            }
        }
        // refreshLastCapturePath() takes the same mutex; do not hold it here.
        refreshLastCapturePath();
        return true;
    }

    void RenderDocCapture::setCapturePath(std::string const & pathTemplate)
    {
        RENDERDOC_API_1_7_0 const * const api = m_api.api();
        if (api == nullptr || api->SetCaptureFilePathTemplate == nullptr) {
            return;
        }
        std::scoped_lock const lock(m_mutex);
        api->SetCaptureFilePathTemplate(pathTemplate.c_str());
        m_pathTemplate = pathTemplate;
        if (m_logEvents) {
            FL_LOG(_log(), std::format("RenderDoc capture path template set to '{}'", pathTemplate));
        }
    }

    bool RenderDocCapture::refreshLastCapturePath()
    {
        RENDERDOC_API_1_7_0 const * const api = m_api.api();
        if (api == nullptr || api->GetNumCaptures == nullptr || api->GetCapture == nullptr) {
            return false;
        }
        std::scoped_lock const lock(m_mutex);
        uint32_t const count = api->GetNumCaptures();
        if (count == 0) {
            return false;
        }
        uint32_t pathLength = 0;
        if (api->GetCapture(count - 1, nullptr, &pathLength, nullptr) != 1 || pathLength == 0) {
            return false;
        }
        std::string path(static_cast<size_t>(pathLength), '\0');
        uint32_t actualLength = pathLength;
        if (api->GetCapture(count - 1, path.data(), &actualLength, nullptr) != 1) {
            return false;
        }
        path.resize(static_cast<size_t>(actualLength));
        while (!path.empty() && path.back() == '\0') {
            path.pop_back();
        }
        m_lastPath = path;
        if (m_logEvents) {
            FL_LOG(_log(), std::format("RenderDoc capture saved to '{}'", m_lastPath));
        }
        return true;
    }

    std::string RenderDocCapture::getLastCapturePath() const
    {
        std::scoped_lock const lock(m_mutex);
        return m_lastPath;
    }
} // namespace FIFE
