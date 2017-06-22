/* 
 * The MIT License (MIT)
 * 
 * Copyright 2017, Deepanshu Goyal
 * Copyright 2016, tangrams
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "freetypelib.h"

FreeTypeLib::FreeTypeLib() {
    assert(!FT_Init_FreeType(&lib));
}

FreeTypeLib::~FreeTypeLib() {
    FT_Done_FreeType(lib);
}

FT_Face* FreeTypeLib::loadFace(const string& fontName, int ptSize, int deviceHDPI, int deviceVDPI) {
    FT_Face* face = new FT_Face;

    assert(!FT_New_Face(lib, fontName.c_str(), 0, face));
    assert(!force_ucs2_charmap(*face));
    assert(!FT_Set_Char_Size(*face, 0, ptSize, deviceHDPI, deviceVDPI));

    return face;
}

void FreeTypeLib::freeFace(FT_Face* face) {
    FT_Done_Face(*face);
    delete face;
}

void FreeTypeLib::freeGlyph(Glyph* glyph) {
    delete glyph;
}

int FreeTypeLib::force_ucs2_charmap(FT_Face ftf) {
    for(int i = 0; i < ftf->num_charmaps; i++) {
        if ((  (ftf->charmaps[i]->platform_id == 0)
            && (ftf->charmaps[i]->encoding_id == 3))
           || ((ftf->charmaps[i]->platform_id == 3)
            && (ftf->charmaps[i]->encoding_id == 1))) {
                return FT_Set_Charmap(ftf, ftf->charmaps[i]);
        }
    }
    return -1;
}

Glyph* FreeTypeLib::rasterize(FT_Face* face, uint32_t glyphIndex) const {
    Glyph* g = new Glyph;

    FT_Int32 flags =  FT_LOAD_DEFAULT;

    assert(!FT_Load_Glyph(*face,
        glyphIndex, // the glyph_index in the font file
        flags
    ));

    FT_GlyphSlot slot = (*face)->glyph;
    assert(!FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL));

    FT_Bitmap ftBitmap = slot->bitmap;

    g->buffer = ftBitmap.buffer;
    g->width = ftBitmap.width;
    g->height = ftBitmap.rows;
    g->bearing_x = slot->bitmap_left;
    g->bearing_y = slot->bitmap_top;

    return g;
}
