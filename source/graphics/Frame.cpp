#include <graphics/Frame.h>

using namespace std;

namespace graphics
{
	Frame::Frame() : box(0,0,0,0), anchor(0,0)
	{
	}
	
	Frame::Frame( const Box& box ) : box(box), anchor(0,box.getHeight())
	{
	}
	
	Frame::Frame( const Frame& frame ) : box(frame.box), anchor(frame.anchor)
	{	
		// Import bounding boxes
		for( vector<Box>::const_iterator it = frame.boundingBoxes.begin() ; it != frame.boundingBoxes.end() ; it++ )
			this->boundingBoxes.push_back( *it );
		
		// Import attack areas
		for( vector<Box>::const_iterator it = frame.attackAreas.begin() ; it != frame.attackAreas.end() ; it++ )
			this->attackAreas.push_back( *it );
		
		// Import defence areas
		for( vector<Box >::const_iterator it = frame.defenceAreas.begin() ; it != frame.defenceAreas.end() ; it++ )
			this->defenceAreas.push_back( *it );
	}
	
	Frame::~Frame()
	{
	}
	
	Box& Frame::getBox()
	{
		return this->box;
	}
	
	Point& Frame::getAnchor()
	{
		return this->anchor;
	}
	
	unsigned int Frame::getBoundingBoxesCount() const
	{
		return this->boundingBoxes.size();
	}
	
	unsigned int Frame::getAttackAreasCount() const
	{
		return this->attackAreas.size();
	}
	
	unsigned int Frame::getDefenceAreasCount() const
	{
		return this->defenceAreas.size();
	}
	
	Box& Frame::getBoundingBox( int index )
	{
		return this->boundingBoxes.at( index );
	}
	
	Box& Frame::getAttackArea( int index )
	{
		return this->attackAreas.at( index );
	}
	
	Box& Frame::getDefenceArea( int index )
	{
		return this->defenceAreas.at( index );
	}
	
	void Frame::addBoundingBox( const Box& box )
	{
		this->boundingBoxes.push_back( box );
	}
	
	void Frame::addAttackArea( const Box& box )
	{
		this->attackAreas.push_back( box );
	}
	
	void Frame::addDefenceArea( const Box& box )
	{
		this->defenceAreas.push_back( box );
	}
	
	void Frame::removeBoundingBox( int index )
	{
		unsigned int i = 0;
		vector<Box>::iterator it;
		
		for( it = this->boundingBoxes.begin() ; it != this->boundingBoxes.end() ; it++ )
		{
			if( i++ == index )
				break;
		}
		
		if( it != this->boundingBoxes.end() )
			this->boundingBoxes.erase( it );
	}
	
	void Frame::removeAttackArea( int index )
	{
		unsigned int i = 0;
		vector<Box>::iterator it;
		
		for( it = this->attackAreas.begin() ; it != this->attackAreas.end() ; it++ )
		{
			if( i++ == index )
				break;
		}
		
		if( it != this->attackAreas.end() )
			this->attackAreas.erase( it );
	}
	
	void Frame::removeDefenceArea( int index )
	{
		unsigned int i = 0;
		vector<Box>::iterator it;
		
		for( it = this->defenceAreas.begin() ; it != this->defenceAreas.end() ; it++ )
		{
			if( i++ == index )
				break;
		}
		
		if( it != this->defenceAreas.end() )
			this->defenceAreas.erase( it );	
	}
}
