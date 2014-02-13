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
	
	int Frame::getBoundingBoxesCount() const
	{
		return this->boundingBoxes.size();
	}
	
	int Frame::getAttackAreasCount() const
	{
		return this->attackAreas.size();
	}
	
	int Frame::getDefenceAreasCount() const
	{
		return this->defenceAreas.size();
	}
	
	const Box& Frame::getBoundingBox( int index ) const
	{
		return this->boundingBoxes.at( index );
	}
	
	const Box& Frame::getAttackArea( int index ) const
	{
		return this->attackAreas.at( index );
	}
	
	const Box& Frame::getDefenceArea( int index ) const
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
}
