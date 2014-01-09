#include <graphics/Animation.h>

#ifdef DEBUG0
#include <iostream>
#endif

namespace graphics
{
	Animation::Animation() : speed(0)
	{
	}
	
	Animation::~Animation()
	{
	}
	
	const Sprite::Frame * Animation::getFrame()
	{
		return NULL;
	}
	
	int Animation::getSpeed()
	{
		return this->speed;
	}
	
	void Animation::setSpeed( int speed )
	{
		this->speed = speed;
	}
	
	void Animation::addFrame( int x, int y, int width, int height )
	{
		Sprite::Frame frame;
		frame.x = x;
		frame.y = y;
		frame.width = width;
		frame.height = height;
		this->frames.push_back( frame );
	}
}

