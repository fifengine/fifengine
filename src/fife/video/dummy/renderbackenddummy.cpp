// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "video/dummy/renderbackenddummy.h"

// C++ standard library includes
#include <memory>
#include <string>
#include <vector>

namespace FIFE
{
    RenderBackendDummy::RenderBackendDummy(SDL_Color const & colorkey, uint32_t width, uint32_t height) :
        RenderBackend(colorkey),
        m_width(width),
        m_height(height),
        m_area(0, 0, static_cast<int32_t>(width), static_cast<int32_t>(height))
    {
    }

    RenderBackendDummy::~RenderBackendDummy() = default;

    std::string const & RenderBackendDummy::getName() const
    {
        static std::string const name = "Dummy";
        return name;
    }

    std::string RenderBackendDummy::getRenderDescription() const
    {
        return "headless (no window)";
    }

    uint32_t RenderBackendDummy::getWidth() const
    {
        return m_width;
    }

    uint32_t RenderBackendDummy::getHeight() const
    {
        return m_height;
    }

    Rect const & RenderBackendDummy::getArea() const
    {
        return m_area;
    }

    void RenderBackendDummy::init(std::string const & /*driver*/)
    {
    }

    void RenderBackendDummy::clearBackBuffer()
    {
    }

    void RenderBackendDummy::setLightingModel([[maybe_unused]] uint32_t lighting)
    {
    }

    uint32_t RenderBackendDummy::getLightingModel() const
    {
        return 0;
    }

    void RenderBackendDummy::setLighting(float /*red*/, float /*green*/, float /*blue*/)
    {
    }

    void RenderBackendDummy::resetLighting()
    {
    }

    void RenderBackendDummy::resetStencilBuffer(uint8_t /*buffer*/)
    {
    }

    void RenderBackendDummy::changeBlending(int32_t /*scr*/, int32_t /*dst*/)
    {
    }

    void RenderBackendDummy::createMainScreen(std::string const & /*title*/, std::string const & /*icon*/)
    {
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage([[maybe_unused]] IResourceLoader* loader)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(std::string const & /*name*/, IResourceLoader* /*loader*/)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(
        uint8_t const * /*data*/, uint32_t /*width*/, uint32_t /*height*/)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(
        std::string const & /*name*/, uint8_t const * /*data*/, uint32_t /*width*/, uint32_t /*height*/)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage([[maybe_unused]] SDL_Surface* surface)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(std::string const & /*name*/, SDL_Surface* /*surface*/)
    {
        return nullptr;
    }

    void RenderBackendDummy::renderVertexArrays()
    {
    }

    void RenderBackendDummy::addImageToArray(
        uint32_t /*id*/, Rect const & /*rec*/, float const * /*st*/, uint8_t /*alpha*/, uint8_t const * /*rgba*/)
    {
    }

    void RenderBackendDummy::changeRenderInfos(
        RenderDataType /*type*/,
        uint16_t /*elements*/,
        int32_t /*src*/,
        int32_t /*dst*/,
        bool /*light*/,
        bool /*stentest*/,
        uint8_t /*stenref*/,
        GLConstants /*stenop*/,
        GLConstants /*stenfunc*/,
        OverlayType /*otype*/)
    {
    }

    void RenderBackendDummy::captureScreen(std::string const & /*filename*/)
    {
        // A headless backend may have no screen surface at all; there is nothing to capture.
        if (m_screen == nullptr) {
            return;
        }
    }

    void RenderBackendDummy::captureScreen(std::string const & /*filename*/, uint32_t /*width*/, uint32_t /*height*/)
    {
        if (m_screen == nullptr) {
            return;
        }
    }

    bool RenderBackendDummy::putPixel(
        int32_t /*x*/, int32_t /*y*/, uint8_t /*r*/, uint8_t /*g*/, uint8_t /*b*/, uint8_t /*a*/)
    {
        return false;
    }

    void RenderBackendDummy::drawLine(
        Point const & /*p1*/, Point const & /*p2*/, uint8_t /*r*/, uint8_t /*g*/, uint8_t /*b*/, uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawThickLine(
        Point const & /*p1*/,
        Point const & /*p2*/,
        uint8_t /*width*/,
        uint8_t /*r*/,
        uint8_t /*g*/,
        uint8_t /*b*/,
        uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawPolyLine(
        std::vector<Point> const & /*points*/,
        uint8_t /*width*/,
        uint8_t /*r*/,
        uint8_t /*g*/,
        uint8_t /*b*/,
        uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawBezier(
        std::vector<Point> const & /*points*/,
        int32_t /*steps*/,
        uint8_t /*width*/,
        uint8_t /*r*/,
        uint8_t /*g*/,
        uint8_t /*b*/,
        uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawTriangle(
        Point const & /*p1*/,
        Point const & /*p2*/,
        Point const & /*p3*/,
        uint8_t /*r*/,
        uint8_t /*g*/,
        uint8_t /*b*/,
        uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::fillTriangle(
        Point const & /*p1*/,
        Point const & /*p2*/,
        Point const & /*p3*/,
        uint8_t /*r*/,
        uint8_t /*g*/,
        uint8_t /*b*/,
        uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawRectangle(
        Point const & /*p*/, uint16_t /*w*/, uint16_t /*h*/, uint8_t /*r*/, uint8_t /*g*/, uint8_t /*b*/, uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::fillRectangle(
        Point const & /*p*/, uint16_t /*w*/, uint16_t /*h*/, uint8_t /*r*/, uint8_t /*g*/, uint8_t /*b*/, uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawQuad(
        Point const & /*p1*/,
        Point const & /*p2*/,
        Point const & /*p3*/,
        Point const & /*p4*/,
        uint8_t /*r*/,
        uint8_t /*g*/,
        uint8_t /*b*/,
        uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawVertex(
        Point const & /*p*/, uint8_t /*size*/, uint8_t /*r*/, uint8_t /*g*/, uint8_t /*b*/, uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawCircle(
        Point const & /*p*/, uint32_t /*radius*/, uint8_t /*r*/, uint8_t /*g*/, uint8_t /*b*/, uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawFillCircle(
        Point const & /*p*/, uint32_t /*radius*/, uint8_t /*r*/, uint8_t /*g*/, uint8_t /*b*/, uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawCircleSegment(
        Point const & /*p*/,
        uint32_t /*radius*/,
        int32_t /*sangle*/,
        int32_t /*eangle*/,
        uint8_t /*r*/,
        uint8_t /*g*/,
        uint8_t /*b*/,
        uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawFillCircleSegment(
        Point const & /*p*/,
        uint32_t /*radius*/,
        int32_t /*sangle*/,
        int32_t /*eangle*/,
        uint8_t /*r*/,
        uint8_t /*g*/,
        uint8_t /*b*/,
        uint8_t /*a*/)
    {
    }

    void RenderBackendDummy::drawLightPrimitive(
        Point const & /*p*/,
        uint8_t /*intensity*/,
        float /*radius*/,
        int32_t /*subdivisions*/,
        float /*xstretch*/,
        float /*ystretch*/,
        uint8_t /*red*/,
        uint8_t /*green*/,
        uint8_t /*blue*/)
    {
    }

    void RenderBackendDummy::enableScissorTest()
    {
    }

    void RenderBackendDummy::disableScissorTest()
    {
    }

    void RenderBackendDummy::attachRenderTarget(ImagePtr& /*img*/, bool /*discard*/)
    {
    }

    void RenderBackendDummy::detachRenderTarget()
    {
    }

    void RenderBackendDummy::renderGuiGeometry(
        std::vector<GuiVertex> const & /*vertices*/,
        std::vector<int> const & /*indices*/,
        DoublePoint const & /*translation*/,
        ImagePtr /*texture*/)
    {
    }

    void RenderBackendDummy::setClipArea(Rect const & /*cliparea*/, bool /*clear*/)
    {
    }

} // namespace FIFE
