#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include "../App/misc.h"

inline bool wordInString( String input, String word )
{
    for ( int i = 0; i < input.size() - word.size(); i++ )
    {
      for ( int j = 0; j < word.size(); j++ )
      {
        if ( input[i + j] != word[j] )
        {
          break;
        }
        if ( j == word.size() - 1 )
        {
          return true;
        }
      }
    }
    return false;
}

#endif
