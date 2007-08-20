#include "precache.h"
#include "../App/app.h"

#include "../Irrlicht/IrrlichtTask.h"
#include "../FreeSL/SoundTask.h"
#include "../IrrConsole/console.h"
#include "../Cal3D/CCal3dSceneNode.h"

////////////////////////////////////////////
// CPrecache 
////////////////////////////////////////////

CPrecache::CPrecache()
{
}

CPrecache::~CPrecache()
{
}

//Adobe Photoshop (.psd)
//JPEG File Interchange Format (.jpg)
//Portable Network Graphics (.png)
//Truevision Targa (.tga)
//Windows Bitmap (.bmp)
//Zsoft Paintbrush (.pcx)

//3D Studio meshes (.3ds)
//Alias Wavefront Maya (.obj)
//Cartography shop 4 (.csm) 
//COLLADA (.xml, .dae)
//DeleD (.dmf)
//FSRad oct (.oct)
//Microsoft DirectX (.x)
//Milkshape (.ms3d)
//My3DTools 3 (.my3D)
//Pulsar LMTools (.lmts)
//Quake 3 levels (.bsp)
//Quake 2 models (.md2)


void CPrecache::LoadFromFile( const char* filename )
{
    CONSOLE.add( " Precaching files..." );
    APPLOG.Write( "Precaching files." );
    std::ifstream in( filename );
    std::string line;
    String betterLine;

    if ( in.is_open() )
    {
      while ( !in.eof() )
      {
        getline( in, line );
        betterLine = line.c_str();
        betterLine.make_lower();

        if ( ( strstr( betterLine.c_str(), ".wav" ) ) || ( strstr( betterLine.c_str(), ".ogg" ) ) )
        {
          //CONSOLE.addx("Precaching sound file: %s", line.c_str() );
          IRR.drawAsyncMessage( line.c_str() );
          SOUND.precacheSound( line.c_str() );
        }

        if ( ( strstr( betterLine.c_str(), ".psd" ) ) || ( strstr( betterLine.c_str(), ".jpg" ) ) || ( strstr( betterLine.c_str(), ".png" ) ) || ( strstr( betterLine.c_str(), ".tga" ) ) || ( strstr( betterLine.c_str(), ".bmp" ) ) || ( strstr( betterLine.c_str(), ".pcx" ) ) )
        {
          //CONSOLE.addx("Precaching texture file: %s", line.c_str() );
          IRR.drawAsyncMessage( line.c_str() );
          IRR.video->getTexture( line.c_str() );
        }

        if ( ( strstr( betterLine.c_str(), ".3ds" ) ) || ( strstr( betterLine.c_str(), ".obj" ) ) || ( strstr( betterLine.c_str(), ".csm" ) ) || ( strstr( betterLine.c_str(), ".xml" ) ) || ( strstr( betterLine.c_str(), ".dae" ) ) || ( strstr( betterLine.c_str(), ".dmf" ) ) || ( strstr( betterLine.c_str(), ".oct" ) ) || ( ( strstr( betterLine.c_str(), ".x" ) ) && ( betterLine[betterLine.size() - 1] == 'x' ) ) || ( strstr( betterLine.c_str(), ".ms3d" ) ) || ( strstr( betterLine.c_str(), ".my3D" ) ) || ( strstr( betterLine.c_str(), ".lmts" ) ) || ( strstr( betterLine.c_str(), ".bsp" ) ) || ( strstr( betterLine.c_str(), ".md2" ) ) )
        {
          //CONSOLE.addx("Precaching mesh file: %s", line.c_str() );
          IRR.drawAsyncMessage( line.c_str() );
          IRR.smgr->getMesh( line.c_str() );
        }

        if ( ( strstr( betterLine.c_str(), ".cfg" ) ) )
        {
          //CONSOLE.addx("Precaching Cal3D config file: %s", line.c_str() );
          IRR.drawAsyncMessage( line.c_str() );
          CCal3DSceneNode::PrecacheModel( line.c_str() );
        }

        //delay(1000);
      }
      in.close();
    }

    CONSOLE.add( " ...Precaching files." );
}

void CPrecache::SaveToFile( const char* filename )
{
    APPLOG.Write( "Saving precached files." );

    std::ofstream out( filename );

    for ( int i = 0; i < APP.filesUsed.size(); i++ )
    {
      out << APP.filesUsed[i].c_str() << endl;
    }

    out.close();
}