// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "debug/renderdoc/renderdocapi.h"

#include <array>
#include <format>
#include <memory>
#include <string>

#include "util/log/logger.h"

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_VIDEO);
            return log;
        }

        // Oldest first would risk using a table with missing members; newest
        // first matches the official docs' recommendation. All version names
        // in renderdoc_app.h alias the same struct layout.
        constexpr std::array<RENDERDOC_Version, 5> kVersions{{
            eRENDERDOC_API_Version_1_7_0,
            eRENDERDOC_API_Version_1_6_0,
            eRENDERDOC_API_Version_1_4_0,
            eRENDERDOC_API_Version_1_1_2,
            eRENDERDOC_API_Version_1_0_0,
        }};
    } // namespace

    void* DefaultRenderDocLibraryLoader::loadResidentModule()
    {
#ifdef _WIN32
        // GetModuleHandle does not add a reference; RenderDoc owns this module.
        return static_cast<void*>(GetModuleHandleA("renderdoc.dll"));
#else
        // RTLD_NOLOAD only detects an already-loaded library. The handle is
        // intentionally not dlclose()d: RenderDoc owns the library.
        return dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
#endif
    }

    void* DefaultRenderDocLibraryLoader::getSymbol(void* module, char const * name)
    {
        if (module == nullptr || name == nullptr) {
            return nullptr;
        }
#ifdef _WIN32
        return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(module), name));
#else
        return dlsym(module, name);
#endif
    }

    RenderDocAPI::RenderDocAPI() : m_ownedLoader(std::make_unique<DefaultRenderDocLibraryLoader>())
    {
        m_loader = m_ownedLoader.get();
    }

    RenderDocAPI::RenderDocAPI(IRenderDocLibraryLoader* loader) : m_loader(loader)
    {
        if (m_loader == nullptr) {
            m_ownedLoader = std::make_unique<DefaultRenderDocLibraryLoader>();
            m_loader      = m_ownedLoader.get();
        }
    }

    RenderDocAPI::~RenderDocAPI()
    {
        shutdown();
    }

    RenderDocAPI::InitResult RenderDocAPI::initialize()
    {
        std::scoped_lock const lock(m_mutex);
        if (m_api.load() != nullptr) {
            return InitResult::AlreadyInitialized;
        }

        void* const module = m_loader->loadResidentModule();
        if (module == nullptr) {
            FL_LOG(_log(), "RenderDoc unavailable: no resident RenderDoc module detected");
            return InitResult::Unavailable;
        }
        m_module.store(module);

        void* const getAPI = m_loader->getSymbol(module, "RENDERDOC_GetAPI");
        if (getAPI == nullptr) {
            FL_WARN(_log(), "RenderDoc detected but RENDERDOC_GetAPI was not found");
            m_module.store(nullptr);
            return InitResult::MissingSymbol;
        }

        // Dynamic symbol lookup yields void*; C++ needs a reinterpret_cast to call it through its real signature.
        auto const getAPIFunc =
            reinterpret_cast<pRENDERDOC_GetAPI>(getAPI); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        for (RENDERDOC_Version const version : kVersions) {
            void* table = nullptr;
            if (getAPIFunc(version, &table) != 1 || table == nullptr) {
                continue;
            }
            m_api.store(static_cast<RENDERDOC_API_1_7_0*>(table));
            m_version = RenderDocVersion{};
            if (m_api.load()->GetAPIVersion != nullptr) {
                m_api.load()->GetAPIVersion(&m_version.major, &m_version.minor, &m_version.patch);
            }
            FL_LOG(
                _log(),
                std::format(
                    "RenderDoc detected; in-app API initialized (reported version {}.{}.{})",
                    m_version.major,
                    m_version.minor,
                    m_version.patch));
            return InitResult::Success;
        }

        FL_WARN(_log(), "RenderDoc in-app API version unsupported (requested 1.7.0 through 1.0.0)");
        m_module.store(nullptr);
        return InitResult::UnsupportedVersion;
    }

    void RenderDocAPI::shutdown()
    {
        std::scoped_lock const lock(m_mutex);
        m_api.store(nullptr);
        m_module.store(nullptr);
        m_version = RenderDocVersion{};
    }

    RenderDocVersion RenderDocAPI::version() const
    {
        std::scoped_lock const lock(m_mutex);
        return m_version;
    }
} // namespace FIFE
