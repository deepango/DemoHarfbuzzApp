#include "MainView.h"

MainView::MainView(const BRect &frame)
	:	BView(frame, "textview", B_FOLLOW_ALL, B_WILL_DRAW)
{
}


void
MainView::Draw(BRect update)
{
	BFont font;
	font.SetSize(18.0);
	
	SetFont(&font);
	
	DrawString("This is some text", BPoint(50,50));
}
