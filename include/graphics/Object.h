#ifndef _GRAPHICS_OBJECT_H
#define	_GRAPHICS_OBJECT_H	1

#include <SDL2/SDL.h>
#include <string>
#include <graphics/Sprite.h>
#include <graphics/Animation.h>

#include <map>

using namespace std;

namespace graphics
{
	class Object
	{
		protected:
			int x;
			int y;
			int width;
			int height;
			int speedModulation;
			int zoom;
			
			Sprite * sprite;
			Animation * currentAnimation;
			map<string, Animation *> animations;
			
			bool anchorPointRenderingState;
			bool boundingBoxesRenderingState;
			bool attackAreasRenderingState;
			bool defenceAreasRenderingState;
	
		public:
			Object();
			Object( const char * filename );
			Object( const string& filename );
			~Object();
		
			void move( int x, int y );
			void resize( int width, int height );
			
			int getX();
			int getY();
			int getWidth();
			int getHeight();
			
			bool isLoaded();
			bool load( const char * filename );
			void render( unsigned int time );
			
			bool setAnimation( const string& name );
			void setSpeedModulation( int modulation );
			int getSpeedModulation();
			int getAnimationSpeed();
			
			int getZoom();
			void setZoom( int zoom );
			
			void setAnchorPointRenderingState( bool state );
			void setBoundingBoxesRenderingState( bool state );
			void setAttackAreasRenderingState( bool state );
			void setDefenceAreasRenderingState( bool state );
	};
}

#endif

