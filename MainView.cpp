#include "MainView.h"

MainView::MainView(const BRect &frame)
	:	BView(frame, "textview", B_FOLLOW_ALL, B_WILL_DRAW)
{
}


void
MainView::Draw(BRect updateRect)
{
    // Get the current size of the view
    BRect bounds = Bounds();

    // Use it to allocate a BBitmap on the
    // stack that is the size of the view
    BBitmap bitmap(bounds, B_RGBA32);

    // Write the demo text into the bitmaps memory:
    _DrawText(&bitmap);

    // Draw the bitmap on the view's surface
    // see https://www.haiku-os.org/legacy-docs/bebook/BView.html#BView_Drawing
    DrawBitmap(&bitmap, updateRect, updateRect);
}

void
MainView::_DrawText(const BBitmap* bitmap)
{
    // the font rasterizing library
    FreeTypeLib lib;

    HBShaper latinShaper("DejaVuSerif.ttf", &lib);
    HBShaper arabicShaper("amiri-regular.ttf", &lib);
    HBShaper russianShaper("DejaVuSerif.ttf", &lib);
    HBShaper hanShaper("fireflysung.ttf", &lib);
    HBShaper hindiShaper("Sanskrit2003.ttf", &lib);

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
        "हालाँकि प्रचलित रूप पूजा",
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
