#include <graphics/Screen.h>

#ifdef DEBUG0
#include <iostream>
using namespace std;
#endif

namespace graphics
{
	Screen * Screen::instance = NULL;

	Screen::Screen() : window(NULL), renderer(NULL)
	{
	}

	Screen::~Screen()
	{
		if( this->renderer != NULL )
		{	
			SDL_DestroyRenderer( this->renderer );
		
			#ifdef DEBUG0
			cout << "[Screen] Renderer destroyed." << endl;
			#endif
		}
		
		if( this->window != NULL )
		{
			SDL_DestroyWindow( this->window );
		
			#ifdef DEBUG0
			cout << "[Screen] Window destroyed." << endl;
			#endif
		}
	}
		
	bool Screen::initialize( const char * title, int width, int height )
	{
		bool success = true;
		SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK );
	
		Screen * screen = new Screen();

		screen->window = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			( width == 0 || height == 0 ) ? SDL_WINDOW_FULLSCREEN : 0
		);
	
		// do no specify width & height and put in fullscreen
		// then => SDL_RenderSetLogicalSize( renderer, 800, 600 );

		if( screen->window == NULL )
		{
			#ifdef DEBUG0
			cout << "[Screen] Unable to create window: " << SDL_GetError() << endl;
			#endif
		
			success = false;
			delete screen;
		}
		else
		{
			#ifdef DEBUG0
			cout << "[Screen] Window created." << endl;
			#endif
		
			screen->renderer = SDL_CreateRenderer( screen->window, -1, 0 );
	
			if( screen->renderer == NULL )
			{
				#ifdef DEBUG0
				cout << "[Screen] Unable to create the renderer: " << SDL_GetError() << endl;
				#endif
		
				success = false;
				delete screen;
			}
			else
			{
				#ifdef DEBUG0
				cout << "[Screen] Renderer created." << endl;
				#endif
			
				screen->clear();
				screen->render();
			
				Screen::instance = screen;
			}
		}
	
		return success;
	}

	Screen * Screen::get()
	{
		return Screen::instance;
	}

	void Screen::destroy()
	{
		Screen * screen = Screen::get();
		delete screen;
		Screen::instance = NULL;
	
		SDL_Quit();
	}

	SDL_Renderer * Screen::getRenderer()
	{
		return this->renderer;
	}

	void Screen::clear()
	{
		this->resetRenderDrawColor();
		SDL_RenderClear( this->renderer );
	}

	void Screen::render()
	{
		SDL_RenderPresent( this->renderer );
	}
	
	void Screen::resetRenderColor()
	{
		SDL_SetRenderDrawColor( this->renderer, 0, 0, 0, 255 );
	}
	
	void Screen::setRenderColor( Color& color )
	{
		SDL_SetRenderDrawColor( this->renderer, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() );
	}
}
