#include <data/node/Node.h>
#include <algorithm>
#include <sstream>
#include <iostream>
using namespace std;

namespace data
{
	namespace node
	{
		const int Node::Comment = 0;
		const int Node::Text = 1;
		const int Node::Tag = 2;

		unsigned int Node::CurrentUId = 0;
		
		Node::Node( int type, string name ) : uid(Node::CurrentUId++), type(type), name(name), parentNode(NULL)
		{
			#ifdef DEBUG0
			cout << "[Node#" << this->uid << "] Created ";

			switch( this->type )
			{
				case Node::Comment:
					cout << "comment node <!--" << this->name << "-->.";
					break;
			
				case Node::Text:
					cout << "text node \"" << this->name << "\".";
					break;
			
				case Node::Tag:
					cout << "tag node <" << this->name << ">.";
					break;
			}
	
			cout << endl;
			#endif
		}

		Node::~Node()
		{
			for( vector<Node *>::iterator it = this->children.begin() ; it != this->children.end() ; it++ )
				delete *it;

			#ifdef DEBUG0
			cout << "[Node#" << this->uid << "] Deleted." << endl;
			#endif
		}

		Node * Node::clone()
		{
			Node * node = new Node( this->type, this->name );
	
			for( map<string, string>::iterator it = this->attributes.begin() ; it != this->attributes.end() ; it++ )
				node->attr( it->first, it->second );
	
			for( vector<Node *>::iterator it = this->children.begin() ; it != this->children.end() ; it++ )
				node->append( (*it)->clone() );
	
			return node;
		}

		void Node::attr( const string& name, const string& value )
		{
			this->attributes[ name ] = value;
	
			#ifdef DEBUG0
			cout << "[Node#" << this->uid << "] Setted attribute \"" << name << "\"=\"" << value << "\"." << endl;
			#endif
		}

		const bool Node::hasAttr( const string& name ) const
		{
			map<string,string>::const_iterator it = this->attributes.find( name );
			return it != this->attributes.end();
		}

		const string& Node::attr( const string& name )
		{
			if( this->hasAttr( name ) )
				return this->attributes[ name ];
			else
				throw "Attribute does not exist.";
		}
		
		const bool Node::isIntegerAttr( const string& name ) const
		{
			bool isInteger = true;
			
			if( this->hasAttr( name ) )
			{
				string txt = this->attributes[ name ];
				bool firstDigit = true;
				
				for( string::iterator it = txt.begin() ; it != txt.end() ; it++ )
				{
					if( firstDigit )
					{
						firstDigit = false;
						
						if( *it == '+' || *it == '-' )
							continue;
					}
					
					if( *it < '0' || *it > '9' )
					{
						isInteger = false;
						break;
					}
				}
			}
			else
				isInteger = false;
			
			return isInteger;
		}
		
		int Node::integerAttr( const string& name ) const
		{
			int value = 0;
			int decimal = 1;
			
			if( this->hasAttr( name ) )
			{
				string txt = this->attributes[ name ];
				
				for( string::reverse_iterator rit = txt.rbegin() ; rit != txt.rend() ; rit++ )
				{
					if( *rit >= '0' && *rit <= '9' )
					{
						value += (*rit - '0') * decimal;
					}
					else if( irt + 1 == txt.rend() && *rit == '-' || *rit == '+' )
					{
						if( *rit == '-' )
							value *= -1;
					}
					else
					{
						value = "0";
						throw "Attribute is not a valid integer.";
					}
					
					decimal *= 10;
				}
			}
			else
				throw "Attribute does not exist.";
				
			return value;
		}

		const string& Node::getName() const
		{
			return this->name;
		}

		int Node::getType() const
		{
			return this->type;
		}
		
		void Node::append( Node * node )
		{
			bool appended = false;
	
			if( node->type == Node::Text && this->children.size() > 0 )
			{
				vector<Node *>::reverse_iterator it = this->children.rbegin();
		
				if( (*it)->type == Node::Text )
				{
					(*it)->name += node->name;
					delete node;
			
					appended = true;
					
					#ifdef DEBUG0
					cout << "[Node#" << this->uid << "] Node#" << (*it)->uid << " updated." << endl;
					#endif
				}
			}
	
			if( !appended )
			{
				this->children.push_back( node );
				node->parentNode = this;
	
				#ifdef DEBUG0
				cout << "[Node#" << this->uid << "] Node#" << node->uid << " appended." << endl;
				#endif
			}
		}

		void Node::remove( Node * node )
		{
			for( vector<Node *>::iterator it = this->children.begin() ; it != this->children.end() ; it++ )
			{
				if( (*it)->uid == node->uid )
				{
					this->children.erase( it );
					break;
				}
			}
		}

		Node * Node::parent()
		{
			return this->parentNode;
		}

		bool Node::test( const string& pattern )
		{
			// tag #id .class [attr]
			string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-:0123456789";
	
			unsigned int index = 0;
			string tag = "";
			map<string, string> attributes;
	
			// Read the tag name
			while( index < pattern.length() )
			{
				if( characters.find( pattern[index] ) != string::npos )
				{
					tag += pattern[index];
					index++;
				}
				else
					break;
			}
	
			transform( tag.begin(), tag.end(), tag.begin(), ::tolower );
	
			// Attributes # or . or [...=...]
			while( index < pattern.length() )
			{
				string attribute = "";
				string value = "";
		
				if( pattern[index] == '#' )
				{
					index++;
			
					while( index < pattern.length() && characters.find( pattern[index] ) != string::npos )
						value += pattern[index++];
			
					attributes["id"] = value;
				}
				else if( pattern[index] == '.' )
				{
					index++;
			
					while( index < pattern.length() && characters.find( pattern[index] ) != string::npos )
						value += pattern[index++];
			
					attributes["class"] = value;
				}
				else if( pattern[index] == '[' )
				{
					index++;
			
					while( index < pattern.length() && characters.find( pattern[index] ) != string::npos )
						attribute += pattern[index++];
				
					transform( attribute.begin(), attribute.end(), attribute.begin(), ::tolower );
				
					if( pattern[index] == '=' )
					{
						index++;
				
						while( index < pattern.length() && pattern[index] != ']' )
							value += pattern[index++];
					
						index++;
					}
					else if( pattern[index] == ']' )
						index++;
					else
						break;
				
					attributes[attribute] = value;
				}
				else
					break;
			}
	
			return this->test( tag, attributes );
		}

		bool Node::test( const string& tag, const map<string, string>& attributes )
		{
			bool valid = false;
	
			if( this->type == Tag )
			{
				int rulesCount = 0;
				int rulesMatched = 0;
	
				if( tag.length() > 0 )
				{
					rulesCount++;
		
					if( tag == this->name )
						rulesMatched++;
				}
	
				for( map<string, string>::const_iterator it = attributes.begin() ; it != attributes.end() ; it++ )
				{
					rulesCount++;
		
					if( this->hasAttr( it->first ) )
					{
						rulesMatched++;
			
						if( it->second.length() > 0 )
						{
							rulesCount++;
				
							if( it->second == this->attr( it->first ) )
								rulesMatched++;
						}
					}
				}
	
				if( rulesCount == rulesMatched )
					valid = true;
			}
	
			return valid;
		}

		Node * Node::find( const string& pattern )
		{
			Node * matchNode = NULL;
	
			if( this->test( pattern ) )
				matchNode = this;
			else
			{
				for( vector<Node *>::iterator it = this->children.begin() ; it != this->children.end() && matchNode == NULL ; it++ )
					matchNode = (*it)->find( pattern );
			}
	
			return matchNode;
		}

		Node * Node::next( const string& pattern )
		{
			Node * node = NULL;
	
			if( this->parentNode != NULL )
			{
				int index = this->parentNode->indexOf( this );
		
				do
				{
					node = this->parentNode->childAt( ++index );
			
					/*if( node != NULL )
					{
						cout << "Node is not null" << endl;
				
						if( node->test( pattern ) )
							cout << "Valid pattern" << endl;
						else
							cout << "Wrong pattern" << endl;
					
						cout << node->text( true ) << endl;
					}
					else
						cout << "No bro!" << endl;*/
				}
				while( node != NULL && !node->test( pattern ) );
			}
	
			return node;
		}

		Node * Node::prev( const string& pattern )
		{
			Node * node = NULL;
	
			if( this->parentNode != NULL )
			{
				int index = this->parentNode->indexOf( this );
		
				do
				{
					node = this->parentNode->childAt( --index );
				}
				while( node != NULL && !node->test( pattern ) );
			}
	
			return node;
		}

		Node * Node::first( const string& pattern )
		{
			Node * node = NULL;
	
			for( vector<Node *>::iterator it = this->children.begin() ; it != this->children.end() ; it++ )
			{
				if( (*it)->test( pattern ) )
				{
					node = (*it);
					break;
				}
			}
	
			return node;
		}

		Node * Node::last( const string& pattern )
		{
			Node * node = NULL;
	
			for( vector<Node *>::reverse_iterator it = this->children.rbegin() ; it != this->children.rend() ; it++ )
			{
				if( (*it)->test( pattern ) )
				{
					node = (*it);
					break;
				}
			}
	
			return node;
		}

		Node * Node::childAt( int index )
		{
			if( index < 0 || static_cast<unsigned int>(index) >= this->children.size() )
				return NULL;
			else
				return this->children[index];
		}

		int Node::indexOf( const Node * node )
		{
			int index = 0;
	
			for( vector<Node *>::iterator it = this->children.begin() ; it != this->children.end() ; it++ )
			{
				if( (*it)->uid == node->uid )
					break;
		
				index++;
			}
	
			return static_cast<unsigned int>(index) == this->children.size() ? -1 : index;
		}


		string Node::text( bool outer, int depth ) const
		{
			ostringstream text;
			string pad( depth, ' ' );
	
			switch( this->type )
			{
				case Node::Comment:
				{
					if( outer )
						text << pad << "<!--";
				
					text << this->name;
			
					if( outer )
						text << "-->" << endl;

					break;
				}
			
				case Node::Text:
				{
					if( outer )
						text << pad;
				
					text << this->name;
			
					if( outer )
						text << endl;
				
					break;
				}
			
				case Node::Tag:
				{
					if( outer )
					{
						text << pad << "<" << this->name;
			
						map<string,string>::const_iterator attrIt;
						for( attrIt = this->attributes.begin() ; attrIt != this->attributes.end() ; attrIt++ )
							text << " " << attrIt->first << "=\"" << attrIt->second << "\"";
			
						if( this->children.size() == 0 )
							text << " />" << endl;
						else
							text << ">" << endl;
					}
			
						vector<Node *>::const_iterator childIt;
						for( childIt = this->children.begin() ; childIt != this->children.end() ; childIt++ )
							text << (*childIt)->text( true, depth + 1 );
			
					if( outer && this->children.size() > 0 )	
						text << pad << "</" << this->name << ">" << endl;
			
					break;
				}
			}
	
			return text.str();
		}

		void Node::show() const
		{
			cout << this->text( true );
		}
	}
}

