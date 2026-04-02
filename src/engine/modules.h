// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODULES_H
#define FIFE_MODULES_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

/**
 * Modules available for logging
 */
enum logmodule_t : signed char
{
    LM_CORE = -1,
    LM_AUDIO,
    LM_CONTROLLER,
    LM_EVTCHANNEL,
    LM_GUI,
    LM_CONSOLE,
    LM_LOADERS,
    LM_NATIVE_LOADERS,
    LM_FO_LOADERS,
    LM_SAVERS,
    LM_NATIVE_SAVERS,
    LM_MODEL,
    LM_STRUCTURES,
    LM_INSTANCE,
    LM_LOCATION,
    LM_METAMODEL,
    LM_CELLGRID,
    LM_SQUAREGRID,
    LM_HEXGRID,
    LM_PATHFINDER,
    LM_UTIL,
    LM_RESMGR,
    LM_VFS,
    LM_VIDEO,
    LM_VIEW,
    LM_CAMERA,
    LM_VIEWVIEW,
    LM_XML,
    LM_EXCEPTION,
    LM_SCRIPT,
    LM_CURSOR,
    LM_MODULE_MAX // sentinel
};

#endif
