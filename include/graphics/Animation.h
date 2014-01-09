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
			int speed;
			vector<Sprite::Frame> frames;
	
		public:
			Animation();
			~Animation();
			
			const Sprite::Frame * getFrame();
			int getSpeed();
			
			void setSpeed( int speed );
			void addFrame( int x, int y, int width, int height );
	};
}

#endif

