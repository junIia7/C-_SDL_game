#include "LWindow.h"

LWindow::LWindow()
{
	mWindow = NULL;
	mWidth = 0;
	mHeight = 0;
}

bool LWindow::init()
{
	mWindow = SDL_CreateWindow( "Kypca4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( mWindow != NULL )
	{
		mWidth = 1920;
		mHeight = 1080;
	}

	return mWindow != NULL;
}

SDL_Renderer* LWindow::createRenderer()
{
	return SDL_CreateRenderer( mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}

void LWindow::handleEvent( SDL_Event& e )
{
	if( e.type == SDL_WINDOWEVENT )
	{
		if( e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			mWidth = e.window.data1;
			mHeight = e.window.data2;
		}
	}
}

void LWindow::free()
{
	if( mWindow != NULL )
	{
		SDL_DestroyWindow( mWindow );
	}

	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth()
{
	return mWidth;
}

int LWindow::getHeight()
{
	return mHeight;
}

