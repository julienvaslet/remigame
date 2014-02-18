#ifndef __GAME_OBJECT_EVENT_HANDLER_H
#define __GAME_OBJECT_EVENT_HANDLER_H	1

#include <controller/EventHandler.h>
#include <graphics/Object.h>

using namespace controller;
using namespace graphics;

namespace game
{
	class ObjectEventHandler : public EventHandler
	{
		protected:
			Object * object;
			
		public:
			ObjectEventHandler( Object * object );
			virtual ~ObjectEventHandler();
			
			virtual void handleEvent( Controller * controller, Mapping::Button button, short int value, unsigned int timestamp );
	};
}

#endif

