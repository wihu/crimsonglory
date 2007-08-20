// $Id: CCal3dSceneNode.h,v 1.4 2004/05/06 16:41:42 tsuter Exp $
#ifndef CCAL3DSCENENODE_H_INCLUDED
#define CCAL3DSCENENODE_H_INCLUDED

// link with the cal3d library
//#pragma comment(lib,"cal3d.lib")


#include "../Engine/engine.h"

#include "cal3d/cal3d.h"
#include "../Engine/fileCache.h"

typedef irr::core::vector3df Vector3; 

class CCal3DModelCache;

class CCal3DSceneNode : public irr::scene::ISceneNode
{
  protected: 
    vector3df targetTranslation, targetRotation;

    //! configuration file for the character 
    std::string configFile; 

    //! the path where the media (textures etc.) are stored 
    std::string mediaPath; 

    //! core model for cal3d, 
    //! this should raelly be in a seperate coremodel server class 
    CalCoreModel* m_calCoreModel; 

    //! instance model for cal3d 
    CalModel* m_calModel; 

    //! indicate if animation is paused 
    bool m_bPaused; 

    //! model scale 
    float m_scale; 

    //! id of the current animation 
    int m_currentAnimationId; 

    //! remaining time for the current animation 
    float m_leftAnimationTime; 

    //! time the animation blending will take 
    float m_blendTime; 

    //! level of detail 
    float m_lodLevel; 

    //! bounding box dummy 
    irr::core::aabbox3df Box; 

    //! material 
    irr::video::SMaterial material; 

    //! last frame time 
    irr::u32 lastTick; 

    //! true iff initialized 
    bool bInitialized; 

    int m_animationId[255];

  public : 

    //! constructor 
    //! @param cf, configuration file 
    //! @param parent, parent scene node 
    //! @param mgr, scene manager 
    //! @param id, scenenode id 
    CCal3DSceneNode( const char* cf, irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id = -1 ); 

    //! destructur 
    virtual ~CCal3DSceneNode(); 

    virtual void UpdateAnimation( float timeStepSec );

    static CFileCache<CCal3DModelCache> coreModels;

    static CCal3DModelCache* PrecacheModel( const char* configFilename );

    //////////////////////////////////////////////// 
    // irrlicht stuff 
    //////////////////////////////////////////////// 

    void OnPreRender(); 

    void render(); 

    void OnPostRender( irr::u32 timeMs ); 

    const irr::core::aabbox3df& getBoundingBox() const; 

    irr::video::SMaterial& getMaterial( irr::s32 i )
    {
        return material;
    } 

    irr::s32 getMaterialCount()
    {
        return 1;
    } 

    virtual void setRotation( const core::vector3df& rotation )
    {
        //RelativeRotation = rotation;
        targetRotation = rotation;
    }

    virtual void setPosition( const core::vector3df& newpos )
    {
        //RelativeTranslation = newpos;
        targetTranslation = newpos;
    }

    //////////////////////////////////////////////// 
    // call 3d stuff 
    //////////////////////////////////////////////// 

    // load all textures 
    bool init(); 

    //////////////////////////////////////////////// 
    // more from Cally demo
    //////////////////////////////////////////////// 
    void setLodLevel( float lodLevel );
    virtual void executeAction( int animNum, float delayOut = 1.0f, float weightTarget = 1.0f, bool clearPrevious = true );
    float getLodLevel();

    CalModel* getCurrentModel()
    {
        return m_calModel;
    }
    CalCoreModel* getCoreModel()
    {
        return m_calCoreModel;
    }

    int getCurrentAnimID()
    {
        return m_currentAnimationId;
    }

    virtual void clearAnim( int animNum, float delayOut = 1.0f );
};



class CCal3DModelCache
{
  public:
    virtual ~CCal3DModelCache(); 

    CalCoreModel* m_calCoreModel;
    float m_scale; 
    int m_animationId[255];

    //! load and parse the configuration file 
    //! @param cf, configuration file name 
    //! @return true on success, false otherwise 
    bool parseModelConfiguration( const std::string& cf, const std::string& mediaPath );
};

#endif