#include "CBoolBlockSceneNode.h"

#include "../Engine/misc.h"


namespace irr
{
namespace scene
{

CBoolBlockSceneNode::CBoolBlockSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id) 
   : scene::ISceneNode(parent, mgr, id) 
{ 
   Material.Wireframe = true; 
   Material.Lighting = false; 

   //terain matrix = 3*3 tiles big, one tile = 10*10 units    

   // defining vertices of terain matrix 
   int i = 0; 
   for(int x=0; x<(3+1); x++) 
   { 
      for(int z=0; z<(3+1); z++) 
      { 
          Vertices[i] = video::S3DVertex( x*10, 0, z*10, 
                                          0,1,0, 
                                          video::SColor(255,100,200,100), 
                                          0, 0); 
          index[x][z] = i; 
          i++;  
      } 
   } 
    
   //defining indices of tiles 
   i = 0; 
   for(int x=0; x<3; x++) 
   { 
      for(int z=0; z<3; z++) 
      { 
          // firsth polygoon of tille 
          indices[i] = index[x][z]; 
          i++; 
          indices[i] = index[x][z+1]; 
          i++; 
          indices[i] = index[x+1][z+1]; 
          i++; 
          // second polygoon of tille 
          indices[i] = index[x][z]; 
          i++; 
          indices[i] = index[x+1][z+1]; 
          i++; 
          indices[i] = index[x+1][z]; 
          i++; 
      } 
   }  

   Box.reset(Vertices[0].Pos); 
   for (s32 i=1; i<16; ++i) 
      Box.addInternalPoint(Vertices[i].Pos); 
} 

//--- DESTRUCTOR ---    
CBoolBlockSceneNode::~CBoolBlockSceneNode() {} 

void CBoolBlockSceneNode::OnPreRender() 
{ 
   if (IsVisible) 
   { 
      SceneManager->registerNodeForRendering(this); 
      ISceneNode::OnPreRender(); 
   } 
} 

void CBoolBlockSceneNode::render() 
{ 
   video::IVideoDriver* driver = SceneManager->getVideoDriver(); 

   driver->setMaterial(Material); 
   driver->setTransform(video::ETS_WORLD, AbsoluteTransformation); 
   driver->drawIndexedTriangleList(&Vertices[0], 16, &indices[0], 18); //num of vertices, num of polygoons 
} 

const core::aabbox3d<f32>& CBoolBlockSceneNode::getBoundingBox() const 
{ 
   return Box; 
} 

s32 CBoolBlockSceneNode::getMaterialCount() 
{ 
   return 1; 
} 

video::SMaterial& CBoolBlockSceneNode::getMaterial(s32 i) 
{ 
   return Material; 
} 

void CBoolBlockSceneNode::setHeight(int x, int z, float height) 
{ 
    core::vector3df position = Vertices[ index[x][z] ].Pos; 
    position.Y = height; 
    Vertices[ index[x][z] ].Pos = position; 
}


} // end namespace scene
} // end namespace irr

