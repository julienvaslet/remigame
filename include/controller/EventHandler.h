#ifndef _CONTROLLER_EVENTHANDLER_H
#define _CONTROLLER_EVENTHANDLER_H	1

namespace controller
{
	class EventHandler;
}

#include <controller/Controller.h>

namespace controller
{
	class EventHandler
	{
		public:
			virtual ~EventHandler(){}
			virtual void handleEvent( Controller * controller, Mapping::Button button, short int value, unsigned int timestamp ) = 0;
	};
}

#endif

