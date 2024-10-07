#include <SDL_image.h>

#include "button.h"

extern SDL_Renderer * gRenderer;

button::button():stat_graph_element() {
    mPosition.x = 0;
    mPosition.y = 0;
}

button::~button(){
    free();
}

void button::setPosition( int x, int y ){
    mPosition.x = x;
    mPosition.y = y;
}

char button::handleEvent(SDL_Event* e) {

	if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP ) {
		int x, y;
		SDL_GetMouseState( &x, &y );

		bool inside = true;

		if( x < mPosition.x )
			inside = false;
		else if( x > mPosition.x + mWidth )
			inside = false;
		else if( y < mPosition.y )
			inside = false;
		else if( y > mPosition.y + mHeight )
			inside = false;

		if( !inside )
		    setColor(255, 255, 255);
		else
		{
			switch( e->type )
			{
				case SDL_MOUSEMOTION:
                setColor(150, 150, 150);
                return 0;
				break;

				case SDL_MOUSEBUTTONDOWN:
                return 1;
				break;
			}
		}
	}
	return 0;
}

void button::render(int x, int y, int w, int h) {
    setPosition(x, y);

	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    if(w||h) {
        mWidth  = renderQuad.w = w;
        mHeight = renderQuad.h = h;
    }

	SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad);
}

bool button::loadMedia(std::string path, SDL_Color Color) {

	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
		printf( "Невозможно загрузить картинку %s! SDL_image Ошибка: %s\n", path.c_str(), IMG_GetError() );
	else
	{
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
			printf( "Невозможно создать текстуру из %s! SDL Ошибка: %s\n", path.c_str(), SDL_GetError() );
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		SDL_FreeSurface( loadedSurface );
	}

	mTexture = newTexture;

	if (mTexture != NULL)
        return true;
	else
        printf( "Ошибка при загрузке текстуры кнопки\n" );
    return false;
}

void button::disable() {
    mPosition = {0, 0};
}

