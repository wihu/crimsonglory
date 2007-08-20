// Copyright (C) 2002-2005 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef FILECACHE_H_INCLUDED__
#define FILECACHE_H_INCLUDED__

#include "engine.h"

template <class T>
class CFileCache
{
  public:

    ~CFileCache()
    {
        for ( u32 i = 0; i < Filees.size(); ++i )
        {
          delete Filees[i].File;
        }
    }

    void addFile( const c8* filename, T* file )
    {
        FileEntry e;
        e.File = file;
        e.Name = filename;
        e.Name.make_lower();

        Filees.push_back( e );
    }

    void removeFile( T* file )
    {
        if ( file )
        {
          for ( int i = 0; i < ( int )Filees.size(); ++i )
          {
            if ( Filees[i].File == file )
            {
              delete Filees[i].File;
              Filees.erase( i );
              return;
            }
          }
        }
    }


    s32 getFileCount()
    {
        return Filees.size();
    }

    s32 getFileIndex( T* file )
    {
        for ( int i = 0; i < ( int )Filees.size(); ++i )
        {
          if ( Filees[i].File == file )
          {
            return i;
          }
        }

        return -1;
    }

    T* getFileByIndex( s32 index )
    {
        if ( number < 0 || number >= ( s32 )Filees.size() )
        {
          return 0;
        }

        return Filees[number].File;
    }

    const c8* getFileFilename( s32 number )
    {
        if ( number < 0 || number >= ( s32 )Filees.size() )
        {
          return 0;
        }

        return Filees[number].Name.c_str();
    }

    const c8* getFileFilename( T* file )
    {
        for ( s32 i = 0; i < ( s32 )Filees.size(); ++i )
        {
          if ( Filees[i].File == file )
          {
            return Filees[i].Name.c_str();
          }
        }

        return 0;
    }

    bool isFileLoaded( const c8* filename )
    {
        core::stringc name = filename;
        name.make_lower();
        return findFile( name.c_str() ) != 0;
    }

    T* findFile( const c8* lowerMadeFilename )
    {
        FileEntry e;
        e.Name = lowerMadeFilename;
        s32 id = Filees.binary_search( e );
        return ( id != -1 ) ? Filees[id].File : 0;
    }

  protected:

    struct FileEntry
    {
        core::stringc Name;
        T* File;

        bool operator <( const FileEntry& other ) const
        {
            return ( Name < other.Name );
        }
    };

    //! loaded filees
    core::array<FileEntry> Filees;
};


#endif

