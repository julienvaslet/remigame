#include <graphics/Animation.h>

#ifdef DEBUG0
#include <iostream>
#endif

namespace graphics
{
	Animation::Animation() : speed(0), lastRender(0), lastFrameRendered(0)
	{
	}
	
	Animation::~Animation()
	{
	}
	
	const Sprite::Frame * Animation::getFrame( unsigned int time )
	{
		const Sprite::Frame * frame = NULL;
		
		if( !this->frames.empty() )
		{
			unsigned int step = (time - lastRender) / this->speed;
			
			if( step > 0 )
			{
				this->lastFrameRendered = (this->lastFrameRendered + step) % this->frames.size();
				this->lastRender = time;
			}
		
			frame = &(this->frames[this->lastFrameRendered]);
		}
		
		return frame;
	}
	
	unsigned int Animation::getSpeed()
	{
		return this->speed;
	}
	
	void Animation::setSpeed( unsigned int speed )
	{
		this->speed = speed;
	}
	
	void Animation::reset()
	{
		this->lastRender = 0;
		this->lastFrameRendered = 0;
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

