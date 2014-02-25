#ifndef __UI_ELEMENT_H
#define __UI_ELEMENT_H  1

#include <graphics/Renderable.h>
#include <map>
#include <string>

using namespace std;

namespace ui
{
	class Element;
	typedef bool (*Event)( Element * );
	
	class Element : graphics::Renderable
	{
		protected:
			map<string, vector<Event>> events;
			
		public:
			Element();
			virtual ~Element();
			
			void addEventHandler( const string& event, Event callback );
			void trigger( const string& event );
			virtual void render( unsigned int ticks ) = 0;
	};
}

#endif
