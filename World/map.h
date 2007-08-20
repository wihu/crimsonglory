#ifndef _MAP_H
#define _MAP_H

#include "../Engine/engine.h"
#include "entity.h"

#include "respawn.h"
#include "editor.h"
#include "../Irrlicht/CAnimSprite.h"

class CReflectedWater;
class ATMOsphere;

////////////////////////////////////////////
// CMap_Cloud 
////////////////////////////////////////////

enum CloudType { CLOUD_1, CLOUD_2, CLOUD_3, CLOUD_4, CLOUD_5, CLOUD_6, CLOUD_7, CLOUD_8, CLOUD_9, };

class CMap_Cloud
{
  public:
    CMap_Cloud( aabbox3df box, int type );
    ~CMap_Cloud();

    aabbox3df* getBox()
    {
        return &cloudbox;
    }
    int getType()
    {
        return type;
    }

    void Scale( vector3df vScale );

  private:
    aabbox3df cloudbox;
    int type;
    array<IBillboardSceneNode*> nodes;
    vector3df vCenter, vSize;
};

////////////////////////////////////////////
// CMap_Zone 
////////////////////////////////////////////

enum ZoneType { ZONE_WATER, ZONE_NULL };

class CMap_Zone
{
  public:
    CMap_Zone( aabbox3df box, int type );
    ~CMap_Zone();

    aabbox3df* getBox()
    {
        return &zonebox;
    }
    int getType()
    {
        return type;
    }
    vector3df* getSize()
    {
        return &vSize;
    }

    f32 getLiquidWeightFactor()
    {
        return fLWF;
    }

    void Scale( vector3df vScale );

    CReflectedWater* water;
    ISceneNode* node;

  private:
    aabbox3df zonebox;
    int type;
    //scene::ISceneNode* node;
    vector3df vSize;

    f32 fLWF;
};

////////////////////////////////////////////
// CMap_Plane 
////////////////////////////////////////////
class CMap_Plane
{
  public:
    CMap_Plane( vector3df p1, vector3df p2, vector3df p3, vector3df p4, f32 newSize );

    triangle3df GetTriangle( s32 i )
    {
        return triangle[i];
    }
    f32 GetSize()
    {
        return size;
    }

    // 1---2
    // 3---4
    void setPlane( vector3df p1, vector3df p2, vector3df p3, vector3df p4 );

    bool getIntersectionWithLine( vector3df p1, vector3df p2, vector3df& outIntersection );
    bool getIntersectionWithLimitedLine( vector3df p1, vector3df p2, vector3df& outIntersection );

  private:
    triangle3df triangle[2];
    f32 size;
};

////////////////////////////////////////////
// CMap 
////////////////////////////////////////////
class CMap : public CEntity
{
  public:
    CMap();
    virtual ~CMap();
    // AUTO_SIZE;

    //CMap_Plane* GetPlane(s32 i)               { if (PlaneExists(i)) return Planes[i]; else return NULL; }
    //s32 GetPlanesNum()                        { return Planes.size(); }
    //bool PlaneExists(s32 i)                   { if ((i<0)||(i>(s32)Planes.size()-1)) return false; else return true;  }

    CMap_Zone* GetZone( s32 i )
    {
        if ( ZoneExists( i ) )
        {
          return Zones[i];
        }
        else
        {
          return NULL;
        }
    }
    s32 GetZonesNum()
    {
        return Zones.size();
    }
    bool ZoneExists( s32 i )
    {
        if ( ( i < 0 ) || ( i > ( s32 )Zones.size() - 1 ) )
        {
          return false;
        }
        else
        {
          return true;
        }
    }


    void AddPlane( CMap_Plane* p )
    {
        Planes.push_back( p );
    }
    // method for adding 2d map plane based on two points
    void Add2DPlane( vector3df p1, vector3df p2 );
    void NewSky( const c8* filename, int hour, int minute );
	void NewFog( SColor color, bool linearFog, f32 start, f32 end, f32 density, bool pixelFog, bool rangeFog );
	void SetFogColor( SColor color ); // works if dynamicFog, colors = 255
    void AddZone( vector3df vPos, vector3df vSize, int zonetype );
    void AddZone( aabbox3df box, int zonetype );
    void addMeshMap( const c8* filename, vector3df vPos, vector3df vScale );
    void addSprite( CAnimSpriteSceneNode* Sprite );
    void addCloud( CMap_Cloud* cloud );
    void setSunLight( ISceneNode* l )
    {
        sunLight = l;
    }
	void NewATMOSphere();

    ISceneNode* getSunLight()
    {
        return sunLight;
    }

    virtual void Think();
    virtual void Render();

    void Load( const c8* filename );
    void Save( const c8* filename );

    CRespawn* GetRespawn()
    {
        return respawn;
    }
    CEditor* GetEditor();

    struct optCell
    {
        array<CMap_Plane*> Planes;
    };
    optCell getOptCell( int c, int r )
    {
        if ( PointInOptGrid( c, r ) )
        {
          return optGrid[r][c];
        } return optGrid[0][0];
    }
    s32 OptGridRow( f32 y )
    {
        return ( floor( y / optCellHeight ) );
    }
    s32 OptGridColumn( f32 x )
    {
        return ( floor( x / optCellWidth ) );
    }
    bool PointInOptGrid( s32 x, s32 y )
    {
        if ( ( x > -1 ) && ( x < optColumns - 1 ) && ( y > -1 ) && ( y < optRows - 1 ) )
        {
          return true;
        } return false;
    }

    f32 worldWidth, worldHeight, worldDepth, planeSize;

  private:
    friend class CEditor;
    vector3df getOptGridRealPos( int r, int c );
    void CheckPlanesInCell( int i, int j, CMap_Plane* plane, vector3df vP1, vector3df vP2, int im );
    void GenerateOptimizationGrid();

    //  void createCollisionFromBlock( CBoolblock *block, vector3df vPos = vector3df(0.0f, 0.0f, 0.0f), vector3df vScale = vector3df(1.0f, 1.0f, 1.0f) );

    array<CMap_Plane*> Planes;
    array<CMap_Zone*> Zones;
    array<CAnimSpriteSceneNode*> Sprites;
    array<CMap_Cloud*> Clouds;

    ISceneNode* sky;
    String skyFilename;
    ISceneNode* sunLight;
	ATMOsphere* atmo;

    optCell** optGrid;
    int optRows, optColumns;
    f32 optCellWidth, optCellHeight;

    CRespawn* respawn;
    CEditor* editor;

	bool linearFog, pixelFog, rangeFog, dynamicFog;
	f32 startFog, endFog, densityFog;
};


// Map File Structure
#define MAPFILE_VERSION 1

struct MapHeader
{
    s32 version;
    s32 planesNum;
    c8 skyfile[255];
    f32 worldWidth, worldHeight, worldDepth;

    s32 dummy[20];
};

struct MapPlane
{
    triangle3df triangle[2];
    f32 size;

    s32 dummy[20];
};

#endif