#include <SDL_image.h>

#include "image.h"

extern SDL_Renderer * gRenderer;

image::image():stat_graph_element() {}

image::~image() {
    free();
}

void image::render(int x, int y, int w, int h) {

	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if( w||h )
	{
		renderQuad.w = w;
		renderQuad.h = h;
	}

	SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad);
}

bool image::loadMedia(std::string path, SDL_Color Color){

	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
		printf( "Невозможно загрузить изображение %s! SDL_image Ошибка: %s\n", path.c_str(), IMG_GetError() );
	else
	{
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) ); // для голубого цвета ??

        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( !newTexture )
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
        printf( "Ошибка при загрузке текстуры картинки!!\n" );
    return false;
}

