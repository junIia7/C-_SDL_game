#ifndef STAT_GRAPH_ELEMENT_H
#define STAT_GRAPH_ELEMENT_H

#include <SDL.h>

class stat_graph_element
{
    protected:

        SDL_Texture* mTexture;

        int mWidth;
        int mHeight;

    public:

        stat_graph_element();

        virtual ~stat_graph_element();

        int getWidth();

        int getHeight();

        virtual void free();

		void setAlpha( unsigned char a);

		void setColor( unsigned char red, unsigned char green, unsigned char blue );

        virtual void render(int x = 0, int y = 0, int w = 100, int h = 100) = 0;
};

#endif // STAT_GRAPH_ELEMENT_H
