#ifndef __UI_USERINTERFACE_H
#define __UI_USERINTERFACE_H	1

#include <graphics/Renderable.h>
#include <ui/Element.h>

#include <SDL2/SDL.h>
#include <string>

using namespace std;

namespace ui
{
	class UserInterface : public graphics::Renderable
	{
		protected:
			map<string, Element> elements;
		
		public:
			UserInterface();
			virtual ~UserInterface();
			
			void addElement( const string& name, const Element& element );
			Element& getElement( const string& name );
			
			virtual void render( unsigned int ticks );
			bool dispatchEvent( const SDL_Event * event );
	};
}

#endif
