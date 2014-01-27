#include <controller/Mapping.h>

namespace controller
{
	Mapping::Mapping( const string& filename )
	{
		// load..
	}

	Mapping::Mapping( const char * filename )
	{
		// load..
	}

	Mapping::~Mapping()
	{
	}
	
	bool Mapping::isLoaded()
	{
		return this->mapping.size() > 0;
	}

	Button Mapping::getButton( short int value )
	{
		return Controller::BTNUP;
	}
}
