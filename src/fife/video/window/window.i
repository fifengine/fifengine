// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "video/window/window.h"
%}

namespace FIFE {

%ignore Window::Window(Window const&);
%ignore Window::operator=(Window const&);
%ignore Window::getSDLWindow;
%ignore Window::signalResized;
%ignore Window::signalMoved;
%ignore Window::signalDPIScaleChanged;
%ignore Window::signalShown;
%ignore Window::signalHidden;
%ignore Window::signalClosed;
%ignore Window::onResized;
%ignore Window::onMoved;
%ignore Window::onDPIScaleChanged;
%ignore Window::onShown;
%ignore Window::onHidden;
%ignore Window::onClosed;

class Window {
public:
    Window();
    ~Window();

    void create(WindowSettings const& settings);
    void destroy();

    float getDPIScaleFactor() const;
    int getWidthInPoints() const;
    int getHeightInPoints() const;
    int getWidthInPixels() const;
    int getHeightInPixels() const;

    void setFullscreenMode(WindowMode mode);
    WindowMode getFullscreenMode() const;

    void setScalingMode(ScalingMode mode);
    ScalingMode getScalingMode() const;

    void setSize(int width, int height);
    void setPosition(int x, int y);
    void minimize();
    void maximize();
    void restore();
    void show();
    void hide();
    void setWindowTitle(std::string const& title);
    void setWindowIcon(std::string const& icon);
    void setVSync(bool enabled);
    bool getVSync() const;

    WindowSettings const& getSettings() const;
    int getRefreshRate() const;

    void updateDPIScaleIfNeeded();

    %newobject getDisplay;
    FIFE::Display const& getDisplay() const;

private:
    Window(Window const&);
    Window& operator=(Window const&);
};

}
