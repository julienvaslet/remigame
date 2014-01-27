#ifndef	_CONTROLLER_MAPPING_H
#define _CONTROLLER_MAPPING_H	1

#ifdef DEBUG0
#include <iostream>
#endif

#include <map>
#include <string>
using namespace std;

namespace controller
{
	class Mapping
	{
		public:
			// enum class Button : short int ?
			enum Button
			{
				BTNUP,
				BTNRIGHT,
				BTNDOWN,
				BTNLEFT,
				LT1,
				RT1,
				LT2,
				RT2,
				LT3,
				RT3,
				SELECT,
				START,
				AXH,
				AXV
			};
			
		protected:
			map<short int value, Button> mapping;
		
		public:
			Mapping( const string& filename );
			Mapping( const char * filename );
			~Mapping();
			
			bool isLoaded();
			Button getButton( short int value );
	};
}

#endif
