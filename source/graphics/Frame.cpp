#include <graphics/Frame.h>

using namespace std;

namespace graphics
{
	Frame::Frame() : box(0,0,0,0), anchor(0,0)
	{
	}
	
	Frame::Frame( const Box& box ) : box(box), anchor(0,box.height)
	{
	}
	
	Frame::Frame( const Frame& frame ) : box(frame.box), anchor(frame.anchor)
	{
		// Import bounding boxes
		for( vector<Box *>::iterator it = frame.boundingBoxes.begin() ; it != frame.boundingBoxes.end() ; it++ )
			this->boundingBoxes.push_back( new Box( **it ) );
		
		// Import attack areas
		for( vector<Box *>::iterator it = frame.attackAreas.begin() ; it != frame.attackAreas.end() ; it++ )
			this->attackAreas.push_back( new Box( **it ) );
		
		// Import defence areas
		for( vector<Box *>::iterator it = frame.defenceAreas.begin() ; it != frame.defenceAreas.end() ; it++ )
			this->defenceAreas.push_back( new Box( **it ) );
	}
	
	Frame::~Frame()
	{
		// Delete bounding boxes
		for( vector<Box *>::iterator it = this->boundingBoxes.begin() ; it != this->boundingBoxes.end() ; it++ )
			delete *it;
		
		// Delete attack areas
		for( vector<Box *>::iterator it = this->attackAreas.begin() ; it != this->attackAreas.end() ; it++ )
			delete *it;
		
		// Delete defence areas
		for( vector<Box *>::iterator it = this->defenceAreas.begin() ; it != this->defenceAreas.end() ; it++ )
			delete *it;
	}
	
	Box& Frame::getBox()
	{
		return this->box;
	}
	
	Point& Frame::getAnchor()
	{
		return this->anchor;
	}
	
	void Frame::addBoundingBox( const Box& box )
	{
		this->boundingBoxes.push_back( new Box( box ) );
	}
	
	void Frame::addAttackArea( const Box& box )
	{
		this->attackAreas.push_back( new Box( box ) );
	}
	
	void Frame::addDefenceArea( const Box& box )
	{
		this->defenceAreas.push_back( new Box( box ) );
	}
}
