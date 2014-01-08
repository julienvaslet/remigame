#include <data/parser/NodeParser.h>
#include <algorithm>
#include <iostream>

using namespace std;

namespace data
{
	namespace parser
	{
		NodeParser::NodeParser( const string& content )
		{
			vector<string> parsedSymbols = this->readSymbols( content, "<>=\"/-!", " \t\n\r" );
			
			#ifdef DEBUG0
			cout << "[NodeParser] Loaded (" << parsedSymbols.size() << " symbols)." << endl;
			#endif
			
			// Packing symbols
			// TODO: Prevent from segmentation fault, add end of array checks
			for( vector<string>::iterator it = parsedSymbols.begin() ; it != parsedSymbols.end() ; it++ )
			{
				bool symbolAppended = false;
		
				// <! && <!-- && </
				if( *it == "<" )
				{
					if( *(++it) == "!" )
					{
						if( *(++it) == "-" )
						{
							if( *(++it) == "-" )
							{
								this->symbols.push_back( "<!--" );
								symbolAppended = true;
							}
							else
							 it -= 3;
						}
						else
						{
							it--;
							this->symbols.push_back( "<!" );
							symbolAppended = true;
						}
					}
					else
					{
						int spacesEated = 0;
				
						while( *it == " " )
						{
							spacesEated++;
							it++;
						}
				
						if( *it == "/" )
						{ 
							this->symbols.push_back( "</" );
							symbolAppended = true;
						}
						else
							it -= spacesEated + 1;
					}
				}
		
				// />
				else if( *it == "/" )
				{
					int spacesEated = 0;
					while( *(++it) == " " ) spacesEated++;
			
					if( *it == ">" )
					{
						this->symbols.push_back( "/>" );
						symbolAppended = true;
					}
					else
						it -= spacesEated + 1;
				}
		
				// -->
				else if( *it == "-" )
				{
					if( *(++it) == "-" )
					{
						if( *(++it) == ">" )
						{
							this->symbols.push_back( "-->" );
							symbolAppended = true;
						}
						else
							it -= 2;
					}
					else
						it--;
				}
		
				// composed-name
				else if( *it != " " )
				{
					if( ++it != parsedSymbols.end() && *it == "-" )
					{
						++it;
						if( *it != "-" && *it != " " )
						{
							it -= 2;
							string name = *it;
							name += *(++it);
							name += *(++it);
							this->symbols.push_back( name );
							symbolAppended = true;
						}
						else
							it -= 2;
					}
					else
						it--;
				}
		
				if( !symbolAppended )
					this->symbols.push_back( *it );
			}
	
			#ifdef DEBUG0
			cout << "[NodeParser] Packed, " << this->symbols.size() << " symbols." << endl;
			#endif
		}
		
		NodeParser::~NodeParser()
		{
		}
		
		void NodeParser::addSelfClosedTag( const string& tag )
		{
			if( find( this->selfClosedTags.begin(), this->selfClosedTags.end(), tag ) == this->selfClosedTags.end() )
			{
				this->selfClosedTags.push_back( tag );
		
				#ifdef DEBUG0
				cout << "[NodeParser] Added self-closed tag <" << tag << ">." << endl;
				#endif
			}
		}
		
		void NodeParser::removeSelfClosedTag( const string& tag )
		{
			vector<string>::iterator it = find( this->selfClosedTags.begin(), this->selfClosedTags.end(), tag );
	
			if( it != this->selfClosedTags.end() )
			{
				this->selfClosedTags.erase( it );
	
				#ifdef DEBUG0
				cout << "[NodeParser] Removed self-closed tag <" << tag << ">." << endl;
				#endif
			}
		}

		node::Node * NodeParser::parse()
		{
			this->initPointer();
			this->root = new node::Node( node::Node::Tag, "root" );
			this->currentNode = this->root;

			while( !this->eop() )
			{
				if( this->doctype() ) {}
				else if( this->comment() ) {}
				else if( this->tag() ) {}
				else this->text();
			}

			return this->root;
		}
		
		node::Node * NodeParser::findTagToClose( node::Node * node, const string& name )
		{
			if( ( node->getType() == node::Node::Tag && node->getName() == name ) || node->parent() == NULL )
				return node;
			else
				return this->findTagToClose( node->parent(), name );
		}
		
		bool NodeParser::doctype()
		{
			bool bReturn = false;
			this->pushPointer();
			
			this->eatSpaces();
	
			if( this->findSymbol( "<!" ) )
			{
				this->nextSymbol();
		
				if( this->findSymbol( "DOCTYPE" ) )
				{
					this->nextSymbol();
			
					while( !this->findSymbol( ">" ) )
						this->nextSymbol();
				
					this->nextSymbol();
					bReturn = true;
				}
			}
	
			this->popPointer( !bReturn );
	
			return bReturn;
		}
		
		bool NodeParser::comment()
		{
			bool bReturn = false;
			this->pushPointer();
			
			this->eatSpaces();
	
			if( this->findSymbol( "<!--" ) )
			{
				this->nextSymbol();
				string content;

				while( !this->findSymbol( "-->" ) )
				{
					content += this->read();
					this->nextSymbol();
				}
		
				this->nextSymbol();
				this->currentNode->append( new node::Node( node::Node::Comment, content ) );
				bReturn = true;
			}
			
			this->popPointer( !bReturn );
	
			return bReturn;
		}
		
		bool NodeParser::tag()
		{
			bool bReturn = false;
			this->pushPointer();
			
			node::Node * tag = NULL;
			this->eatSpaces();
	
			if( this->findSymbol( "<" ) )
			{
				this->nextSymbol();
				this->eatSpaces();
		
				string name = this->read();
				transform( name.begin(), name.end(), name.begin(), ::tolower );
		
				tag = new node::Node( node::Node::Tag, name );
				this->nextSymbol();
	
				this->currentNode->append( tag );
				this->currentNode = tag;
		
				while( this->attribute() );
	
				this->eatSpaces();
				
				if( this->findSymbol( "/>" ) || (this->findSymbol( ">" ) && find( this->selfClosedTags.begin(), this->selfClosedTags.end(), tag->getName() ) != this->selfClosedTags.end()) )
				{
					this->nextSymbol();
					this->currentNode = tag->parent();
					bReturn = true;
				}
				else if( this->findSymbol( ">" ) )
				{
					this->nextSymbol();
					bReturn = true;
				}
				else
				{
					this->currentNode = tag->parent();
					this->currentNode->remove( tag );
				}
			}
			else if( this->findSymbol( "</" ) )
			{
				this->nextSymbol();
				this->eatSpaces();
		
				string name = this->read();
				transform( name.begin(), name.end(), name.begin(), ::tolower );
		
				tag = this->findTagToClose( this->currentNode, name );

				this->nextSymbol();
				this->eatSpaces();
		
				if( this->findSymbol( ">" ) )
				{
					this->nextSymbol();
					bReturn = true;
					this->currentNode = tag->parent() == NULL ? tag : tag->parent();
				}
			}
	
			this->popPointer( !bReturn );
			
			return bReturn;
		}

		bool NodeParser::text()
		{
			node::Node * text = new node::Node( node::Node::Text, this->read() );
			this->nextSymbol();
			this->currentNode->append( text );
	
			return true;
		}
		
		bool NodeParser::attribute()
		{
			bool bReturn = false;
			this->pushPointer();
			
			this->eatSpaces();
			string name = this->read();	
			transform( name.begin(), name.end(), name.begin(), ::tolower );
	
			this->nextSymbol();
			this->eatSpaces();
	
			if( this->findSymbol( "=" ) )
			{
				this->nextSymbol();
				this->eatSpaces();
		
				if( this->findSymbol( "\"" ) )
				{
					this->nextSymbol();
			
					string value = "";
			
					while( !this->findSymbol( "\"" ) )
					{
						value += this->read();
						this->nextSymbol();
					}
			
					if( this->findSymbol( "\"" ) )
					{
						this->nextSymbol();
						this->currentNode->attr( name, value );
						bReturn = true;
					}
				}
			}
	
			this->popPointer( !bReturn );
			
			return bReturn;
		}
	}
}

