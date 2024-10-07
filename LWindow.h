#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>

class LWindow
{
	public:

		LWindow();

		bool init();

		SDL_Renderer* createRenderer();

		void handleEvent( SDL_Event& e );

		void free();

		int getWidth();
		int getHeight();

	private:

		SDL_Window* mWindow;

		int mWidth;
		int mHeight;
};

#endif // WINDOW
