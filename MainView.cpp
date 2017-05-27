#include "MainView.h"

MainView::MainView(const BRect &frame)
	:	BView(frame, "textview", B_FOLLOW_ALL, B_WILL_DRAW)
{
	// the font rasterizing library
    FreeTypeLib lib;
    //clock_t begin, end;

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
}


void
MainView::Draw(BRect update)
{
	BFont font;
	font.SetSize(18.0);
	
	SetFont(&font);
	
	DrawString("This is some text", BPoint(50,50));
}
