#ifndef BUTTON_H
#define BUTTON_H

#include <string>

#include "stat_graph_el.h"

class button : public stat_graph_element
{
    private:

        SDL_Point mPosition;

    public:

		button();

		~button();

		void setPosition( int x, int y );

		char handleEvent( SDL_Event* e );

		void render(int x = 0, int y = 0, int w = 100, int h = 100) override;

		virtual bool loadMedia(std::string src, SDL_Color Color = {0, 0, 0});

		void disable();
};

#endif // BUTTON_H
