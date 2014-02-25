#ifndef __UI_USERINTERFACE_H
#define __UI_USERINTERFACE_H	1

#include <graphics/Renderable.h>
#include <ui/Element.h>

#include <SDL2/SDL.h>
#include <string>
#include <set>
#include <map>

using namespace std;

namespace ui
{
	class UserInterface : public graphics::Renderable
	{
		protected:
			map<string, Element *> elements;
			set<string> mouseoverElements;
		
		public:
			UserInterface();
			virtual ~UserInterface();
			
			void addElement( const string& name, Element * element );
			Element * getElement( const string& name );
			
			virtual void render( unsigned int ticks );
			bool dispatchEvent( const SDL_Event * event );
	};
}

#endif
