# FIFE Animation XML Format Specification

**Version:** 1.1
**Date:** 2026-05-22
**Status:** Adopted

---

## Changelog

| Version | Date       | Description |
|---------|------------|-------------|
| 1.0     | 2026-03-18 | Initial format as part of demo reintegration. Single mode: individual frame files via `<frame source="...">`. |
| 1.1     | 2026-05-22 | Added **atlas mode**: `<animation atlas="...">` with shared sub-images created from `xpos`/`ypos`/`width`/`height` on each `<frame>`. The `atlas`, `xpos`, `ypos` attributes were already present in data files as dead metadata. They are now processed by the loader. |

---

## Overview

FIFE uses XML files to define sprite animations.
An animation is a sequence of image frames, each displayed for a configurable duration.
The engine supports two modes for supplying frame imagery:

1. **Individual-frame mode (v1.0):** Each `<frame>` references its own PNG file on disk.
2. **Atlas mode (v1.1):** A single sprite sheet PNG is referenced via the `atlas` attribute on `<animation>`,
and each `<frame>` defines its sub-rectangle within the atlas using `xpos`, `ypos`, `width`, `height`.

---

## File Structure

```
<?xml version="1.0"?>
<animation ...>                     <!-- standalone -->
  <frame .../>
  <frame .../>
</animation>
```

or wrapped in an `<assets>` container (required when loading multiple animations from one file):

```
<?xml version="1.0"?>
<?fife type="animation"?>           <!-- optional processing instruction -->
<assets>
  <animation ...>
    <frame .../>
  </animation>
  <animation ...>                   <!-- additional animations -->
    <frame .../>
  </animation>
</assets>
```

The loader (`AnimationLoader`) accepts both forms:
- **`load()`** — returns the single animation found; if `<assets>` is used, it reads the first `<animation>` child.
- **`loadMultiple()`** — returns all `<animation>` children within `<assets>`.

---

## Root: `<animation>` Element

| Attribute       | Type    | Required | Default | Description |
|-----------------|---------|----------|---------|-------------|
| `id`            | string  | No       | Filename | Unique identifier for the animation in the `AnimationManager`. If omitted, the animation file path (relative to the loading XML) is used. |
| `direction`     | int     | No       | 0       | Direction value for associating this animation with movement. |
| `action_frame`  | int     | No       | -1      | Index of the "action frame" — the frame where the associated game action (e.g., a punch) occurs. |
| `delay`         | int (ms)| No       | 0       | Default frame duration in milliseconds. Used as fallback when a `<frame>` does not specify its own `delay`. |
| `x_offset`      | int     | No       | 0       | Default horizontal pixel offset for all frames. Applied as an X-shift on each frame image. |
| `y_offset`      | int     | No       | 0       | Default vertical pixel offset for all frames. Applied as a Y-shift on each frame image. |
| `atlas`         | string  | No*      | —       | **Atlas mode only.** Path to the sprite sheet PNG file (resolved relative to the animation XML's directory). When present, each `<frame>` must define `xpos`/`ypos` (and optionally `width`/`height`) to identify its sub-region in the atlas. |
| `width`         | int     | No*      | 0       | **Atlas mode.** Default frame width in pixels. Used as fallback when a `<frame>` does not specify its own `width`. |
| `height`        | int     | No*      | 0       | **Atlas mode.** Default frame height in pixels. Used as fallback when a `<frame>` does not specify its own `height`. |

\* Required when `atlas` is present; ignored in individual-frame mode.

---

## Frame: `<frame>` Element

| Attribute    | Type    | Required | Default | Description |
|--------------|---------|----------|---------|-------------|
| `source`     | string  | Yes      | —       | **Individual-frame mode:** Filename of the frame PNG (resolved relative to the animation XML's directory). **Atlas mode:** Identifier string for the frame image in the `ImageManager`. The value itself is not resolved as a file path. |
| `delay`      | int (ms)| No       | `animation.delay` | Duration this frame is displayed, in milliseconds. |
| `x_offset`   | int     | No       | `animation.x_offset` | Horizontal pixel offset for this frame. |
| `y_offset`   | int     | No       | `animation.y_offset` | Vertical pixel offset for this frame. |
| `xpos`       | int     | No*      | 0       | **Atlas mode only.** X-coordinate of the frame's top-left corner within the atlas image. |
| `ypos`       | int     | No*      | 0       | **Atlas mode only.** Y-coordinate of the frame's top-left corner within the atlas image. |
| `width`      | int     | No*      | `animation.width` | **Atlas mode only.** Width of the frame sub-region in pixels. |
| `height`     | int     | No*      | `animation.height` | **Atlas mode only.** Height of the frame sub-region in pixels. |

\* Meaningful only in atlas mode. In individual-frame mode, these attributes are silently ignored.

---

## Loading Behavior

### Individual-frame mode (`atlas` attribute absent)

1. The loader opens the animation XML via VFS.
2. For each `<frame>`, it resolves the `source` path relative to the XML file's directory.
3. If the resolved file does not exist on the native filesystem, it falls back to the bare `source` value.
4. The file is registered in the `ImageManager` and loaded as a standalone image (not shared).
5. Each frame has its own independent `SDL_Surface` / OpenGL texture.

### Atlas mode (`atlas` attribute present)

1. The loader opens the animation XML and resolves the `atlas` path relative to the XML file's directory.
2. The atlas image is loaded once into the `ImageManager`.
3. For each `<frame>`:
   - A sub-image is created via `useSharedImage(atlasImage, Rect(xpos, ypos, width, height))`.
   - The sub-image shares the atlas's underlying pixel data — no copy is made.
   - The sub-image is registered in the `ImageManager` under the `source` identifier.
4. All frames share a single GPU texture (when using OpenGL or SDL renderer). This is more memory-efficient than individual-frame mode.

---

## Saving Behavior

The `AnimationSaver` writes animation XML in the **individual-frame format**:

- It always writes `<frame source="...">` using `Image::getName()` as the source value.
- For shared images (atlas mode at load time), it additionally writes `xpos`, `ypos`, `width`, `height` attributes from the sub-image rectangle.
- The `atlas` attribute is **not written** by the current saver. Atlas-mode animations saved and reloaded will fall back to individual-frame loading, but the shared sub-image metadata (`xpos`/`ypos`/`width`/`height`) is preserved.

---

## Examples

### Example 1: Individual-frame mode (v1.0)

```xml
<?xml version="1.0"?>
<animation id="crate_spin" delay="10">
  <frame source="crate/full_s_000.png"/>
  <frame source="crate/full_s_0001.png" delay="20"/>
  <frame source="crate/full_s_0002.png" delay="40"/>
</animation>
```

### Example 2: Atlas mode (v1.1)

```xml
<?xml version="1.0"?>
<animation atlas="cog.png" width="52" height="52" delay="100">
  <frame source="cog_0001.png" xpos="0"   ypos="0"  delay="40"/>
  <frame source="cog_0002.png" xpos="52"  ypos="0"  delay="40"/>
  <frame source="cog_0003.png" xpos="104" ypos="0"  delay="40"/>
  <frame source="cog_0004.png" xpos="156" ypos="0"  delay="40"/>
  <frame source="cog_0005.png" xpos="208" ypos="0"  delay="40"/>
  <frame source="cog_0006.png" xpos="260" ypos="0"  delay="40"/>
  <frame source="cog_0007.png" xpos="0"   ypos="52" delay="40"/>
  <frame source="cog_0008.png" xpos="52"  ypos="52" delay="40"/>
  <frame source="cog_0009.png" xpos="104" ypos="52" delay="40"/>
  <frame source="cog_0010.png" xpos="156" ypos="52" delay="40"/>
  <frame source="cog_0011.png" xpos="208" ypos="52" delay="40"/>
  <frame source="cog_0012.png" xpos="260" ypos="52" delay="40"/>
</animation>
```

Corresponding atlas: `cog.png` — 312×104 pixels, containing 12 frames arranged in a 6×2 grid, each 52×52 pixels.

### Example 3: `<assets>` wrapper with multiple animations

```xml
<?xml version="1.0"?>
<?fife type="animation"?>
<assets>
  <animation delay="200" x_offset="0" y_offset="-48">
    <frame source="idle.png"/>
  </animation>
  <animation id="walk" delay="150">
    <frame source="walk_0001.png"/>
    <frame source="walk_0002.png"/>
    <frame source="walk_0003.png"/>
  </animation>
</assets>
```

---

## Path Resolution

All file paths in the animation XML are relative to the directory containing the XML file:

```
gui/animations/cog/animation.xml   references
gui/animations/cog/cog.png         ← resolved relative to XML directory
```

At load time, the `AnimationLoader` receives the animation XML path from the VFS.
For each source reference, the parent directory of the XML is prepended before looking up the file.

---

## Compatibility Notes

- **v1.0 files** (without `atlas`) continue to work unchanged — they fall through to the original individual-frame code path.
- **v1.1 files** (with `atlas`) are a strict superset: they add new attributes without removing any. Old loaders will ignore the `atlas`/`xpos`/`ypos` attributes but will fail if the `source` files do not exist as standalone PNGs on disk.
- The `AnimationSaver` does not yet write the `atlas` attribute. Atlas-mode animations that are saved produce individual-frame XML output with `xpos`/`ypos`/`width`/`height` metadata preserved on each `<frame>`, which can still be loaded by the atlas-mode parser (since it reads those attributes from `<frame>` elements).

---

## See Also

- `src/fife/loaders/native/map/animationloader.cpp` — Animation XML loader implementation
- `src/fife/savers/native/map/animationsaver.cpp` — Animation XML saver implementation
- `src/fife/loaders/native/map/ianimationloader.h` — Loader interface
- `src/fife/savers/native/map/ianimationsaver.h` — Saver interface
