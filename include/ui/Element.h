#ifndef __UI_ELEMENT_H
#define __UI_ELEMENT_H  1

#include <graphics/Renderable.h>
#include <map>
#include <string>

using namespace std;

namespace ui
{
	class Element : public Renderable
	{
		protected:
			map<string, void *> events;
			
		public:
			Element();
			virtual ~Element();
			
			void trigger( const string& event );
			virtual void render( unsigned int ticks ) = 0;
	};
}

#endif
