#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

template <typename T>
class Singleton
{
    static T* ms_singleton;
  protected:

    Singleton()
    {
        assert( !ms_singleton );
        ms_singleton = static_cast<T*>( this );
    }

    ~Singleton()
    {
        assert( ms_singleton );
        ms_singleton = 0;
    }

  public:
    static inline void Create()
    {
        if ( ms_singleton )
        {
          return;
        }
        new T();
    }
    static inline T& GetSingleton()
    {
        assert( ms_singleton );
        return *ms_singleton;
    }
    static inline T* GetSingletonPtr()
    {
        assert( ms_singleton );
        return ms_singleton;
    }
    static inline void SetSingletonPtr( T* n )
    {
        if ( !ms_singleton )
        {
          ms_singleton = n;
        }
    }
    static inline bool IsValid()
    {
        return ( ms_singleton );
    }
    static inline void Destroy()
    {
        if ( ms_singleton )
        {
          delete ms_singleton;
        }
        ms_singleton = 0;
    }
};

template <typename T>
T* Singleton <T> ::ms_singleton = 0;

#endif
