#ifndef __UI_LABEL_H
#define __UI_LABEL_H	1

#include <ui/Element.h>
#include <graphics/Font.h>
#include <graphics/Screen.h>

#include <string>

using namespace graphics;
using namespace std;

namespace ui
{
	class Label : public Element
	{
		protected:
			string fontName;
			string value;
			
			void autoResize();
		
		public:
			Label( const string& fontName, const string& value );
			virtual ~Label();
			
			const string& getValue();
			void setValue( const string& value, bool resize = true );
			const string& getFont();
			void setFont( const string& fontName, bool resize = true );
			virtual void render( unsigned int time );
	};
}

#endif

