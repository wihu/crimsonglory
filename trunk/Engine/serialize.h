#ifndef SERIALIZE_H_INCLUDED
#define SERIALIZE_H_INCLUDED

class ISerializer
{
  public:
    virtual void IO( unsigned char& value ) = 0;
    virtual void IO( unsigned long& value ) = 0;
    virtual void IO( long& value ) = 0;
    virtual void IO( std::string& str ) = 0;
};

class CSerialSaver : virtual public ISerializer
{
  protected:
    unsigned char* buffer;
    bool bHasOverflowed;
    unsigned long length;
    unsigned long bytesUsed;
  public:
    CSerialSaver( unsigned char* buf, unsigned long size )
    {
        buffer = buf; length = size; bytesUsed = 0; bHasOverflowed = false;
    }

    void IO( unsigned char& value )
    {
        if ( bHasOverflowed )
        {
          return;
        } //stop writing when overflowed
        if ( bytesUsed + 1 > length )
        {
          bHasOverflowed = true; return;
        }
        *buffer = value;
        ++buffer; ++bytesUsed;
    }
    void IO( unsigned long& value )
    {
        if ( bHasOverflowed )
        {
          return;
        } //stop writing when overflowed
        if ( bytesUsed + 4 > length )
        {
          bHasOverflowed = true; return;
        }
        //      SDLNet_Write32(value,buffer);
        buffer += 4; bytesUsed += 4;
    }
    void IO( long& value )
    {
        if ( bHasOverflowed )
        {
          return;
        } //stop writing when overflowed
        if ( bytesUsed + 4 > length )
        {
          bHasOverflowed = true; return;
        }
        //      SDLNet_Write32(value,buffer);
        buffer += 4; bytesUsed += 4;
    }
    void IO( std::string& str )
    {
        unsigned long l = str.length();
        IO( l );
        if ( bHasOverflowed )
        {
          return;
        }
        if ( bytesUsed + l > length )
        {
          bHasOverflowed = true; return;
        }
        memcpy( buffer, str.c_str(), l );
        buffer += l; bytesUsed += l;
    }

    bool hasOverflowed()
    {
        return bHasOverflowed;
    }
    long getFlow()
    {
        return length - bytesUsed;
    } //should be equal to 0 when we're done
};

class CSerialLoader : virtual public ISerializer
{
  protected:
    unsigned char* buffer;
    bool bHasOverflowed;
    unsigned long length;
    unsigned long bytesUsed;
  public:

    CSerialLoader( unsigned char* buf, unsigned long size )
    {
        buffer = buf; length = size; bytesUsed = 0; bHasOverflowed = false;
    }

    void IO( unsigned char& value )
    {
        if ( bHasOverflowed )
        {
          return;
        } //stop writing when overflowed
        if ( bytesUsed + 1 > length )
        {
          bHasOverflowed = true; return;
        }
        value = *buffer;
        ++buffer; ++bytesUsed;
    }
    void IO( unsigned long& value )
    {
        if ( bHasOverflowed )
        {
          return;
        } //stop writing when overflowed
        if ( bytesUsed + 4 > length )
        {
          bHasOverflowed = true; return;
        }
        //      value=SDLNet_Read32(buffer);
        buffer += 4; bytesUsed += 4;
    }
    void IO( long& value )
    {
        if ( bHasOverflowed )
        {
          return;
        } //stop writing when overflowed
        if ( bytesUsed + 4 > length )
        {
          bHasOverflowed = true; return;
        }
        //      value=SDLNet_Read32(buffer);
        buffer += 4; bytesUsed += 4;
    }
    void IO( std::string& str )
    {
        unsigned long l;
        IO( l );
        if ( bHasOverflowed )
        {
          return;
        }
        if ( bytesUsed + l > length )
        {
          bHasOverflowed = true; return;
        }
        char* szBuf = new char[l + 1];
        szBuf[l] = 0;
        memcpy( szBuf, buffer, l );
        str = szBuf;
        delete[] szBuf;
        buffer += l; bytesUsed += l;
    }

    bool hasOverflowed()
    {
        return bHasOverflowed;
    }
    long getFlow()
    {
        return length - bytesUsed;
    } //should be equal to 0 when we're done
};

class CSerialSizer : virtual public ISerializer
{
  protected:
    unsigned long length;
  public:
    CSerialSizer()
    {
        length = 0;
    }

    void IO( unsigned char& value )
    {
        ++length;
    }

    void IO( unsigned long& value )
    {
        length += 4;
    }

    void IO( long& value )
    {
        length += 4;
    }

    void IO( std::string& str )
    {
        IO( length ); length += str.length();
    }

    unsigned long getLength()
    {
        return length;
    }
};

#endif
