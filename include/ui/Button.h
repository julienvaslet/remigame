#ifndef __UI_BUTTON_H
#define __UI_BUTTON_H	1

#include <ui/Element.h>
#include <graphics/Point.h>
#include <graphics/Box.h>
#include <graphics/Font.h>
#include <graphics/Screen.h>

#include <string>

using namespace graphics;
using namespace std;

namespace ui
{
	class Button : public Element
	{
		protected:
			string fontName;
			string value;
			Box box;
			
			bool highlighted;
			bool pushed;
			
			void autoResize();
		
		public:
			Button( const string& fontName, const string& value );
			virtual ~Button();
			
			Box& getBox();
			const string& getValue();
			void setValue( const string& value, bool resize = true );
			const string& getFont();
			void setFont( const string& fontName, bool resize = true );
			virtual void render( unsigned int time );
			
			static bool eventMouseDown( Element * );
			static bool eventMouseUp( Element * );
			static bool eventMouseEnter( Element * );
			static bool eventMouseLeave( Element * );
	};
}

#endif

