#include "../Engine/engine.h"

namespace irr
{
namespace scene
{

class CBoolBlockSceneNode : public scene::ISceneNode 
{ 
public: 

   //--- CONSTRUCTOR --- 
   CBoolBlockSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id = -1); 

   //--- DESTRUCTOR --- 
   ~CBoolBlockSceneNode(); 

   virtual void OnPreRender(); 

   virtual void render(); 

   virtual const core::aabbox3d<f32>& getBoundingBox() const; 

   virtual s32 getMaterialCount(); 

   virtual video::SMaterial& getMaterial(s32 i); 
    
   // acess height of vertice based on its position in matrix 
   void setHeight(int x, int z, float height);    

private:
   core::aabbox3d<f32> Box; 
   video::SMaterial Material; 
   video::S3DVertex Vertices[16]; //number of vertices 
   u16 indices[6*9];              //number of indices - 3*3=9 tiles each have 6 indices 
   int index [3+1] [3+1];         //index for storing vertex number - 4*4=16 vertices 
};

} // end namespace scene
} // end namespace irr


