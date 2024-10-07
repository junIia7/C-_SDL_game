#include "stat_graph_el.h"

stat_graph_element::stat_graph_element() {

	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

stat_graph_element::~stat_graph_element() {
    free();
}

void stat_graph_element::free() {
    if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void stat_graph_element::setAlpha(unsigned char a) {
	SDL_SetTextureAlphaMod( mTexture, a );
}

void stat_graph_element::setColor(unsigned char red, unsigned char green, unsigned char blue) {
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

int stat_graph_element::getWidth() {
    return mWidth;
}

int stat_graph_element::getHeight() {
    return mHeight;
}
