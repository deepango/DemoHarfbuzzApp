#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <view.h>

class MainView : public BView
{
public:
					MainView(const Brect &frame)
		void		Draw(BRect update);
};

#endif
