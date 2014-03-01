#ifndef __UI_PUSHBUTTON_H
#define __UI_PUSHBUTTON_H	1

#include <graphics/Font.h>
#include <graphics/Screen.h>
#include <ui/Button.h>

#include <string>

using namespace graphics;
using namespace std;

namespace ui
{
	class PushButton : public Button
	{
		protected:
			bool pushState;
			
		public:
			PushButton( const string& fontName, const string& value, bool pushed = false );
			virtual ~PushButton();
			
			void setPushState( bool state );
			bool getPushState() const;
			
			static bool eventMouseUp( Element * );
			static bool eventMouseLeave( Element * );
	};
}

#endif

