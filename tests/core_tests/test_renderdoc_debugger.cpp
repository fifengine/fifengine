// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "controller/enginesettings.h"
#include "debug/renderdoc/renderdocapi.h"
#include "debug/renderdoc/renderdoccapture.h"
#include "debug/renderdoc/renderdocconfig.h"
#include "debug/renderdoc/renderdocdebugger.h"
#include "debug/renderdoc/renderdocmarkers.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha): clang-format sorts by group

namespace
{
    struct FakeState
    {
            int getAPICalls{0};
            std::vector<RENDERDOC_Version> requestedVersions;
            RENDERDOC_Version lastRequested{eRENDERDOC_API_Version_1_0_0};
            int triggerCalls{0};
            int startCalls{0};
            int endCalls{0};
            int endResult{1};
            int isCapturingResult{0};
            int isFrameCapturingCalls{0};
            std::string pathTemplate;
            int versionMajor{1};
            int versionMinor{7};
            int versionPatch{0};
    };

    FakeState g_state;
    RENDERDOC_API_1_7_0 g_api{};

    void RENDERDOC_CC fakeGetAPIVersion(int* major, int* minor, int* patch)
    {
        if (major != nullptr) {
            *major = g_state.versionMajor;
        }
        if (minor != nullptr) {
            *minor = g_state.versionMinor;
        }
        if (patch != nullptr) {
            *patch = g_state.versionPatch;
        }
    }

    void RENDERDOC_CC fakeTriggerCapture()
    {
        ++g_state.triggerCalls;
    }

    void RENDERDOC_CC fakeStartFrameCapture(RENDERDOC_DevicePointer, RENDERDOC_WindowHandle)
    {
        ++g_state.startCalls;
    }

    uint32_t RENDERDOC_CC fakeIsFrameCapturing()
    {
        ++g_state.isFrameCapturingCalls;
        return static_cast<uint32_t>(g_state.isCapturingResult);
    }

    uint32_t RENDERDOC_CC fakeEndFrameCapture(RENDERDOC_DevicePointer, RENDERDOC_WindowHandle)
    {
        ++g_state.endCalls;
        return static_cast<uint32_t>(g_state.endResult);
    }

    uint32_t RENDERDOC_CC fakeGetNumCaptures()
    {
        return 1;
    }

    uint32_t RENDERDOC_CC fakeGetCapture(uint32_t idx, char* filename, uint32_t* pathlength, uint64_t* timestamp)
    {
        if (idx != 0) {
            return 0;
        }
        char const * const path = "/tmp/fake_frame123.rdc";
        uint32_t const len      = static_cast<uint32_t>(std::strlen(path) + 1);
        if (pathlength != nullptr) {
            *pathlength = len;
        }
        if (filename != nullptr) {
            std::memcpy(filename, path, len);
        }
        if (timestamp != nullptr) {
            *timestamp = 42;
        }
        return 1;
    }

    void RENDERDOC_CC fakeSetCaptureFilePathTemplate(char const * path)
    {
        g_state.pathTemplate = path != nullptr ? path : "";
    }

    char const * RENDERDOC_CC fakeGetCaptureFilePathTemplate()
    {
        return g_state.pathTemplate.c_str();
    }

    void installFakeAPI()
    {
        g_api                            = RENDERDOC_API_1_7_0{};
        g_api.GetAPIVersion              = &fakeGetAPIVersion;
        g_api.TriggerCapture             = &fakeTriggerCapture;
        g_api.StartFrameCapture          = &fakeStartFrameCapture;
        g_api.IsFrameCapturing           = &fakeIsFrameCapturing;
        g_api.EndFrameCapture            = &fakeEndFrameCapture;
        g_api.GetNumCaptures             = &fakeGetNumCaptures;
        g_api.GetCapture                 = &fakeGetCapture;
        g_api.SetCaptureFilePathTemplate = &fakeSetCaptureFilePathTemplate;
        g_api.GetCaptureFilePathTemplate = &fakeGetCaptureFilePathTemplate;
    }

    class FakeLoader final : public FIFE::IRenderDocLibraryLoader
    {
        public:
            enum class Mode
            {
                Available,
                NoModule,
                NoSymbol,
                UnsupportedVersion,
                OlderVersion,
                RejectNewest
            };

            explicit FakeLoader(Mode mode) : m_mode(mode)
            {
                g_state = FakeState{};
                installFakeAPI();
                m_activeMode = mode;
                if (mode == Mode::OlderVersion || mode == Mode::RejectNewest) {
                    g_state.versionMajor = 1;
                    g_state.versionMinor = 6;
                    g_state.versionPatch = 0;
                }
            }

            void* loadResidentModule() override
            {
                return m_mode == Mode::NoModule ? nullptr : reinterpret_cast<void*>(0x1234);
            }

            void* getSymbol(void*, char const * name) override
            {
                if (m_mode == Mode::NoSymbol || std::strcmp(name, "RENDERDOC_GetAPI") != 0) {
                    return nullptr;
                }
                return reinterpret_cast<void*>(&getAPI);
            }

            static int RENDERDOC_CC getAPI(RENDERDOC_Version version, void** out)
            {
                ++g_state.getAPICalls;
                g_state.lastRequested = version;
                g_state.requestedVersions.push_back(version);
                if (m_activeMode == Mode::UnsupportedVersion ||
                    (m_activeMode == Mode::RejectNewest && version == eRENDERDOC_API_Version_1_7_0)) {
                    return 0;
                }
                if (out != nullptr) {
                    *out = &g_api;
                }
                return 1;
            }

            static void setActiveMode(Mode mode)
            {
                m_activeMode = mode;
            }

        private:
            Mode m_mode;
            static Mode m_activeMode;
    };

    FakeLoader::Mode FakeLoader::m_activeMode = FakeLoader::Mode::Available;
} // namespace

TEST_CASE("renderdoc API initializes from a resident module", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    FIFE::RenderDocAPI api(&loader);

    REQUIRE(api.initialize() == FIFE::RenderDocAPI::InitResult::Success);
    CHECK(api.isAvailable());
    CHECK(api.isInitialized());
    CHECK(api.api() != nullptr);
    CHECK(api.version().major == 1);
    CHECK(api.version().minor == 7);
    CHECK(g_state.getAPICalls == 1);
    CHECK(g_state.lastRequested == eRENDERDOC_API_Version_1_7_0);
}

TEST_CASE("renderdoc unavailable when module is absent", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::NoModule);
    FIFE::RenderDocAPI api(&loader);

    CHECK(api.initialize() == FIFE::RenderDocAPI::InitResult::Unavailable);
    CHECK_FALSE(api.isAvailable());
    CHECK_FALSE(api.isInitialized());
    CHECK(api.api() == nullptr);
}

TEST_CASE("renderdoc API loading fails when entry point is missing", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::NoSymbol);
    FIFE::RenderDocAPI api(&loader);

    CHECK(api.initialize() == FIFE::RenderDocAPI::InitResult::MissingSymbol);
    CHECK_FALSE(api.isInitialized());
}

TEST_CASE("renderdoc API falls back when newest version is unsupported", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::UnsupportedVersion);
    FakeLoader loader(FakeLoader::Mode::UnsupportedVersion);
    FIFE::RenderDocAPI api(&loader);

    CHECK(api.initialize() == FIFE::RenderDocAPI::InitResult::UnsupportedVersion);
    CHECK(g_state.getAPICalls > 1);
    CHECK_FALSE(api.isInitialized());
}

TEST_CASE("renderdoc reports the version returned by the implementation", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::OlderVersion);
    FIFE::RenderDocAPI api(&loader);

    REQUIRE(api.initialize() == FIFE::RenderDocAPI::InitResult::Success);
    CHECK(api.version().major == 1);
    CHECK(api.version().minor == 6);
    CHECK(api.version().patch == 0);
}

namespace
{
    struct GLRecord
    {
            int pushCalls{0};
            int popCalls{0};
            int insertCalls{0};
            int labelCalls{0};
            uint32_t lastSource{0};
            uint32_t lastType{0};
            uint32_t lastSeverity{0};
            uint32_t lastIdentifier{0};
            uint32_t lastName{0};
            std::string lastMessage;
    };

    GLRecord g_gl;

    void RENDERDOC_CC fakePushDebugGroup(uint32_t source, uint32_t, int32_t, char const * message)
    {
        ++g_gl.pushCalls;
        g_gl.lastSource  = source;
        g_gl.lastMessage = message != nullptr ? message : "";
    }

    void RENDERDOC_CC fakePopDebugGroup()
    {
        ++g_gl.popCalls;
    }

    void RENDERDOC_CC
    fakeDebugMessageInsert(uint32_t source, uint32_t type, uint32_t, uint32_t severity, int32_t, char const * message)
    {
        ++g_gl.insertCalls;
        g_gl.lastSource   = source;
        g_gl.lastType     = type;
        g_gl.lastSeverity = severity;
        g_gl.lastMessage  = message != nullptr ? message : "";
    }

    void RENDERDOC_CC fakeObjectLabel(uint32_t identifier, uint32_t name, int32_t, char const * label)
    {
        ++g_gl.labelCalls;
        g_gl.lastIdentifier = identifier;
        g_gl.lastName       = name;
        g_gl.lastMessage    = label != nullptr ? label : "";
    }

    FIFE::RenderDocGLFunctions fakeGLFunctions()
    {
        return FIFE::RenderDocGLFunctions{
            &fakePushDebugGroup, &fakePopDebugGroup, &fakeDebugMessageInsert, &fakeObjectLabel};
    }

    std::unique_ptr<FIFE::RenderDocAPI> initializedAPI(FakeLoader& loader)
    {
        auto api = std::make_unique<FIFE::RenderDocAPI>(&loader);
        REQUIRE(api->initialize() == FIFE::RenderDocAPI::InitResult::Success);
        return api;
    }
} // namespace

TEST_CASE("renderdoc single frame capture reports API-reported success", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocCapture capture(*api);

    CHECK(capture.triggerCapture());
    CHECK(g_state.triggerCalls == 1);
    CHECK(capture.startCapture());
    CHECK(g_state.startCalls == 1);
    CHECK(capture.isCaptureInProgress());
    CHECK_FALSE(capture.startCapture());
    CHECK(g_state.startCalls == 1);
    CHECK(capture.endCapture());
    CHECK_FALSE(capture.isCaptureInProgress());
    CHECK(capture.getLastCapturePath() == "/tmp/fake_frame123.rdc");
}

TEST_CASE("renderdoc failed EndFrameCapture does not record a capture path", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    g_state.endResult = 0;
    auto api          = initializedAPI(loader);
    FIFE::RenderDocCapture capture(*api);

    REQUIRE(capture.startCapture());
    CHECK_FALSE(capture.endCapture());
    CHECK(capture.getLastCapturePath().empty());
    CHECK_FALSE(capture.isCaptureInProgress());
}

TEST_CASE("renderdoc capture path template is set through the in-app API", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocCapture capture(*api);

    capture.setCapturePath("/tmp/captures/fife");
    CHECK(g_state.pathTemplate == "/tmp/captures/fife");
    CHECK(capture.getPathTemplate() == "/tmp/captures/fife");
}

TEST_CASE("renderdoc capture calls are inert when the API is unavailable", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::NoModule);
    FIFE::RenderDocAPI api(&loader);
    CHECK(api.initialize() == FIFE::RenderDocAPI::InitResult::Unavailable);
    FIFE::RenderDocCapture capture(api);

    CHECK_FALSE(capture.triggerCapture());
    CHECK_FALSE(capture.startCapture());
    CHECK_FALSE(capture.endCapture());
    CHECK(g_state.triggerCalls == 0);
    CHECK(g_state.startCalls == 0);
    CHECK(g_state.endCalls == 0);
}

TEST_CASE("renderdoc markers emit KHR_debug calls with a valid context", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocMarkers markers(*api);
    g_gl = GLRecord{};
    markers.setGLFunctions(fakeGLFunctions());
    markers.setContextValid(true);

    markers.beginEvent(FIFE::passName(FIFE::RenderDocPass::Frame));
    markers.setMarker("one-shot");
    markers.nameTexture(7, "atlas");
    markers.endEvent();

    CHECK(g_gl.pushCalls == 1);
    CHECK(g_gl.popCalls == 1);
    CHECK(g_gl.insertCalls == 1);
    CHECK(g_gl.labelCalls == 1);
    CHECK(g_gl.lastSource == 0x824A);
    CHECK(g_gl.lastType == 0x8268);
    CHECK(g_gl.lastMessage == "atlas");
}

TEST_CASE("renderdoc markers are inert without a GL context", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocMarkers markers(*api);
    g_gl = GLRecord{};
    markers.setGLFunctions(fakeGLFunctions());
    markers.setContextValid(false);

    markers.beginEvent("Frame");
    markers.setMarker("tick");
    markers.nameFramebuffer(3, "fbo");
    markers.endEvent();

    CHECK(g_gl.pushCalls == 0);
    CHECK(g_gl.popCalls == 0);
    CHECK(g_gl.insertCalls == 0);
    CHECK(g_gl.labelCalls == 0);
}

TEST_CASE("renderdoc unbalanced EndEvent is ignored", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocMarkers markers(*api);
    g_gl = GLRecord{};
    markers.setGLFunctions(fakeGLFunctions());
    markers.setContextValid(true);

    markers.endEvent();
    CHECK(g_gl.popCalls == 0);

    markers.beginEvent("Frame");
    markers.endEvent();
    markers.endEvent();
    CHECK(g_gl.popCalls == 1);
}

TEST_CASE("renderdoc GL context lifecycle disables markers after shutdown", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocMarkers markers(*api);
    g_gl = GLRecord{};
    markers.setGLFunctions(fakeGLFunctions());
    markers.setContextValid(true);
    markers.beginEvent("Frame");
    CHECK(g_gl.pushCalls == 1);

    api->shutdown();
    markers.beginEvent("Frame");
    CHECK(g_gl.pushCalls == 1);
    CHECK_FALSE(api->isInitialized());
}

TEST_CASE("renderdoc pass names are stable", "[renderdoc]")
{
    CHECK(std::string(FIFE::passName(FIFE::RenderDocPass::Shadow)) == "Shadow Pass");
    CHECK(std::string(FIFE::passName(FIFE::RenderDocPass::Geometry)) == "Geometry Pass");
    CHECK(std::string(FIFE::passName(FIFE::RenderDocPass::Lighting)) == "Lighting Pass");
    CHECK(std::string(FIFE::passName(FIFE::RenderDocPass::Bloom)) == "Bloom");
    CHECK(std::string(FIFE::passName(FIFE::RenderDocPass::Tonemapping)) == "Tonemapping");
    CHECK(std::string(FIFE::passName(FIFE::RenderDocPass::Gui)) == "UI Pass");
}

TEST_CASE("renderdoc config mirrors engine settings", "[renderdoc]")
{
    FIFE::EngineSettings settings;
    CHECK_FALSE(settings.isRenderDocEnabled());
    CHECK(settings.isRenderDocHotkeyCapture());
    CHECK(settings.getRenderDocCaptureHotkey() == "F12");

    settings.setRenderDocEnabled(true);
    settings.setRenderDocAutoCapture(true);
    settings.setRenderDocHotkeyCapture(false);
    settings.setRenderDocCaptureHotkey("P");
    settings.setRenderDocCaptureDirectory("/tmp/caps");
    settings.setRenderDocDebugMarkers(false);
    settings.setRenderDocResourceNaming(false);
    settings.setRenderDocLogCaptureEvents(false);

    CHECK(settings.isRenderDocEnabled());
    CHECK(settings.isRenderDocAutoCapture());
    CHECK_FALSE(settings.isRenderDocHotkeyCapture());
    CHECK(settings.getRenderDocCaptureHotkey() == "P");
    CHECK(settings.getRenderDocCaptureDirectory() == "/tmp/caps");
    CHECK_FALSE(settings.isRenderDocDebugMarkers());
    CHECK_FALSE(settings.isRenderDocResourceNaming());
    CHECK_FALSE(settings.isRenderDocLogCaptureEvents());
}

TEST_CASE("renderdoc facade initializes only when enabled", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader disabledLoader(FakeLoader::Mode::Available);
    FIFE::RenderDocDebugger disabled(&disabledLoader);
    FIFE::RenderDocConfig config;
    CHECK_FALSE(disabled.initialize(config));
    CHECK_FALSE(disabled.isEnabled());
    CHECK(g_state.getAPICalls == 0);

    FakeLoader loader(FakeLoader::Mode::Available);
    FIFE::RenderDocDebugger debugger(&loader);
    config.enableRenderDoc  = true;
    config.autoCapture      = true;
    config.captureDirectory = "/tmp/captures";
    CHECK(debugger.initialize(config));
    CHECK(debugger.isEnabled());
    CHECK(debugger.isInitialized());
    CHECK(debugger.isAvailable());
    CHECK(g_state.pathTemplate == "/tmp/captures/fife_capture");

    debugger.onFrameBegin();
    CHECK(g_state.triggerCalls == 1);
    debugger.onFrameBegin();
    CHECK(g_state.triggerCalls == 1);
    debugger.onFrameEnd();
}

TEST_CASE("renderdoc facade shutdown is idempotent", "[renderdoc]")
{
    FakeLoader::setActiveMode(FakeLoader::Mode::Available);
    FakeLoader loader(FakeLoader::Mode::Available);
    FIFE::RenderDocDebugger debugger(&loader);
    FIFE::RenderDocConfig config;
    config.enableRenderDoc = true;
    REQUIRE(debugger.initialize(config));

    debugger.shutdown();
    debugger.shutdown();
    CHECK_FALSE(debugger.isEnabled());
    CHECK_FALSE(debugger.isInitialized());
    CHECK_FALSE(debugger.triggerCapture());
    CHECK_FALSE(debugger.startCapture());
}

TEST_CASE("renderdoc hotkey uses edge detection", "[renderdoc]")
{
    CHECK(FIFE::RenderDocDebugger::isHotkeyEdge(true, false));
    CHECK_FALSE(FIFE::RenderDocDebugger::isHotkeyEdge(true, true));
    CHECK_FALSE(FIFE::RenderDocDebugger::isHotkeyEdge(false, false));
}

TEST_CASE("renderdoc one-shot marker uses the correct GL severity", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocMarkers markers(*api);
    g_gl = GLRecord{};
    markers.setGLFunctions(fakeGLFunctions());
    markers.setContextValid(true);

    markers.setMarker("tick");
    CHECK(g_gl.lastSource == 0x824A);
    CHECK(g_gl.lastType == 0x8268);
    CHECK(g_gl.lastSeverity == 0x826B);
}

TEST_CASE("renderdoc object labels use the correct GL object enums", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocMarkers markers(*api);
    g_gl = GLRecord{};
    markers.setGLFunctions(fakeGLFunctions());
    markers.setContextValid(true);

    markers.nameTexture(1, "texture");
    CHECK(g_gl.lastIdentifier == 0x1702);
    markers.nameFramebuffer(2, "framebuffer");
    CHECK(g_gl.lastIdentifier == 0x8D40);
    markers.nameBuffer(3, "buffer");
    CHECK(g_gl.lastIdentifier == 0x82E0);
    markers.nameShader(4, "shader");
    CHECK(g_gl.lastIdentifier == 0x82E1);
    markers.nameProgram(5, "program");
    CHECK(g_gl.lastIdentifier == 0x82E2);
    markers.nameRenderbuffer(6, "renderbuffer");
    CHECK(g_gl.lastIdentifier == 0x8D41);
    markers.nameVertexArray(7, "vao");
    CHECK(g_gl.lastIdentifier == 0x8074);
}

TEST_CASE("renderdoc retries an older API version when the newest is rejected", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::RejectNewest);
    FIFE::RenderDocAPI api(&loader);
    REQUIRE(api.initialize() == FIFE::RenderDocAPI::InitResult::Success);
    REQUIRE(g_state.requestedVersions.size() >= 2);
    CHECK(g_state.requestedVersions[0] == eRENDERDOC_API_Version_1_7_0);
    CHECK(g_state.requestedVersions[1] == eRENDERDOC_API_Version_1_6_0);
    CHECK(api.version().major == 1);
    CHECK(api.version().minor == 6);
}

TEST_CASE("renderdoc tolerates a sparse API table", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api                  = initializedAPI(loader);
    g_api.IsFrameCapturing    = nullptr;
    g_api.SetCaptureOptionU32 = nullptr;
    FIFE::RenderDocCapture capture(*api);

    CHECK(capture.startCapture());
    CHECK(capture.isCaptureInProgress());
    CHECK(capture.endCapture());
}

TEST_CASE("renderdoc capture reset clears in-progress state", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocCapture capture(*api);
    REQUIRE(capture.startCapture());
    capture.reset();
    CHECK_FALSE(capture.isCaptureInProgress());
    CHECK(capture.startCapture());
}

TEST_CASE("renderdoc marker reset clears group depth", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::Available);
    auto api = initializedAPI(loader);
    FIFE::RenderDocMarkers markers(*api);
    g_gl = GLRecord{};
    markers.setGLFunctions(fakeGLFunctions());
    markers.setContextValid(true);
    markers.beginEvent("Frame");
    markers.reset();
    markers.endEvent();
    CHECK(g_gl.popCalls == 0);
}

TEST_CASE("renderdoc facade reinitialization clears capture state", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::Available);
    FIFE::RenderDocDebugger debugger(&loader);
    FIFE::RenderDocConfig config;
    config.enableRenderDoc = true;
    REQUIRE(debugger.initialize(config));
    REQUIRE(debugger.startCapture());
    debugger.shutdown();
    REQUIRE(debugger.initialize(config));
    CHECK(debugger.startCapture());
}

TEST_CASE("renderdoc facade exposes configured hotkey policy", "[renderdoc]")
{
    FakeLoader loader(FakeLoader::Mode::Available);
    FIFE::RenderDocDebugger debugger(&loader);
    FIFE::RenderDocConfig config;
    config.enableRenderDoc = true;
    config.captureOnHotkey = false;
    config.captureHotkey   = "P";
    REQUIRE(debugger.initialize(config));
    CHECK_FALSE(debugger.isHotkeyCaptureEnabled());
    CHECK(debugger.getCaptureHotkey() == "P");
}
