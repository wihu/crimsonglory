#ifndef BOOLBLOCK_H_INCLUDED
#define BOOLBLOCK_H_INCLUDED

#include "../Engine/engine.h"


////////////////////////////////////////////
// CBoolpoint
////////////////////////////////////////////

struct CBoolpoint
{
    CBoolpoint();
    ~CBoolpoint();

	bool filled;
	f32 weight;
};


////////////////////////////////////////////
// CBoolblock 
////////////////////////////////////////////

class CBoolblock
{
  public:
    CBoolblock();
    virtual ~CBoolblock();

	void LoadField( const c8* filename );
	void createBlock(vector3df vPos, vector3df vScale);
	void fillAll(bool value);

	SMesh *mesh;

  private:
	void polygonize();

	CBoolpoint field[100][100];

	ISceneNode* blocknode;

	SMeshBuffer* meshbuffer;

	array<video::S3DVertex> vertices; 
	array<u16> indices;  
	aabbox3d<f32> box;

	vector3df vScale;
};

#endif