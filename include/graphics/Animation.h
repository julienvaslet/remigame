#ifndef _GRAPHICS_ANIMATION_H
#define	_GRAPHICS_ANIMATION_H	1

#include <graphics/Sprite.h>
#include <string>
#include <vector>

using namespace std;

namespace graphics
{
	class Animation
	{
		protected:
			unsigned int speed;
			int speedModulation;
			unsigned int lastRender;
			unsigned int lastFrameRendered;
			vector<Sprite::Frame> frames;
	
		public:
			Animation();
			~Animation();
			
			const Sprite::Frame * getFrame( unsigned int time );
			unsigned int getSpeed();
			
			void setSpeed( unsigned int speed );
			void setSpeedModulation( int modulation );
			void addFrame( int x, int y, int width, int height );
			
			void reset();
	};
}

#endif

