#pragma once

#include "hb.h"
#include "hb-ft.h"
#include "freetypelib.h"
#include <cmath>
#include <vector>
#include <limits>

#include <Bitmap.h>

using namespace std;

typedef struct {
    std::string data;
    std::string language;
    hb_script_t script;
    hb_direction_t direction;
    const char* c_data() { return data.c_str(); };
} HBText;

namespace HBFeature {
    const hb_tag_t KernTag = HB_TAG('k', 'e', 'r', 'n'); // kerning operations
    const hb_tag_t LigaTag = HB_TAG('l', 'i', 'g', 'a'); // standard ligature substitution
    const hb_tag_t CligTag = HB_TAG('c', 'l', 'i', 'g'); // contextual ligature substitution

    static hb_feature_t LigatureOff = { LigaTag, 0, 0, std::numeric_limits<unsigned int>::max() };
    static hb_feature_t LigatureOn  = { LigaTag, 1, 0, std::numeric_limits<unsigned int>::max() };
    static hb_feature_t KerningOff  = { KernTag, 0, 0, std::numeric_limits<unsigned int>::max() };
    static hb_feature_t KerningOn   = { KernTag, 1, 0, std::numeric_limits<unsigned int>::max() };
    static hb_feature_t CligOff     = { CligTag, 0, 0, std::numeric_limits<unsigned int>::max() };
    static hb_feature_t CligOn      = { CligTag, 1, 0, std::numeric_limits<unsigned int>::max() };
}

class HBShaper {
    public:
        HBShaper(const string& fontFile, FreeTypeLib* lib);
        ~HBShaper();

        void init();
        void drawText(HBText& text, const BBitmap* bitmap, float x, float y);
        void addFeature(hb_feature_t feature);

    private:
        FreeTypeLib* lib;
        FT_Face* face;

        hb_font_t* font;
        hb_buffer_t* buffer;
        vector<hb_feature_t> features;
};

HBShaper::HBShaper(const string& fontFile, FreeTypeLib* fontLib) {
    lib = fontLib;
    float size = 50;
    face = lib->loadFace(fontFile, size * 64, 72, 72);
}

void HBShaper::addFeature(hb_feature_t feature) {
    features.push_back(feature);
}
	
void
HBShaper::drawText(HBText& text, const BBitmap* bitmap, float x, float y)
{
	// Directly copies the rasterized glyph bitmap to the bitmap's buffer

	uint8* bits = (uint8*)bitmap->Bits();
	uint32 bytesPerRow = bitmap->BytesPerRow();
	BRect bitmapBounds = bitmap->Bounds();

	// Here is the original code from the test app
	hb_buffer_reset(buffer);

	hb_buffer_set_direction(buffer, text.direction);
	hb_buffer_set_script(buffer, text.script);
	hb_buffer_set_language(buffer,
		hb_language_from_string(text.language.c_str(),
		text.language.size()));
	size_t length = text.data.size();

	hb_buffer_add_utf8(buffer, text.c_data(), length, 0, length);

	// harfbuzz shaping
	hb_shape(font, buffer, features.empty()
		? NULL : &features[0], features.size());

	unsigned int glyphCount;
	hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(
		buffer, &glyphCount);
	hb_glyph_position_t *glyphPos
		= hb_buffer_get_glyph_positions(buffer, &glyphCount);

	for (int i = 0; i < glyphCount; ++i) {
		Glyph* glyph = lib->rasterize(face, glyphInfo[i].codepoint);

		// From the original code...
		// The other variables are probably no longer needed:
		float xa = (float) glyphPos[i].x_advance / 64;
		float ya = (float) glyphPos[i].y_advance / 64;
		float xo = (float) glyphPos[i].x_offset / 64;
		float yo = (float) glyphPos[i].y_offset / 64;
		float x0 = x + xo + glyph->bearing_x;
		float y0 = floor(y + yo + glyph->bearing_y);
		// The libfreetype coordinate system is upside down, so flip y0:
		y0 = bitmapBounds.bottom - y0;

		// Calculate an area for the glyph...
		BRect glyphArea = BRect(0, 0, (int)glyph->width - 1,
			(int)glyph->height - 1);
		// ...move it into the bitmap's coordinate space...
		glyphArea.OffsetTo(x0, y0);
		// ... and intersect it with the bitmap's bounds to find
		// the overlapping area:
		glyphArea = glyphArea & bitmapBounds;
		// Test wether the glyphArea is valid. If it is not valid,
		// there was no overlapping area:
		if (!glyphArea.IsValid())
			continue;

		// Figure out the offset into the memory of the bitmap
		// for the top/left of the glyph area. The bitmap's buffer
		// has "bytesPerRow" bytes in each vertical row of pixels.
		// And it has 4 bytes per pixel, one byte per color component
		// B, G, R, and A (alpha).
		uint8* bitmapStart = bits + (int32)glyphArea.top * bytesPerRow
			+ (int32)glyphArea.left * 4;

		// Since the glyphArea is constrained to the bitmap area,
		// we may not copy all of the glyph's bitmap. Therefor we need
		// to figure out where in the glyph buffer is the pixel that
		// corresponds to the (glyphArea.left, glyphArea.top). The
		// buffer(0, 0) is at (x0, y0), so that is our reference point,
		// the new offset is relative to that.
		// The glyph buffer is monochrome, which means it has one byte
		// per pixel. glyph->width gives the number of bytes per row.
		uint8* glyphStart = glyph->buffer
			+ (int32)(glyphArea.top - y0) * glyph->width
			+ (int32)(glyphArea.left - x0);

		// Copy the rasterized glyph into the bitmap buffer
		for (int iy = (int)glyphArea.top; iy <= (int)glyphArea.bottom; ++iy) {
			// Pixel pointers to the start of the row in each buffer:
			uint8* b = bitmapStart;
			uint8* g = glyphStart;
			// Copy one row of the glyph data while doing a
			// "color space conversion":
			for (int ix = (int)glyphArea.left; ix <= (int)glyphArea.right; ++ix) {
				// Do a "saturated add" operation. The same pixel in the bitmap
				// may be touched by multiple glyphs when rendering text, we
				// let the glyph values add up:
				uint8 value = std::min(255, b[0] + *g);
				b[0] = value;
				b[1] = value;
				b[2] = value;
				b[3] = 255;

				b += 4; // four bytes per pixel
				g += 1; // just one byte per pixel
			}
			// Next row in the bitmap's buffer:
			bitmapStart += bytesPerRow;
			// Next row in the glyph's buffer:
			glyphStart += glyph->width;
		}

		// Advance x and y for the next glyph
		x += xa;
		y += ya;

		lib->freeGlyph(glyph);
	}
}

void HBShaper::init() {
    font = hb_ft_font_create(*face, NULL);
    buffer = hb_buffer_create();

    assert(hb_buffer_allocation_successful(buffer));
}

HBShaper::~HBShaper() {
    lib->freeFace(face);

    hb_buffer_destroy(buffer);
    hb_font_destroy(font);
}
