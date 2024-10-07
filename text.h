#ifndef TEXT_H
#define TEXT_H

#include <string>

#include <SDL_ttf.h>

#include "stat_graph_el.h"
#include "LWindow.h"

class text : public stat_graph_element
{
    private:

        TTF_Font* Font = NULL;

        std::string FontType;

        unsigned char FontSize;
    public:

        text(std::string FT = "courier.ttf", unsigned char FS = 16);

        ~text();

        void setFontType(std::string Font);

        void setFontSize(unsigned char sizze);

		void render(int x = 0, int y = 0, int w = 100, int h = 100) override;

		bool loadMedia(std::string textureText, SDL_Color textColor = {0, 0, 0});
};

#endif // TEXT_H
