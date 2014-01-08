#include <data/parser/Parser.h>
#include <algorithm>
#include <iostream>
using namespace std;

namespace data
{
	namespace parser
	{
		Parser::Parser()
		{
		}

		Parser::~Parser()
		{
		}
		
		vector<string> Parser::readSymbols( const string& content, const string& separators, const string& spaces )
		{
			vector<string> parsedSymbols;
	
			// Parsing symbols
			unsigned int lastIndex = 0;
			bool parsingSpaces = false;
	
			for( unsigned int index = 0 ; index < content.length() ; index++ )
			{
				if( separators.find( content[index] ) != string::npos || spaces.find( content[index] ) != string::npos )
				{
					if( index > lastIndex )
					{
						parsingSpaces = false;
						parsedSymbols.push_back( content.substr( lastIndex, index - lastIndex ) );
					}
			
					if( spaces.find( content[index] ) == string::npos )
					{
						parsingSpaces = false;
						parsedSymbols.push_back( string(1, content[index]) );
					}
					else if( !parsingSpaces )
					{
						parsingSpaces = true;
						parsedSymbols.push_back( " " );
					}
			
					lastIndex = index + 1;
				}
				else if( index + 1 == content.length() )
				{
					parsingSpaces = false;
					parsedSymbols.push_back( content.substr( lastIndex, index - lastIndex + 1 ) );
				}
			}
			
			return parsedSymbols;
		}

		bool Parser::eop()
		{
			return this->pointer == this->symbols.end();
		}
		
		void Parser::initPointer()
		{
			this->pointer = this->symbols.begin();
		}
		
		
		void Parser::pushPointer()
		{
			this->pointers.push( this->pointer );
		}
		
		void Parser::popPointer( bool setActive )
		{
			if( ! this->pointers.empty() )
			{
				if( setActive )
					this->pointer = this->pointers.top();
				
				this->pointers.pop();
			}
		}

		int Parser::eatSpaces()
		{
			int spaces = 0;
	
			while( !this->eop() && this->findSymbol( " " ) )
			{
				spaces++;
				this->nextSymbol();
			}
	
			return spaces;
		}

		void Parser::nextSymbol( int count )
		{
			this->pointer += count;
		}

		void Parser::prevSymbol( int count )
		{
			this->pointer -= count;
		}

		bool Parser::findSymbol( const string& symbol )
		{
			return this->pointer != this->symbols.end() && *(this->pointer) == symbol;
		}

		string Parser::read()
		{
			return *(this->pointer);
		}
	}
}

