#ifndef IMAGE_H
#define IMAGE_H

#include <string>

#include "stat_graph_el.h"

class image : public stat_graph_element
{

    public:
        image();

        ~image();

		void render(int x = 0, int y = 0, int w = 0, int h = 0) override;

		bool loadMedia(std::string path, SDL_Color Color = {0, 0, 0});
};

#endif // IMAGE_H
