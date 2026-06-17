// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "video/window/display.h"

static PyObject* _wrap_Display_getDisplayBounds(int index) {
    SDL_Rect r = FIFE::Display::getDisplayBounds(index);
    return Py_BuildValue("(iiii)", r.x, r.y, r.w, r.h);
}
static PyObject* _wrap_Display_getUsableBounds(int index) {
    SDL_Rect r = FIFE::Display::getUsableBounds(index);
    return Py_BuildValue("(iiii)", r.x, r.y, r.w, r.h);
}
static PyObject* _wrap_Display_getDesktopMode(int index) {
    SDL_DisplayMode m = FIFE::Display::getDesktopMode(index);
    return Py_BuildValue("(iiid)", m.format, m.w, m.h, m.refresh_rate);
}
static PyObject* _wrap_Display_getSupportedModes(int index) {
    auto modes = FIFE::Display::getSupportedModes(index);
    PyObject* list = PyList_New(modes.size());
    for (size_t i = 0; i < modes.size(); ++i) {
        PyList_SetItem(list, i, Py_BuildValue("(iiid)", modes[i].format, modes[i].w, modes[i].h, modes[i].refresh_rate));
    }
    return list;
}
static PyObject* _wrap_Display_getClosestMode(int index, int width, int height, int refreshRate) {
    SDL_DisplayMode m = FIFE::Display::getClosestMode(index, width, height, refreshRate);
    return Py_BuildValue("(iiid)", m.format, m.w, m.h, m.refresh_rate);
}
%}

%include "std_vector.i"

namespace FIFE {

%ignore Display::getDisplayBounds;
%ignore Display::getUsableBounds;
%ignore Display::getDesktopMode;
%ignore Display::getSupportedModes;
%ignore Display::getClosestMode;

class Display {
public:
    static int getDisplayCount();
    static std::string getDisplayName(int index);
    static float getDisplayContentScale(int index);
    static int getDisplayWidth(int index);
    static int getDisplayHeight(int index);
};

}

%inline %{
PyObject* DisplayHelper_getDisplayBounds(int index) { return _wrap_Display_getDisplayBounds(index); }
PyObject* DisplayHelper_getUsableBounds(int index) { return _wrap_Display_getUsableBounds(index); }
PyObject* DisplayHelper_getDesktopMode(int index) { return _wrap_Display_getDesktopMode(index); }
PyObject* DisplayHelper_getSupportedModes(int index) { return _wrap_Display_getSupportedModes(index); }
PyObject* DisplayHelper_getClosestMode(int index, int width, int height, int refreshRate) { return _wrap_Display_getClosestMode(index, width, height, refreshRate); }
%}

%pythoncode %{
Display.getDisplayBounds = staticmethod(DisplayHelper_getDisplayBounds)
Display.getUsableBounds = staticmethod(DisplayHelper_getUsableBounds)
Display.getDesktopMode = staticmethod(DisplayHelper_getDesktopMode)
Display.getSupportedModes = staticmethod(DisplayHelper_getSupportedModes)
Display.getClosestMode = staticmethod(DisplayHelper_getClosestMode)
%}
