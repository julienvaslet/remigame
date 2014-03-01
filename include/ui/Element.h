#ifndef __UI_ELEMENT_H
#define __UI_ELEMENT_H  1

#include <graphics/Renderable.h>
#include <graphics/Box.h>
#include <map>
#include <vector>
#include <string>

using namespace std;

namespace ui
{
	class Element;
	typedef bool (*Event)( Element * );
	
	class Element : graphics::Renderable
	{
		protected:
			graphics::Box box;
			bool disabledState;
			map<string, vector<Event> > events;
			
		public:
			Element();
			virtual ~Element();
			
			graphics::Box& getBox();
			void addEventHandler( const string& event, Event callback );
			void removeEventHandler( const string& event );
			void trigger( const string& event );
			virtual void render( unsigned int ticks ) = 0;
			bool isDisabled() const;
			void setDisabledState( bool state );
	};
}

#endif
