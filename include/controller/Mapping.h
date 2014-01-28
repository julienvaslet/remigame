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
			enum Button
			{
				NOBTN,
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
			// Button ID => ( Button Action, default value )
			map<int, pair<Button,short int> > mapping;
		
		public:
			Mapping( const string& filename );
			Mapping( const char * filename );
			~Mapping();
			
			bool load( const char * filename );
			bool isLoaded();
			Mapping::Button getButton( int value );
	};
}

#endif
