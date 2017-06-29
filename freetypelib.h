#ifndef FREE_TYPE_LIB_H
#define FREE_TYPE_LIB_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <cassert>
#include <cmath>

#include <SupportDefs.h>

using namespace std;

typedef struct {
	unsigned char* buffer;
	unsigned int width;
	unsigned int height;
	float bearing_x;
	float bearing_y;
} Glyph;

class FreeTypeLib {
public:
	FreeTypeLib();
	~FreeTypeLib();

	FT_Face* loadFace(const char* fontName, int ptSize, int deviceHDPI, int deviceVDPI);
	void freeFace(FT_Face* face);
	Glyph* rasterize(FT_Face* face, uint32 glyphIndex) const;
	void freeGlyph(Glyph* glyph);

private:
	FT_Library lib;
	int force_ucs2_charmap(FT_Face ftf);
};

#endif // FREE_TYPE_LIB_H
