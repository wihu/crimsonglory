#ifndef APPMISC_H_INCLUDED
#define APPMISC_H_INCLUDED

#include "../Game/stdafx.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

int inline random( int max )
{
    return rand() % max;
}

//float inline frand(int precis=4)
//{
//  int val=(int)pow(10,precis);
//  return (rand()%val)/(float)val;
//}

void inline InitRandomNumbers()
{
    srand( ( unsigned int ) time( 0 ) );
}

// cross-platform??
/*inline int GetCpuClocks()
{
    // Counter
    struct { int low, high; } counter;


    // Use RDTSC instruction to get clocks count
    __asm push EAX
    __asm push EDX
    __asm __emit 0fh __asm __emit 031h   // RDTSC
    __asm mov counter.low, EAX
    __asm mov counter.high, EDX
    __asm pop EDX
    __asm pop EAX


    // Return result
    return *(int *)(&counter.high);
}*/

inline unsigned int getPreciseTime()
{
#ifdef WIN32
    return GetTickCount();
#else
    struct timeval now;
    gettimeofday( &now, NULL );
    return ( now.tv_sec - start.tv_sec ) * 1000 + ( now.tv_usec - start.tv_usec ) / 1000;
#endif
}


void inline delay( int t )
{
#ifdef WIN32
    Sleep( t );
#else
    usleep( t );
#endif
}

inline WideString f2wchar( irr::f32 f )
{
    wchar_t t[32];
    swprintf( t, 32, L"%2.1f", f );
    return t;
}

inline WideString i2wchar( int i )
{
    wchar_t s[32];
    swprintf( s, 32, L"%d", i );
    return s;
}

inline WideString string2wide( const String str )
{
    int len = str.size() + 1;
    wchar_t* buf = new wchar_t[len];

    ::mbstowcs( buf, str.c_str(), len );
    WideString wstr = buf;
    delete[] buf;
    return wstr;
}
//=====================================================================================
inline String wide2string( const WideString str )
{
    int len = str.size() + 1;
    char* buf = new char[len];
    ::wcstombs( buf, str.c_str(), len );
    String wstr = buf;
    delete[] buf;
    return wstr;
}

// free memory after this
inline char* wchar2char( WideString a_string )
{
    char* ResultChar = new char[1024];
    wcstombs( ResultChar, a_string.c_str(), 1024 );
    return _strdup( ResultChar );
}

inline int wchar2i( WideString w )
{
    return _wtol( w.c_str() );
}

inline float clamp( float val, float minVal, float maxVal )
{
    return min( max( val, minVal ), maxVal );
}

inline WideString IPplusPort( WideString IP, int port )
{
    WideString wstr = IP;
    wstr += ":";
    wstr += port;
    return wstr;
}

// cross-platform??
inline void changeWorkingDir( String dir )
{
    WideString wstr = dir.c_str();
    _wchdir( wstr.c_str() );
}

// TODO: Make a string tools cpp
// this is in Irrlicht I think for string?
inline int StringEqualUntil( const char* s1, const char* s2, int n )
{
    int c1, c2;

    while ( 1 )
    {
      c1 = *s1++;
      c2 = *s2++;

      if ( !n-- )
      {
        return 0;
      }       // strings are equal until end point

      if ( c1 != c2 )
      {
        if ( c1 >= 'a' && c1 <= 'z' )
        {
          c1 -= ( 'a' - 'A' );
        }
        if ( c2 >= 'a' && c2 <= 'z' )
        {
          c2 -= ( 'a' - 'A' );
        }
        if ( c1 != c2 )
        {
          return -1;
        } // strings not equal
      }
      if ( !c1 )
      {
        return 0;
      }      // strings are equal
    }

    return -1;
}

inline int MakeBuildNumber( char* date )
{
    char* mon[12] =
    {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    char mond[12] =
    {
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    int m = 0; 
    int d = 0;
    int y = 0;
    int b;

    for ( m = 0; m < 11; m++ )
    {
      if ( StringEqualUntil( &date[0], mon[m], 3 ) == 0 )
      {
        break;
      }
      d += mond[m];
    }

    d += atoi( &date[4] ) - 1;

    y = atoi( &date[7] ) - 1900;

    b = d + ( int )( ( y - 1 ) * 365.25 );

    if ( ( ( y % 4 ) == 0 ) && m > 1 )
    {
      b += 1;
    }

    b -= 38308 - 41; // Oct 9 2005

    return b;
}


inline int fileExists( const char* fileName )
{
    FILE* infile;
    int ret = ( 0 );
    infile = fopen( fileName, "r" );
    if ( infile == NULL )
    {
      fclose( infile );// if you don't close, you get in trouble..
      ret = ( 1 );
    }
    else
    {
      fclose( infile ); // if you don't close, you get in trouble..
      ret = ( 0 );
    }
    fclose( infile );
    return ( ret );
}

inline bool fileExists2( const char* fname )
{
    FILE* fp = fopen( fname, "r" ); 
    if ( fp )
    {
      // exists 
      fclose( fp ); 
      return true;
    }
    else
    {
      return false;
    }
}

#endif
