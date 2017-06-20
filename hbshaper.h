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
        

       // void init();
        //void drawText(HBText& text, const BBitmap* bitmap, float x, float y);
       // void addFeature(hb_feature_t feature);

    private:
        FreeTypeLib* lib;
        FT_Face* face;

        hb_font_t* font;
        hb_buffer_t* buffer;
        vector<hb_feature_t> features;
};

void HBShaper::init() {
    font = hb_ft_font_create(*face, NULL);
    buffer = hb_buffer_create();

    assert(hb_buffer_allocation_successful(buffer));
}

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
	uint8* bits = (uint8*)bitmap->Bits();
	uint32 bytesPerRow = bitmap->BytesPerRow();

	// You don't need the GL meshes anymore, you can copy the rasterized
	// glyph directly into the BBitmap's memory. You may have to do a
	// color space conversion, i.e. the glyph's data may be gray scale,
	// but the BBitmap's buffer is BGRA with 8 bits per chanel.

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

		// Figure out the offset into the memory of the bitmap
		// for x and y:
		// This probaly needs to be changed to use x0 and y0 instead
		// of x and y.
		uint8* pixel = bits + (int32)y0 * bytesPerRow + (int32)x0 * 4;

		// Copy the rasterized glyph into the bitmap buffer
		for (int iy = 0; iy < glyph->height; ++iy) {
			uint8* p = pixel;
			// Offset into the rasterized glyph buffer:
			uint8* g = glyph->buffer + iy * glyph->width;

			// Copy one row of the glyph data while doing a
			// "color space conversion":
			for (int ix = 0; iy < glyph->width; ++ix) {
				for(int ixyz=0,int iabc=x0;ixyz<glyph->height,iabc<bitmap->IntegerHeight();ixyz++,iabc++) {
					for(int jxyz=0,int jabc=y0;jxyz<glyph->width,jabc<bitmap->IntegerWidth();jxyz++,jabc++) {
						if(ixyz==iabc && jxyz==jabc) {
							p[0] = *g;
							p[1] = *g;
							p[2] = *g;
							p[3] = 255;
						}
					}
				}
				p += 4; // four bytes per pixel
				g += 1; // probably just one byte per pixel
			}
			// Next row in the bitmap's buffer:
			pixel += bytesPerRow;
		}

		// Advance x and y for the next glyph
		x += xa;
		y += ya;

		lib->freeGlyph(glyph);
	}
}
/*
void HBShaper::init() {
    font = hb_ft_font_create(*face, NULL);
    buffer = hb_buffer_create();

    assert(hb_buffer_allocation_successful(buffer));
}
*/

HBShaper::~HBShaper() {
    lib->freeFace(face);
    hb_buffer_destroy(buffer);
    hb_font_destroy(font);
}
