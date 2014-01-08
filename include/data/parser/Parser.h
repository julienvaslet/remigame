#ifndef __DATA_PARSER_PARSER_H
#define __DATA_PARSER_PARSER_H	1

#include <string>
#include <vector>
#include <stack>
using namespace std;

namespace data
{
	namespace parser
	{
		class Parser
		{
			protected:
				vector<string> symbols;
				vector<string>::iterator pointer;
				stack<vector<string>::iterator> pointers;
	
				bool eop();
				bool findSymbol( const string& symbol );
				void nextSymbol( int count = 1 );
				void prevSymbol( int count = 1 );
				
				void initPointer();
				void pushPointer();
				void popPointer( bool setActive = false );
				
				string read();
				int eatSpaces();
				
				vector<string> readSymbols( const string& content, const string& separators, const string& spaces );
	
			public:
				Parser();
				~Parser();
		};
	}
}

#endif

