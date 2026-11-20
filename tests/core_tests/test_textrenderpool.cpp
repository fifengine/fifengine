// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// 3rd party library includes
#include <catch2/catch_test_macros.hpp>

// Standard C++ library includes
#include <cstddef>
#include <cstdint>
#include <string>

// FIFE includes
#include "fixture.h"
#include "video/fonts/ifont.h"
#include "video/fonts/textrenderpool.h"
#include "video/image.h"

using FIFE::Image;
using FIFE::ImagePtr;
using FIFE::Rect;
using FIFE::TextRenderPool;

namespace
{

    /** Minimal concrete IFont: a real object the pool reads attributes from. */
    class TestFont final : public FIFE::IFont
    {
        public:
            void setRowSpacing(int32_t spacing) override
            {
                m_rowSpacing = spacing;
            }
            int32_t getRowSpacing() const override
            {
                return m_rowSpacing;
            }
            void setGlyphSpacing(int32_t spacing) override
            {
                m_glyphSpacing = spacing;
            }
            int32_t getGlyphSpacing() const override
            {
                return m_glyphSpacing;
            }
            void setAntiAlias(bool antiAlias) override
            {
                m_antiAlias = antiAlias;
            }
            bool isAntiAlias() const override
            {
                return m_antiAlias;
            }
            void setBoldStyle(bool style) override
            {
                m_bold = style;
            }
            bool isBoldStyle() const override
            {
                return m_bold;
            }
            void setItalicStyle(bool style) override
            {
                m_italic = style;
            }
            bool isItalicStyle() const override
            {
                return m_italic;
            }
            void setUnderlineStyle(bool style) override
            {
                m_underline = style;
            }
            bool isUnderlineStyle() const override
            {
                return m_underline;
            }
            void setStrikethroughStyle(bool style) override
            {
                m_strikethrough = style;
            }
            bool isStrikethroughStyle() const override
            {
                return m_strikethrough;
            }
            void setDynamicColoring(bool coloring) override
            {
                m_dynamicColoring = coloring;
            }
            bool isDynamicColoring() const override
            {
                return m_dynamicColoring;
            }
            int32_t getStringIndexAt(std::string const & text, int32_t x) const override
            {
                static_cast<void>(text);
                return x;
            }
            Image* getAsImage(std::string const & text) const override
            {
                static_cast<void>(text);
                return nullptr;
            }
            Image* getAsImageMultiline(std::string const & text) const override
            {
                static_cast<void>(text);
                return nullptr;
            }
            std::string splitTextToWidth(std::string const & text, int32_t render_width) override
            {
                static_cast<void>(render_width);
                return text;
            }
            void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
                m_color = SDL_Color{r, g, b, a};
            }
            SDL_Color getColor() const override
            {
                return m_color;
            }
            int32_t getWidth(std::string const & text) const override
            {
                return static_cast<int32_t>(text.size());
            }
            int getHeight() const override
            {
                return 10;
            }
            void invalidate() override
            {
            }

            SDL_Color m_color{255, 255, 255, 255};
            int32_t m_rowSpacing{0};
            int32_t m_glyphSpacing{0};
            bool m_antiAlias{true};
            bool m_bold{false};
            bool m_italic{false};
            bool m_underline{false};
            bool m_strikethrough{false};
            bool m_dynamicColoring{false};
    };

    /** Minimal concrete Image: a real object that counts invalidate() calls. */
    class TestImage final : public Image
    {
        public:
            TestImage() : Image(static_cast<SDL_Surface*>(nullptr))
            {
            }

            void invalidate() override
            {
                ++m_invalidateCalls;
            }
            void render(Rect const & rect, uint8_t alpha, uint8_t const * rgb) override
            {
                static_cast<void>(rect);
                static_cast<void>(alpha);
                static_cast<void>(rgb);
            }
            void setSurface(SDL_Surface* surface) override
            {
                static_cast<void>(surface);
            }
            void useSharedImage(ImagePtr const & shared, Rect const & region) override
            {
                static_cast<void>(shared);
                static_cast<void>(region);
            }
            void forceLoadInternal() override
            {
            }

            int m_invalidateCalls{0};
    };

    /** Test-only view onto a real TextRenderPool. Pool behavior still comes from the real class. */
    class PoolSpy final : public TextRenderPool
    {
        public:
            using TextRenderPool::TextRenderPool;

            void backdateOldestEntries(std::size_t count)
            {
                // Push the oldest entries' timestamps past the 1 minute retention,
                // so the real removeOldEntries() sweep can expire them.
                auto it = m_pool.end();
                while (count > 0 && it != m_pool.begin()) {
                    --it;
                    it->timestamp -= 120ULL * 1000ULL;
                    --count;
                }
            }

            std::size_t poolSize() const
            {
                return m_pool.size();
            }
    };

} // namespace

struct TextRenderPoolFixture : TestFixture
{
};

TEST_CASE_METHOD(
    TextRenderPoolFixture, "TextRenderPool returns nullptr for text that was never added", "[textrenderpool]")
{
    TextRenderPool pool(4);
    TestFont font;
    CHECK(pool.getRenderedText(&font, "missing") == nullptr);
}

TEST_CASE_METHOD(
    TextRenderPoolFixture, "TextRenderPool returns the same image for repeated lookups", "[textrenderpool]")
{
    TextRenderPool pool(4);
    TestFont font;
    TestImage* image = new TestImage();
    pool.addRenderedText(&font, "hello", image);
    CHECK(pool.getRenderedText(&font, "hello") == image);
    CHECK(pool.getRenderedText(&font, "hello") == image);
}

TEST_CASE_METHOD(TextRenderPoolFixture, "TextRenderPool distinguishes different texts", "[textrenderpool]")
{
    TextRenderPool pool(4);
    TestFont font;
    TestImage* imageA = new TestImage();
    TestImage* imageB = new TestImage();
    pool.addRenderedText(&font, "A", imageA);
    pool.addRenderedText(&font, "B", imageB);
    CHECK(pool.getRenderedText(&font, "A") == imageA);
    CHECK(pool.getRenderedText(&font, "B") == imageB);
}

TEST_CASE_METHOD(
    TextRenderPoolFixture,
    "TextRenderPool keeps separate entries for the same text with different colors",
    "[textrenderpool]")
{
    TextRenderPool pool(4);
    TestFont redFont;
    redFont.setColor(255, 0, 0, 255);
    TestFont blueFont;
    blueFont.setColor(0, 0, 255, 255);
    TestImage* redImage  = new TestImage();
    TestImage* blueImage = new TestImage();
    pool.addRenderedText(&redFont, "same text", redImage);
    pool.addRenderedText(&blueFont, "same text", blueImage);
    CHECK(pool.getRenderedText(&redFont, "same text") == redImage);
    CHECK(pool.getRenderedText(&blueFont, "same text") == blueImage);

    // No pooled entry matches this font: the lookup must miss.
    TestFont otherFont;
    otherFont.setColor(0, 255, 0, 255);
    CHECK(pool.getRenderedText(&otherFont, "same text") == nullptr);
}

TEST_CASE_METHOD(
    TextRenderPoolFixture, "TextRenderPool returns nullptr when font attributes do not match", "[textrenderpool]")
{
    TextRenderPool pool(4);
    TestFont font;
    font.setGlyphSpacing(3);
    TestImage* image = new TestImage();
    pool.addRenderedText(&font, "A", image);

    // Same text, but glyph spacing differs from the pooled entry.
    TestFont otherFont;
    otherFont.setGlyphSpacing(0);
    CHECK(pool.getRenderedText(&otherFont, "A") == nullptr);
}

TEST_CASE_METHOD(
    TextRenderPoolFixture,
    "TextRenderPool stays bounded and evicts the least recently used entries",
    "[textrenderpool]")
{
    PoolSpy pool(4);
    TestFont font;
    TestImage* a = new TestImage();
    TestImage* b = new TestImage();
    TestImage* c = new TestImage();
    TestImage* d = new TestImage();
    TestImage* e = new TestImage();
    TestImage* f = new TestImage();
    pool.addRenderedText(&font, "A", a);
    pool.addRenderedText(&font, "B", b);
    pool.addRenderedText(&font, "C", c);
    pool.addRenderedText(&font, "D", d);
    pool.addRenderedText(&font, "E", e);
    pool.addRenderedText(&font, "F", f);

    CHECK(pool.poolSize() == 4);
    CHECK(pool.getRenderedText(&font, "A") == nullptr);
    CHECK(pool.getRenderedText(&font, "B") == nullptr);
    CHECK(pool.getRenderedText(&font, "C") == c);
    CHECK(pool.getRenderedText(&font, "D") == d);
    CHECK(pool.getRenderedText(&font, "E") == e);
    CHECK(pool.getRenderedText(&font, "F") == f);
}

TEST_CASE_METHOD(TextRenderPoolFixture, "TextRenderPool refreshes recency on hit when evicting", "[textrenderpool]")
{
    PoolSpy pool(4);
    TestFont font;
    TestImage* a = new TestImage();
    TestImage* b = new TestImage();
    TestImage* c = new TestImage();
    TestImage* d = new TestImage();
    TestImage* e = new TestImage();
    pool.addRenderedText(&font, "A", a);
    pool.addRenderedText(&font, "B", b);
    pool.addRenderedText(&font, "C", c);
    pool.addRenderedText(&font, "D", d);

    // Refresh A: it becomes most recently used.
    CHECK(pool.getRenderedText(&font, "A") == a);

    // B is now the least recently used: adding E evicts B, not A.
    pool.addRenderedText(&font, "E", e);
    CHECK(pool.getRenderedText(&font, "A") == a);
    CHECK(pool.getRenderedText(&font, "B") == nullptr);
    CHECK(pool.getRenderedText(&font, "C") == c);
    CHECK(pool.getRenderedText(&font, "D") == d);
    CHECK(pool.getRenderedText(&font, "E") == e);
}

TEST_CASE_METHOD(TextRenderPoolFixture, "TextRenderPool removeOldEntries drops expired entries", "[textrenderpool]")
{
    PoolSpy pool(4);
    TestFont font;
    TestImage* a = new TestImage();
    TestImage* b = new TestImage();
    pool.addRenderedText(&font, "A", a);
    pool.addRenderedText(&font, "B", b);

    // Backdate the oldest entry beyond the 1 minute retention, then sweep.
    pool.backdateOldestEntries(1);
    pool.removeOldEntries();

    CHECK(pool.getRenderedText(&font, "A") == nullptr);
    CHECK(pool.getRenderedText(&font, "B") == b);
}

TEST_CASE_METHOD(
    TextRenderPoolFixture, "TextRenderPool invalidateCachedText invalidates all pooled images", "[textrenderpool]")
{
    TextRenderPool pool(4);
    TestFont font;
    TestImage* a = new TestImage();
    TestImage* b = new TestImage();
    pool.addRenderedText(&font, "A", a);
    pool.addRenderedText(&font, "B", b);
    CHECK(a->m_invalidateCalls == 0);
    CHECK(b->m_invalidateCalls == 0);

    pool.invalidateCachedText();

    CHECK(a->m_invalidateCalls == 1);
    CHECK(b->m_invalidateCalls == 1);
}

TEST_CASE_METHOD(TextRenderPoolFixture, "TextRenderPool with zero-sized pool keeps it empty", "[textrenderpool]")
{
    PoolSpy pool(0);
    TestFont font;
    TestImage* image = new TestImage();
    pool.addRenderedText(&font, "A", image);
    CHECK(pool.poolSize() == 0);
    CHECK(pool.getRenderedText(&font, "A") == nullptr);
}

TEST_CASE_METHOD(TextRenderPoolFixture, "TextRenderPool handles duplicate adds explicitly", "[textrenderpool]")
{
    PoolSpy pool(4);
    TestFont font;
    TestImage* first  = new TestImage();
    TestImage* second = new TestImage();
    pool.addRenderedText(&font, "A", first);
    pool.addRenderedText(&font, "A", second);

    // One list entry per map key: the duplicate add replaced the first image.
    CHECK(pool.poolSize() == 1);
    CHECK(pool.getRenderedText(&font, "A") == second);
}

TEST_CASE_METHOD(TextRenderPoolFixture, "TextRenderPool treats alpha as part of the key", "[textrenderpool]")
{
    TextRenderPool pool(4);
    TestFont opaqueFont;
    opaqueFont.setColor(255, 0, 0, 255);
    TestFont transparentFont;
    transparentFont.setColor(255, 0, 0, 0);
    TestImage* opaqueImage      = new TestImage();
    TestImage* transparentImage = new TestImage();
    pool.addRenderedText(&opaqueFont, "A", opaqueImage);
    pool.addRenderedText(&transparentFont, "A", transparentImage);

    // The key includes alpha: each query returns its own entry.
    CHECK(pool.getRenderedText(&opaqueFont, "A") == opaqueImage);
    CHECK(pool.getRenderedText(&transparentFont, "A") == transparentImage);

    // A color that differs in rgb is still a miss.
    TestFont otherFont;
    otherFont.setColor(0, 0, 255, 255);
    CHECK(pool.getRenderedText(&otherFont, "A") == nullptr);
}

TEST_CASE_METHOD(
    TextRenderPoolFixture, "TextRenderPool removeOldEntries removes multiple expired entries", "[textrenderpool]")
{
    PoolSpy pool(10);
    TestFont font;
    TestImage* a = new TestImage();
    TestImage* b = new TestImage();
    TestImage* c = new TestImage();
    pool.addRenderedText(&font, "A", a);
    pool.addRenderedText(&font, "B", b);
    pool.addRenderedText(&font, "C", c);

    // Backdate the two oldest entries beyond the 1 minute retention.
    pool.backdateOldestEntries(2);
    pool.removeOldEntries();

    CHECK(pool.getRenderedText(&font, "A") == nullptr);
    CHECK(pool.getRenderedText(&font, "B") == nullptr);
    CHECK(pool.getRenderedText(&font, "C") == c);
}

TEST_CASE_METHOD(TextRenderPoolFixture, "TextRenderPool lookup still works after invalidation", "[textrenderpool]")
{
    TextRenderPool pool(4);
    TestFont font;
    TestImage* a = new TestImage();
    TestImage* b = new TestImage();
    pool.addRenderedText(&font, "A", a);
    pool.addRenderedText(&font, "B", b);
    pool.invalidateCachedText();
    CHECK(a->m_invalidateCalls == 1);
    CHECK(b->m_invalidateCalls == 1);

    // Entries remain indexed and retrievable after invalidation.
    CHECK(pool.getRenderedText(&font, "A") == a);
    CHECK(pool.getRenderedText(&font, "B") == b);
}
