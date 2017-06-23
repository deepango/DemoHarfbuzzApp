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

#include "MainView.h"
#include "hbshaper.h"
#include "freetypelib.h"
#include <Bitmap.h>

MainView::MainView(const BRect &frame)
	:	BView(frame, "textview", B_FOLLOW_ALL, B_WILL_DRAW)
{
}


void
MainView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();
	BBitmap bitmap(bounds, B_RGBA32);
	_DrawText(&bitmap);
	DrawBitmap(&bitmap, updateRect, updateRect);
}

void
MainView::_DrawText(const BBitmap* bitmap)
{
	
	FreeTypeLib lib;
	HBShaper latinShaper("fonts/DejaVuSerif.ttf", &lib);
	HBShaper arabicShaper("fonts/amiri-regular.ttf", &lib);
	HBShaper russianShaper("fonts/DejaVuSerif.ttf", &lib);
	HBShaper hanShaper("fonts/fireflysung.ttf", &lib);
	HBShaper hindiShaper("fonts/Sanskrit2003.ttf", &lib);

	latinShaper.init();
	arabicShaper.init();
	russianShaper.init();
	hanShaper.init();
	hindiShaper.init();
	
	HBText hbt1 = {
		"ficellé fffffi. VAV.",
		"fr",
		HB_SCRIPT_LATIN,
		HB_DIRECTION_LTR
		};
		
		HBText hbt2 = {
		"تسجّل يتكلّم",
		"ar",
		HB_SCRIPT_ARABIC,
		HB_DIRECTION_RTL
		};
		
		HBText hbt3 = {
			"Дуо вёжи дёжжэнтиюнт ут",
			"ru",
			HB_SCRIPT_CYRILLIC,
			HB_DIRECTION_LTR
		};
			
		HBText hbt4 = {
			"緳 踥踕",
			"ch",
			HB_SCRIPT_HAN,
			HB_DIRECTION_TTB
		};
			
		HBText hbt5 = {
			"अंतरराष्ट्रीय क्षत्रिय",
			"hi",
			HB_SCRIPT_DEVANAGARI,
			HB_DIRECTION_LTR
		};
				
	latinShaper.addFeature(HBFeature::KerningOn);
	// Draw the glyphs of each shaper, pass the bitmap to draw on
	
	latinShaper.drawText(hbt1, bitmap, 20, 320);
	arabicShaper.drawText(hbt2, bitmap, 20, 220);
	russianShaper.drawText(hbt3, bitmap, 20, 120);
	hanShaper.drawText(hbt4, bitmap, 700, 380);
	hindiShaper.drawText(hbt5, bitmap, 20, 20);
}
