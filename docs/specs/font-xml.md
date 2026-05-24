# FIFE Font Manifest XML Format Specification

**Version:** 1.0
**Date:** 2026-05-24
**Status:** Adopted

---

## Changelog

| Version | Date       | Description |
|---------|------------|-------------|
| 1.0     | 2026-05-24 | Initial format. Family/face structure with TrueType and bitmap support. Loaded at engine startup from `config/fonts.xml`. |

---

## Overview

FIFE uses XML files to define font families and their associated face files (TrueType or bitmap).
A font manifest binds logical family identifiers (e.g., `"UI"`, `"FreeSans"`) to physical font files
on disk or in the VFS. These family IDs are then used by the GUI layer (`guimanager.createFont("FreeSans", 12)`)
to select and instantiate fonts.

The manifest is optional. If `config/fonts.xml` is not found in the VFS at engine startup, it is silently
skipped. Fonts can still be loaded programmatically via `FontManager::setFallbackFont()` and
the `FontPathResolver` fallback in `FifechanManager::createFont()`.

---

## File Structure

```xml
<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">fonts/FreeSans.ttf</face>
    <face weight="700">fonts/FreeSansBold.ttf</face>
    <!-- <fallback>CJK</fallback>  (reserved, see Compatibility Notes) -->
  </family>
  <family id="Mono">
    <face weight="400">fonts/FreeMono.ttf</face>
  </family>
</fonts>
```

## Full example from the pychan demo (`demos/pychan/config/fonts.xml`)

```xml
<?xml version="1.0"?>
<fonts version="1">
  <family id="FreeMono">
    <face type="truetype" weight="400" size="12" antialias="true" color="255,255,255">fonts/freefont/FreeMono.ttf</face>
  </family>
  <family id="FreeSans">
    <face type="truetype" weight="400" size="12" antialias="true" color="255,255,255">fonts/freefont/FreeSans.ttf</face>
  </family>
  <family id="samanata_large_pixelated">
    <face type="truetype" weight="400" size="30" antialias="0" color="255,255,0">fonts/samanata.ttf</face>
  </family>
  <family id="samanata_large">
    <face type="truetype" weight="400" size="20" antialias="1" color="255,255,100">fonts/samanata.ttf</face>
  </family>
  <family id="samanata_large_recoloring">
    <face type="truetype" weight="400" size="20" antialias="1" color="255,255,100" recoloring="1">fonts/samanata.ttf</face>
  </family>
  <family id="samanata_small_pixelated">
    <face type="truetype" weight="400" size="8" antialias="0" color="255,255,100" row_spacing="50" glyph_spacing="50">fonts/samanata.ttf</face>
  </family>
  <family id="samanata_small">
    <face type="truetype" weight="400" size="10" antialias="1" color="255,255,100">fonts/samanata.ttf</face>
  </family>
</fonts>
```

---

## Root: `<fonts>` Element

| Attribute  | Type   | Required | Default | Description |
|------------|--------|----------|---------|-------------|
| `version`  | string | No       | —       | Format version identifier (`"1"`). If missing, a warning is logged. Reserved for future format evolution. |

---

## Family: `<family>` Element

A logical grouping of font faces. A manifest may contain zero or more `<family>` elements.

| Attribute | Type   | Required | Default | Description |
|-----------|--------|----------|---------|-------------|
| `id`      | string | Yes      | —       | Unique family identifier (e.g., `"FreeSans"`, `"UI"`). Used as the `familyId` argument in `guimanager.createFont()` / `FontManager::getFontHandle()`. Families without an `id` attribute are silently skipped. |

---

## Face: `<face>` Element

Defines a single font face file (TrueType TTF/TTC or bitmap image) within a family.
A family may contain multiple `<face>` elements to provide different weights and styles.

| Attribute       | Type    | Required | Default            | Description |
|-----------------|---------|----------|--------------------|-------------|
| `type`          | string  | No       | `"truetype"`       | Font type. `"truetype"` for TTF/TTC files, `"bitmap"` for glyph sheet images. |
| `weight`        | int     | No       | `400`              | Font weight. Numeric values: `100` (Thin), `300` (Light), `400` (Regular), `500` (Medium), `600` (SemiBold), `700` (Bold), `900` (Black). The internal ID is constructed as `familyId/weight` (e.g., `"UI/400"`) and must be unique across the entire manifest — duplicates raise an error. |
| `size`          | int     | No       | `12`               | Point size for the font face. |
| `antialias`     | bool    | No       | `true`             | Enable anti-aliased rendering. Accepts `"true"`/`"1"` or `"false"`/`"0"`. |
| `row_spacing`   | int     | No       | `0`                | Additional vertical spacing between text lines, in pixels. |
| `glyph_spacing` | int     | No       | `0`                | Additional horizontal spacing between glyphs, in pixels. |
| `color`         | string  | No       | `"255,255,255"`    | Default font color. Format: `"R,G,B"` or `"R,G,B,A"` where each component is 0–255. |
| `recoloring`    | bool    | No       | `false`            | Enable per-widget color overrides via `setColor()`. Accepts `"true"`/`"1"` or `"false"`/`"0"`. |
| `glyphs`        | string  | No*      | *(empty)*          | **Required for `type="bitmap"` only.** The glyph string that maps character positions in the bitmap image. |

**Text content** (required, non-empty): Path to the font source file (TTF, TTC, or bitmap image).
This path is resolved through the `AssetResolver` chain at manifest load time.

---

## Internal ID Construction

Each `<face>` receives an internal ID of the form `<familyId>/<weight>`:

```xml
<family id="UI">
  <face weight="400">fonts/FreeSans.ttf</face>
  <face weight="700">fonts/FreeSansBold.ttf</face>
</family>
```

Produces IDs `UI/400` and `UI/700`. These IDs must be unique across the entire document — duplicates raise a `FIFE::Duplicate` error at parse time.

---

## Loading Behavior

1. **Engine startup** — `Engine::init()` checks VFS for `config/fonts.xml`. If found, reads the raw data and calls `FontManager::loadManifestFromString()`.
2. **XML parsing** — `FontDefinitionLoader::loadFromXml()` parses the XML into a list of `FontDefinition` structs.
3. **Asset resolution** — For each `<face>`, the `AssetResolver` resolves the font source path via two providers in priority order:
   - **FilesystemAssetProvider (priority 200):** Searches each directory in `FontPaths` (set via `engine.getSettings().setFontPaths()`). If the source is an absolute path and the file exists on the native filesystem, it matches.
   - **VfsAssetProvider (priority 100):** Falls back to VFS lookup via `VFS::exists(source)`. The source path is relative to the VFS root.
4. **Family registration** — Each resolved face is wrapped in a `FontFace` object (either `TrueTypeFontFace` or `ImageFontFace`) and registered into a `FontFamily` within the `FontManager`.
5. **Runtime lookup** — When the GUI calls `FifechanManager::createFont("FreeSans", 12)`, it dispatches to `FontManager::getFontHandle("FreeSans", 12)`, which selects the best matching face and returns a cached `FontInstance`.

### Face Selection Algorithm

When `getFontHandle(familyId, size, weight, italic)` is called, `FontFamily::selectFace()` searches faces in this priority order:

1. Exact match on weight + italic + codepoint support
2. Exact weight, any italic, codepoint support
3. Any weight, any italic, codepoint support
4. First registered face (fallback for missing codepoints)

---

## Saving Behavior

There is currently no font manifest saver. Font manifests are authored by hand or generated externally.
The `FontManager` state (loaded families, cached instances) is runtime-only and is not serialized.

---

## Path Resolution

Font source paths within `<face>` elements are resolved relative to the VFS root or the `FontPaths`
directories, depending on which `AssetProvider` matches first.

```
config/fonts.xml                              ← manifest location (VFS)
fonts/freefont/FreeSans.ttf                   ← face source (resolved via AssetResolver)
```

At load time, the `FontManager` receives the resolved `AssetHandle` from the `AssetResolver`.
The source path is **not** resolved relative to the manifest XML's directory — it is resolved
against the global VFS and the engine's `FontPaths` setting.

---

## Compatibility Notes

- **`<fallback>` element:** The `<fallback>` element is declared in test data (`tests/data/fonts_test.xml`)
  and supported by the `FontFamily::addFallback()` C++ API, but it is **not yet parsed** from XML by
  `FontDefinitionLoader`. Fallback families must currently be set programmatically via
  `FontManager::setFallbackFont(familyId)`. When implemented, the intended format is:

  ```xml
  <family id="UI">
    <face weight="400">fonts/FreeSans.ttf</face>
    <fallback>CJK</fallback>
    <fallback>Emoji</fallback>
  </family>
  ```

- **v1.0 files** — the current and only version. No backward compatibility concerns yet.
- **Python SWIG gap:** `FontManager` is not exposed to Python. Manifests can only be loaded
  via the hardcoded `config/fonts.xml` path at engine init, or from C++ code. Python applications
  cannot load custom manifests dynamically.

---

## Examples

### Minimal

```xml
<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">fonts/FreeSans.ttf</face>
  </family>
</fonts>
```

### Multiple families with attributes (from `demos/pychan/config/fonts.xml`)

```xml
<?xml version="1.0"?>
<fonts version="1">
  <family id="FreeMono">
    <face type="truetype" weight="400" size="12" antialias="true" color="255,255,255">fonts/freefont/FreeMono.ttf</face>
  </family>
  <family id="FreeSans">
    <face type="truetype" weight="400" size="12" antialias="true" color="255,255,255">fonts/freefont/FreeSans.ttf</face>
  </family>
  <family id="samanata_large_pixelated">
    <face type="truetype" weight="400" size="30" antialias="0" color="255,255,0">fonts/samanata.ttf</face>
  </family>
  <family id="samanata_large">
    <face type="truetype" weight="400" size="20" antialias="1" color="255,255,100">fonts/samanata.ttf</face>
  </family>
  <family id="samanata_large_recoloring">
    <face type="truetype" weight="400" size="20" antialias="1" color="255,255,100" recoloring="1">fonts/samanata.ttf</face>
  </family>
  <family id="samanata_small_pixelated">
    <face type="truetype" weight="400" size="8" antialias="0" color="255,255,100" row_spacing="50" glyph_spacing="50">fonts/samanata.ttf</face>
  </family>
  <family id="samanata_small">
    <face type="truetype" weight="400" size="10" antialias="1" color="255,255,100">fonts/samanata.ttf</face>
  </family>
</fonts>
```

### Bitmap font (from `tests/data/fonts_test.xml`)

```xml
<?xml version="1.0"?>
<fonts version="1">
  <family id="RPGDefault">
    <face type="bitmap" weight="400"
          glyphs=" !&quot;#$%&amp;'()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    >fonts/rpgfont.png</face>
  </family>
</fonts>
```

---

## Error Handling

| Condition | Exception |
|-----------|-----------|
| Empty XML content | `FIFE::InvalidFormat` |
| Malformed XML | `FIFE::InvalidFormat` |
| Root element is not `<fonts>` | `FIFE::InvalidFormat` |
| Empty face source text | `FIFE::InvalidFormat` |
| Duplicate internal ID (`familyId/weight`) | `FIFE::Duplicate` |
| File open failure (in `loadManifest()` only) | `FIFE::InvalidFormat` |

---

## See Also

- `src/fife/video/fonts/fontdefinitionloader.cpp` — Font XML parser implementation
- `src/fife/video/fonts/fontdefinitionloader.h` — Parser interface
- `src/fife/video/fonts/fontmanager.cpp` — Family registration and face selection
- `src/fife/video/fonts/fonttypes.h` — `FontDefinition`, `FontType`, `FontWeight` structs
- `src/fife/video/fonts/fontfamily.cpp` — `FontFamily::selectFace()` algorithm
- `src/fife/video/fonts/assetresolver.cpp` — Source path resolution chain
- `src/fife/controller/engine.cpp` (lines 193–205) — Engine init loading of `config/fonts.xml`
- `demos/pychan/config/fonts.xml` — Reference manifest used by the pychan demo
- `tests/data/fonts_test.xml` — Test data manifest with bitmap font and fallback declarations
