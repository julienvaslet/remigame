#ifndef _GRAPHICS_ANIMATION_H
#define	_GRAPHICS_ANIMATION_H	1

#include <graphics/Frame.h>
#include <game/Player.h>
#include <string>
#include <vector>

using namespace std;
using namespace game;

namespace graphics
{
	class Animation
	{
		protected:
			unsigned int speed;
			int speedModulation;
			unsigned int lastRender;
			unsigned int lastFrameRendered;
			unsigned int times;
			Player * timesCallback;
			vector<Frame> frames;
	
		public:
			Animation();
			~Animation();
			
			Frame * getFrame( unsigned int time );
			unsigned int getSpeed();
			
			void setTimes( unsigned int times, Player * player );
			void setSpeed( unsigned int speed );
			void setSpeedModulation( int modulation );
			void addFrame( const Frame& frame );
			
			unsigned int getFrameCount() const;
			Frame& getFrameByIndex( unsigned int index );
			void removeFrameByIndex( unsigned int index );
			
			void reset();
	};
}

#endif

