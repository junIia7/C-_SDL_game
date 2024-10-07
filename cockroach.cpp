#include <SDL_image.h>

#include "cockroach.h"

extern SDL_Renderer * gRenderer;

running_cockroach::running_cockroach(char alias[25], unsigned int ParticipNumb, unsigned int WinNumb):upravl_perem_po_ekr_el(){
    setInfo(alias, ParticipNumb, WinNumb);
}

void running_cockroach::render(int x, int y, int w, int h) {

	SDL_Rect renderQuad = { mPosX, mPosY, mWidth, mHeight };
    if(w || h) {
        mWidth  = renderQuad.w = w;
        mHeight = renderQuad.h = h;
    }

    if(x || y) {
        renderQuad = { x, y, mWidth, mHeight };
        mPosX = x;
        mPosY = y;
    }

	SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad);
}

void running_cockroach::move() {
    mPosX += mVelX;
}

int running_cockroach::getPosX(){
    return mPosX;
}

bool running_cockroach::loadMedia(std::string path, SDL_Color Color) {

	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Невозможно загрузить изображение %s! SDL_image Ошибка: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) ); // для голубого цвета ??

        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Невозможно создать текстуру из %s! SDL Ошибка: %s\n", path.c_str(), SDL_GetError() );
		}
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
        printf( "Ошибка при загрузке текстуры кнопки!\n" );
    return false;
}

void running_cockroach::readData (const char *filename, char index) {

    FILE* file = fopen(filename, "rb");
    if (!file){
        fprintf(stderr, "Ошибка открытия файла.\n");
        return;
    }

    long offset = sizeof(data) * index;

    fseek(file, offset, SEEK_SET);

    fread(&info, sizeof(struct data), 1, file);

    fclose(file);
}

void running_cockroach::GameOver(char a) {
    info.ParticipNumb++;
    if(a)
        info.WinNumb++;
}
