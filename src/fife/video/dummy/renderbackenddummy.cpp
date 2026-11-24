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

    void RenderBackendDummy::init(std::string const &)
    {
    }

    void RenderBackendDummy::clearBackBuffer()
    {
    }

    void RenderBackendDummy::setLightingModel(uint32_t lighting)
    {
    }

    uint32_t RenderBackendDummy::getLightingModel() const
    {
        return 0;
    }

    void RenderBackendDummy::setLighting(float, float, float)
    {
    }

    void RenderBackendDummy::resetLighting()
    {
    }

    void RenderBackendDummy::resetStencilBuffer(uint8_t)
    {
    }

    void RenderBackendDummy::changeBlending(int32_t, int32_t)
    {
    }

    void RenderBackendDummy::createMainScreen(std::string const &, std::string const &)
    {
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(IResourceLoader* loader)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(std::string const & name, IResourceLoader* loader)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(uint8_t const * data, uint32_t width, uint32_t height)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(
        std::string const & name, uint8_t const * data, uint32_t width, uint32_t height)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(SDL_Surface* surface)
    {
        return nullptr;
    }

    std::unique_ptr<Image> RenderBackendDummy::createImage(std::string const & name, SDL_Surface* surface)
    {
        return nullptr;
    }

    void RenderBackendDummy::renderVertexArrays()
    {
    }

    void RenderBackendDummy::addImageToArray(uint32_t, Rect const &, float const *, uint8_t, uint8_t const *)
    {
    }

    void RenderBackendDummy::changeRenderInfos(
        RenderDataType, uint16_t, int32_t, int32_t, bool, bool, uint8_t, GLConstants, GLConstants, OverlayType)
    {
    }

    void RenderBackendDummy::captureScreen(std::string const & filename)
    {
        // A headless backend may have no screen surface at all; there is nothing to capture.
        if (m_screen == nullptr) {
            return;
        }
    }

    void RenderBackendDummy::captureScreen(std::string const & filename, uint32_t width, uint32_t height)
    {
        if (m_screen == nullptr) {
            return;
        }
    }

    bool RenderBackendDummy::putPixel(int32_t, int32_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
        return false;
    }

    void RenderBackendDummy::drawLine(Point const &, Point const &, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawThickLine(Point const &, Point const &, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawPolyLine(std::vector<Point> const &, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawBezier(
        std::vector<Point> const &, int32_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawTriangle(
        Point const &, Point const &, Point const &, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::fillTriangle(
        Point const &, Point const &, Point const &, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawRectangle(Point const &, uint16_t, uint16_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::fillRectangle(Point const &, uint16_t, uint16_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawQuad(
        Point const &, Point const &, Point const &, Point const &, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawVertex(Point const &, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawCircle(Point const &, uint32_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawFillCircle(Point const &, uint32_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawCircleSegment(
        Point const &, uint32_t, int32_t, int32_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawFillCircleSegment(
        Point const &, uint32_t, int32_t, int32_t, uint8_t, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::drawLightPrimitive(
        Point const &, uint8_t, float, int32_t, float, float, uint8_t, uint8_t, uint8_t)
    {
    }

    void RenderBackendDummy::enableScissorTest()
    {
    }

    void RenderBackendDummy::disableScissorTest()
    {
    }

    void RenderBackendDummy::attachRenderTarget(ImagePtr&, bool)
    {
    }

    void RenderBackendDummy::detachRenderTarget()
    {
    }

    void RenderBackendDummy::renderGuiGeometry(
        std::vector<GuiVertex> const & vertices,
        std::vector<int> const & indices,
        DoublePoint const & translation,
        ImagePtr texture)
    {
    }

    void RenderBackendDummy::setClipArea(Rect const & cliparea, bool clear)
    {
    }

} // namespace FIFE
