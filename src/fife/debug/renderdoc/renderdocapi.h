// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_RENDERDOC_API_H
#define FIFE_DEBUG_RENDERDOC_API_H

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>

#include "platform.h"

// Vendored verbatim from RenderDoc v1.x: renderdoc/api/app/renderdoc_app.h
// (API version 1.7.0). Never link against RenderDoc itself.
#include "debug/renderdoc/renderdoc_app.h"

namespace FIFE
{
    /** Version actually reported by the loaded RenderDoc implementation. */
    struct RenderDocVersion
    {
            int major{0};
            int minor{0};
            int patch{0};
    };

    /** Platform seam used to find an already-resident RenderDoc module.
     *
     * The documented loading mechanism is passive detection: on Windows
     * GetModuleHandle("renderdoc.dll"), on Linux dlopen with RTLD_NOLOAD.
     * The module is owned by RenderDoc and must never be unloaded by us.
     * Tests inject a fake implementation of this interface.
     */
    class FIFE_API IRenderDocLibraryLoader
    {
        public:
            virtual ~IRenderDocLibraryLoader() = default;

            /** @return module handle, or nullptr if RenderDoc is not resident. */
            virtual void* loadResidentModule() = 0;

            /** @return address of @p name, or nullptr if it is not exported. */
            virtual void* getSymbol(void* module, char const * name) = 0;
    };

    class FIFE_API DefaultRenderDocLibraryLoader final : public IRenderDocLibraryLoader
    {
        public:
            void* loadResidentModule() override;
            void* getSymbol(void* module, char const * name) override;
    };

    /** Owns the dynamically located RenderDoc in-app API table.
     *
     * Construction and destruction are safe with or without RenderDoc. All
     * failures are reported through the return value of initialize() and
     * logged once via FIFE's logger; nothing throws.
     */
    class FIFE_API RenderDocAPI
    {
        public:
            enum class InitResult : uint8_t
            {
                Success,
                Unavailable,
                MissingSymbol,
                UnsupportedVersion,
                AlreadyInitialized
            };

            /** Uses the real platform loader. */
            RenderDocAPI();

            /** Uses @p loader (non-owning) — test seam. */
            explicit RenderDocAPI(IRenderDocLibraryLoader* loader);

            ~RenderDocAPI();
            RenderDocAPI(RenderDocAPI const &)            = delete;
            RenderDocAPI& operator=(RenderDocAPI const &) = delete;

            /** Locates the module and requests the newest supported API. */
            InitResult initialize();

            /** Clears all state. Safe to call repeatedly. */
            void shutdown();

            /** True when a RenderDoc module was found. */
            bool isAvailable() const
            {
                return m_module.load() != nullptr;
            }

            /** True when a usable API table was obtained. */
            bool isInitialized() const
            {
                return m_api.load() != nullptr;
            }

            /** @return the API table, or nullptr when unavailable. */
            RENDERDOC_API_1_7_0* api() const
            {
                return m_api.load();
            }

            /** @return the version the implementation actually reported. */
            RenderDocVersion version() const;

            /** Highest API version this module can request. */
            static constexpr RENDERDOC_Version maxVersion = eRENDERDOC_API_Version_1_7_0;

        private:
            IRenderDocLibraryLoader* m_loader;
            std::unique_ptr<IRenderDocLibraryLoader> m_ownedLoader;
            std::atomic<void*> m_module{nullptr};
            std::atomic<RENDERDOC_API_1_7_0*> m_api{nullptr};
            mutable std::mutex m_mutex;
            RenderDocVersion m_version{};
    };
} // namespace FIFE

#endif
