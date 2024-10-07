#include "text.h"
#include "stat_graph_el.h"

extern LWindow gWindow;

extern SDL_Renderer * gRenderer;

text::text(std::string FT, unsigned char FS):stat_graph_element() {
    FontType = FT;
    FontSize = FS;
}

text::~text() {
    TTF_CloseFont( Font );
    Font = NULL;
    free();
}

bool text::loadMedia(std::string textureText, SDL_Color textColor) {

    if(Font) {
        TTF_CloseFont( Font );
        Font = NULL;
    }
	free();

	Font = TTF_OpenFont( FontType.c_str(), FontSize );
    if( Font == NULL)
		printf( "Ошибка при загрузке шрифта! SDL_ttf Ошибка: %s\n", TTF_GetError() );
	else
	{
        SDL_Surface* textSurface = TTF_RenderUTF8_Blended_Wrapped( Font, textureText.c_str(), textColor, gWindow.getWidth()); // gWindow&
        if( textSurface == NULL ) {
            //printf( "Невозможно отрендерить текст! SDL_ttf Ошибка: %s\n", TTF_GetError() );
        }
        else
        {
            mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
            if( mTexture == NULL )
                printf( "Невозможно создать текстуру из текста! SDL Ошибка: %s\n", SDL_GetError() );
            else
            {
                mWidth = textSurface->w;
                mHeight = textSurface->h;
            }
            SDL_FreeSurface( textSurface );
        }
	}

	if (mTexture != NULL)
        return true;
	else
        //printf( "Ошибка при загрузке текстуры текста!\n" );
    return false;
}

void text::render(int x, int y, int w, int h) {
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad);
}

void text::setFontType(std::string Font) {
    FontType = Font;
}

void text::setFontSize(unsigned char sizze) {
    FontSize = sizze;
}
