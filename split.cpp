#include <cctype>
#include <algorithm>
#include "split.h"

using std::isspace;
using std::string;
using std::getline;
using std::find_if;

bool not_space(char c)
{
    return !isspace(c);
}

bool space(char c)
{
    return isspace(c);
}

template<class Out>
void split(const string& str, Out os)
{
    typedef string::const_iterator iter;
    
    iter i = str.begin();
    
    while (i != str.end())
    {
		// ignore leading blanks
		i = find_if(i, str.end(), not_space);
	
		// find the end of the next word
		iter j = find_if(i, str.end(), space);
	
		// copy characters in range i,j to the output
		if (i != str.end())
		{
			*os++ = string(i, j);
		}
	
		// update the first iterator
		i = j;
    }
}
