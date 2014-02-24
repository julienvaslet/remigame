#ifndef __UI_ELEMENT_H
#define __UI_ELEMENT_H  1

#include <graphics/Renderable.h>
#include <map>
#include <string>

using namespace std;

namespace ui
{
	typedef void (*Event)( Element * );
	
	class Element : public Renderable
	{
		protected:
			map<string, Event> events;
			
		public:
			Element();
			virtual ~Element();
			
			void setEvent( const string& event, Event callback );
			void trigger( const string& event );
			virtual void render( unsigned int ticks ) = 0;
	};
}

#endif
