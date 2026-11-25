// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SHADER_COMPAT_GLSL_H
#define FIFE_SHADER_COMPAT_GLSL_H

// One macro layer for all four dialects. Only GLSL type-less aliases live
// here; the version/precision lines are supplied by preambleFor().
inline constexpr char const * kShaderCompatMacros = R"(
#define FIFE_VERTEX_IN in
#define FIFE_VERTEX_OUT out
#define FIFE_FRAG_IN in
#define FIFE_FRAG_OUT_DECL out vec4 fife_fragColor;
#define FIFE_FRAG_COLOR fife_fragColor
#define FIFE_TEXTURE texture
)";

#endif
