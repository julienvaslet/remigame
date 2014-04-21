#ifndef _GRAPHICS_OBJECT_H
#define	_GRAPHICS_OBJECT_H	1

#include <SDL2/SDL.h>
#include <string>
#include <graphics/Point.h>
#include <graphics/Sprite.h>
#include <graphics/Animation.h>
#include <graphics/Renderable.h>

#include <map>

using namespace std;

namespace graphics
{
	class Object : public Renderable
	{
		protected:
			Point origin;
			int speedModulation;
			int zoom;
			bool invertedHorizontalAxis;
			bool invertedVerticalAxis;
			
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
			virtual ~Object();
			
			Point& getOrigin();
			
			bool isLoaded();
			bool load( const char * filename );
			virtual void render( unsigned int time );
			
			bool setAnimation( const string& name, unsigned int times = 0, Player * callback = NULL );
			void setSpeedModulation( int modulation );
			int getSpeedModulation();
			int getAnimationSpeed();
			
			void invertHorizontalAxis( bool status );
			bool isHorizontalAxisInverted();
			void invertVerticalAxis( bool status );
			bool isVerticalAxisInverted();
			
			int getZoom();
			void setZoom( int zoom );
			
			void setAnchorPointRenderingState( bool state );
			void setBoundingBoxesRenderingState( bool state );
			void setAttackAreasRenderingState( bool state );
			void setDefenceAreasRenderingState( bool state );
	};
}

#endif

