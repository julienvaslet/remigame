#ifndef __GRAPHICS_RENDERABLE_H
#define __GRAPHICS_RENDERABLE_H	1

namespace graphics
{
	class Renderable
	{
		public:
			virtual void render( unsigned int time ) = 0;
	};
}

#endif

