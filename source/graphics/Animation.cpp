#include <graphics/Animation.h>

#ifdef DEBUG0
#include <iostream>
#endif

namespace graphics
{
	Animation::Animation() : speed(0), speedModulation(0), lastRender(0), lastFrameRendered(0)
	{
	}
	
	Animation::~Animation()
	{
	}
	
	Frame * Animation::getFrame( unsigned int time )
	{
		Frame * frame = NULL;
		
		if( !this->frames.empty() )
		{
			if( this->speed != 0 )
			{
				unsigned int step = (time - lastRender) / (this->speed + this->speedModulation);
			
				if( step > 0 )
				{
					this->lastFrameRendered = (this->lastFrameRendered + step) % this->frames.size();
					this->lastRender = time;
				}
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
	
	void Animation::setSpeedModulation( int modulation )
	{
		this->speedModulation = modulation;
	}
	
	void Animation::reset()
	{
		this->lastRender = 0;
		this->lastFrameRendered = 0;
		this->speedModulation = 0;
	}
	
	void Animation::addFrame( const Frame& frame )
	{
		this->frames.push_back( frame );
	}
	
	unsigned int Animation::getFrameCount() const
	{
		return this->frames.size();
	}
	
	Frame& Animation::getFrameByIndex( unsigned int index )
	{
		return this->frames[index];
	}
}

