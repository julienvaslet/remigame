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
			set<string> hiddenElements;
		
		public:
			UserInterface();
			virtual ~UserInterface();
			
			void addElement( const string& name, Element * element, bool hidden = false );
			void showElement( const string& name );
			void hideElement( const string& name );
			Element * getElement( const string& name );
			
			virtual void render( unsigned int ticks );
			bool dispatchEvent( const SDL_Event * event );
	};
}

#endif
