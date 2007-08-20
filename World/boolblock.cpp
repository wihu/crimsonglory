#include "boolblock.h"

//IRR.
#include "../Irrlicht/IrrlichtTask.h"
//CONSOLE.
#include "../IrrConsole/console.h"

////////////////////////////////////////////
// CBoolpoint
////////////////////////////////////////////

CBoolpoint::CBoolpoint()
{
	filled = false;
	weight = 0;
}

CBoolpoint::~CBoolpoint()
{
}

////////////////////////////////////////////
// CBoolblock 
////////////////////////////////////////////

CBoolblock::CBoolblock()
{
	meshbuffer = new SMeshBuffer; 

		vertices.push_back( video::S3DVertex(0,-5,0,0,0,1, video::SColor(255,255,255,255),0,0) ); 
		vertices.push_back( video::S3DVertex(5,-5,0,0,0,1 ,video::SColor(255,255,255,255),0,1) );
		vertices.push_back( video::S3DVertex(10,0,0,0,0,1, video::SColor(255,255,255,255),1,1) );
		vertices.push_back( video::S3DVertex(10,5,0,0,0,1, video::SColor(255,255,255,255),1,0) );    

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2); 
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3); 
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(1); 
		indices.push_back(0);
		indices.push_back(3);
		indices.push_back(2); 
}

CBoolblock::~CBoolblock()
{
	//delete meshbuffer;
}

void CBoolblock::createBlock(vector3df vPos, vector3df vScale)
{
	this->vScale = vScale;

	polygonize();

	meshbuffer->Vertices = vertices; 
	meshbuffer->Indices = indices; 

	//box.reset(vertices[0].Pos); 
	//for(int i=1;i<vertices.size();i++) 
	//{ 
	//	box.addInternalPoint(vertices[i].Pos); 
	//} 
	//meshbuffer->BoundingBox = box;

	mesh = new SMesh;
	mesh->addMeshBuffer( meshbuffer );

	blocknode = IRR.smgr->addMeshSceneNode( mesh );
	blocknode->setPosition(vPos);

	blocknode->setDebugDataVisible(true);
	blocknode->setMaterialTexture(0,	IRR.video->getTexture("Textures/whiterock.jpg"));
	blocknode->setMaterialFlag( EMF_LIGHTING, (bool)IRR.useLighting );
//	blocknode->setMaterialFlag( EMF_BACK_FACE_CULLING, true );
//	blocknode->setMaterialFlag( EMF_NORMALIZE_NORMALS, true);

	//delete mesh;
}

void CBoolblock::polygonize()
{
	vertices.clear();
	indices.clear();

	field[ 0 ][ 0 ].filled = true;
	field[ 1 ][ 0 ].filled = true;
	field[ 0 ][ 1 ].filled = true;
	field[ 1 ][ 1 ].filled = true;

	field[ 10 ][ 10 ].filled = true;
	field[ 11 ][ 10 ].filled = true;
	field[ 12 ][ 10 ].filled = true;
	field[ 13 ][ 10 ].filled = true;
	field[ 10 ][ 11 ].filled = true;
	field[ 11 ][ 11 ].filled = true;
	field[ 12 ][ 11 ].filled = true;
	field[ 13 ][ 11 ].filled = true;
	field[ 10 ][ 12 ].filled = true;
	field[ 11 ][ 12 ].filled = true;
	field[ 12 ][ 12 ].filled = true;
	field[ 13 ][ 12 ].filled = true;
	field[ 14 ][ 12 ].filled = true;

	field[ 12 ][ 10 ].filled = true;
	field[ 13 ][ 10 ].filled = true;
	field[ 14 ][ 10 ].filled = true;
	field[ 15 ][ 10 ].filled = true;
	field[ 12 ][ 11 ].filled = true;
	field[ 13 ][ 11 ].filled = true;
	field[ 14 ][ 11 ].filled = true;
	field[ 15 ][ 11 ].filled = true;
	field[ 12 ][ 12 ].filled = true;
	field[ 13 ][ 12 ].filled = true;
	field[ 14 ][ 12 ].filled = true;
	field[ 25 ][ 12 ].filled = true;
	field[ 21 ][ 12 ].filled = true;

	field[ 10 ][ 13 ].filled = true;
	field[ 11 ][ 13 ].filled = true;
	field[ 12 ][ 13 ].filled = true;
	field[ 13 ][ 13 ].filled = true;
	field[ 10 ][ 14 ].filled = true;
	field[ 11 ][ 14 ].filled = true;
	field[ 12 ][ 14 ].filled = true;
	field[ 13 ][ 14 ].filled = true;
	field[ 10 ][ 15 ].filled = true;
	field[ 11 ][ 15 ].filled = true;
	field[ 12 ][ 15 ].filled = true;

	field[ 14 ][ 12 ].filled = true;
	field[ 15 ][ 12 ].filled = true;
	field[ 16 ][ 12 ].filled = true;
	field[ 17 ][ 12 ].filled = true;
	field[ 14 ][ 13 ].filled = true;
	field[ 15 ][ 13 ].filled = true;
	field[ 16 ][ 13 ].filled = true;
	field[ 17 ][ 14 ].filled = true;
	field[ 16 ][ 14 ].filled = true;
	field[ 17 ][ 14 ].filled = true;
	field[ 18 ][ 14 ].filled = true;


	LoadField( "Maps/test.txt" );

	//fillAll(true);


	// calculate wieghts
    for ( int i = 1; i < 100 - 2; i += 1 )
    {
        for ( int j = 1; j < 100 - 2; j += 1 )
        {
			//if (!random(5))
			//	field[ i ][ j ].filled = false;

			if ( ( field[ i-1 ][ j ].filled )&&( field[ i+1 ][ j ].filled )&&( field[ i ][ j-1 ].filled )
				&&( field[ i ][ j+1 ].filled )&&( field[ i-1 ][ j-1 ].filled )&&( field[ i+1 ][ j-1 ].filled )
				&&( field[ i-1 ][ j+1 ].filled )&&( field[ i+1 ][ j+1 ].filled ) )
				field[ i ][ j ].weight += 1;
		}
	}

	// create vertices
	int x, y;
	f32 tu, tv;
	plane3df p( vector3df(0,0,0), vector3df(0,0,1) );

    for ( int j = 0; j < 100 - 1; j += 1 )
    {
        for ( int i = 0; i < 100 - 1; i += 1 )
        {
			x = i; y = j;

			//if (j > 1)
			//	if ( ( field[ i ][ j-1 ].filled )&&( !field[ i ][ j-2 ].filled ) )
			//	{
			//		y = j-1;
			//	}

			tu = x / vScale.Z;
			tv = y / vScale.Z;

			vertices.push_back( video::S3DVertex( x * vScale.X    , y * vScale.Y,     field[ i ][ j ].weight * vScale.Z,   0, 0, 1, video::SColor(255,255,255,255)
				, tu, tv) );
		}
	}

	int v1, v2, v3;
	vector3df vTemp;
	// create polygons
    for ( int i = 0; i < 100 - 1; i += 1 )
	{ 
        for ( int j = 0; j < 100 - 1; j += 1 )
        {
            // check the shape of the quad
            // **    **      *     *
            // *      *     **     **
            // 1      2     3      4

            if ( ( field[ i ][ j ].filled ) &&
                    ( field[ i + 1 ][ j ].filled ) &&
                    ( field[ i ][ j + 1 ].filled ) )
            {
				v1 = (j)*99 + i;
				v2 = (j)*99 + i+1;
				v3 = (j+1)*99 + i;
				indices.push_back( v1 );
				indices.push_back( v2 );
				indices.push_back( v3 ); 

				triangle3df t;
				t.set( vector3df(i, j, field[ i ][ j ].weight),
					   vector3df(i+1, j, field[ i+1 ][ j ].weight),
					   vector3df(i, j+1, field[ i ][ j+1 ].weight) );

				vertices[ v1 ].Normal += t.getNormal();
				vertices[ v2 ].Normal += t.getNormal();
				vertices[ v3 ].Normal += t.getNormal();
            }
            else
            if ( ( field[ i ][ j ].filled ) &&
                    ( field[ i + 1 ][ j ].filled ) &&
                    ( field[ i + 1 ][ j + 1 ].filled ) )
            {
				v1 = (j)*99 + i;
				v2 = (j)*99 + i+1;
				v3 = (j+1)*99 + i+1;
				indices.push_back( v1 );
				indices.push_back( v2 );
				indices.push_back( v3 ); 

				triangle3df t;
				t.set( vector3df(i, j, field[ i ][ j ].weight),
					   vector3df(i+1, j, field[ i+1 ][ j ].weight),
					   vector3df(i+1, j+1, field[ i+1 ][ j+1 ].weight) );

				vertices[ v1 ].Normal += t.getNormal();
				vertices[ v2 ].Normal += t.getNormal();
				vertices[ v3 ].Normal += t.getNormal();
            }

            if ( ( field[ i + 1 ][ j ].filled ) &&
                    ( field[ i + 1 ][ j + 1 ].filled ) &&
                    ( field[ i ][ j + 1 ].filled ) )
            {
				v1 = (j)*99 + i+1;
				v2 = (j+1)*99 + i+1;
				v3 = (j+1)*99 + i;
				indices.push_back( v1 );
				indices.push_back( v2 );
				indices.push_back( v3 ); 

				triangle3df t;
				t.set( vector3df(i+1, j, field[ i+1 ][ j ].weight),
					   vector3df(i+1, j+1, field[ i+1 ][ j+1 ].weight),
					   vector3df(i, j+1, field[ i ][ j+1 ].weight) );

				vertices[ v1 ].Normal += t.getNormal();
				vertices[ v2 ].Normal += t.getNormal();
				vertices[ v3 ].Normal += t.getNormal();

            }
            else
            if ( ( field[ i ][ j ].filled ) &&
                    ( field[ i + 1 ][ j + 1 ].filled ) &&
                    ( field[ i ][ j + 1 ].filled ) )
            {
				v1 = (j)*99 + i;
				v2 = (j+1)*99 + i+1;
				v3 = (j+1)*99 + i;
				indices.push_back( v1 );
				indices.push_back( v2 );
				indices.push_back( v3 ); 

				triangle3df t;
				t.set( vector3df(i, j, field[ i ][ j ].weight),
					   vector3df(i+1, j+1, field[ i+1 ][ j+1 ].weight),
					   vector3df(i, j+1, field[ i ][ j+1 ].weight) );

				vertices[ v1 ].Normal += t.getNormal();
				vertices[ v2 ].Normal += t.getNormal();
				vertices[ v3 ].Normal += t.getNormal();
            }

        }

    }

	// create polygons
    for ( int i = 0; i < 100 - 1; i += 1 )
	{ 
        for ( int j = 0; j < 100 - 1; j += 1 )
        {
            // check the shape of the quad
            //        **    *       *
            // **           *       *
            // 1      2     3      4
            if ( ( field[ i ][ j + 1 ].filled ) &&
                    ( field[ i + 1 ][ j + 1 ].filled ) &&
					( !field[ i ][ j ].filled ) &&
                    ( !field[ i + 1][ j ].filled ) )
            {

				v1 = (j+1)*99 + i;
				v2 = (j+1)*99 + i+1;
				indices.push_back( v2 );
				indices.push_back( v1 );

				x = i; y = j+1;
				tu = x / vScale.Z;
				tv = (y-1) / vScale.Z;
				vertices.push_back( video::S3DVertex( x * vScale.X    , y * vScale.Y,   -1 * vScale.Z,   0, 0, 1, video::SColor(255,255,255,255)
					, tu, tv) );
				indices.push_back( vertices.size()-1 ); 

				indices.push_back( v2 );
				indices.push_back( vertices.size()-1 );
				x = i+1; y = j+1;
				tu = x / vScale.Z;
				tv = (y-1) / vScale.Z;
				vertices.push_back( video::S3DVertex( x * vScale.X    , y * vScale.Y,   -1 * vScale.Z,   0, 0, 1, video::SColor(255,255,255,255)
					, tu, tv) );
				indices.push_back( vertices.size()-1 ); 

				triangle3df t;
				t.set( vector3df(i, j, -1),
					   vector3df(i, j+1, field[ i ][ j+1 ].weight),
					   vector3df(i+1, j+1, field[ i+1 ][ j+1 ].weight) );

				vertices[ v1 ].Normal += t.getNormal();
				vertices[ v2 ].Normal += t.getNormal();
				vertices[ vertices.size()-2 ].Normal += t.getNormal();

				t.set( vector3df(i, j, -1),
					   vector3df(i+1, j, -1),
					   vector3df(i+1, j+1, field[ i+1 ][ j+1 ].weight) );

				vertices[ v2 ].Normal += t.getNormal();
				vertices[ vertices.size()-2 ].Normal += t.getNormal();
				vertices[ vertices.size()-1 ].Normal += t.getNormal();
            }


			////////////////////////////////////////////
            // check the shape of the quad
            // **    **      *     *
            // *      *     **     **
            // 1      2     3      4

            if ( ( !field[ i ][ j ].filled ) &&
                    ( field[ i + 1 ][ j ].filled ) &&
					( field[ i ][ j + 1 ].filled ) &&
                    ( field[ i + 1][ j + 1 ].filled ) )
            {
				v1 = (j)*99 + i+1;
				v2 = (j+1)*99 + i;
				indices.push_back( v2 );
				indices.push_back( v1 );

				x = i; y = j+1;
				tu = x / vScale.Z;
				tv = (y-1) / vScale.Z;
				vertices.push_back( video::S3DVertex( x * vScale.X    , y * vScale.Y,   -1 * vScale.Z,   0, 0, 1, video::SColor(255,255,255,255)
					, tu, tv) );
				indices.push_back( vertices.size()-1 ); 

				indices.push_back( v1 );
				indices.push_back( vertices.size()-1 );
				x = i+1; y = j;
				tu = (x-1) / vScale.Z;
				tv = (y) / vScale.Z;
				vertices.push_back( video::S3DVertex( x * vScale.X    , y * vScale.Y,   -1 * vScale.Z,   0, 0, 1, video::SColor(255,255,255,255)
					, tu, tv) );
				indices.push_back( vertices.size()-1 ); 

				triangle3df t;
				t.set( vector3df(i, j, -1),
					   vector3df(i, j+1, field[ i ][ j+1 ].weight),
					   vector3df(i+1, j+1, field[ i+1 ][ j+1 ].weight) );

				vertices[ v1 ].Normal += t.getNormal();
				vertices[ v2 ].Normal += t.getNormal();
				vertices[ vertices.size()-2 ].Normal += t.getNormal();

				t.set( vector3df(i, j, -1),
					   vector3df(i+1, j, -1),
					   vector3df(i+1, j+1, field[ i+1 ][ j+1 ].weight) );

				vertices[ v2 ].Normal += t.getNormal();
				vertices[ vertices.size()-2 ].Normal += t.getNormal();
				vertices[ vertices.size()-1 ].Normal += t.getNormal();
            }
		}
	}

	for ( int j = 0; j < vertices.size(); j += 1 )
	{
		vertices[j].Normal.normalize();
	}
}

void CBoolblock::fillAll(bool value)
{
    for ( int i = 0; i < 100 - 1; i += 1 )
	{ 
        for ( int j = 0; j < 100 - 1; j += 1 )
        {
			field[i][j].filled = value;
		}
	}
}

void CBoolblock::LoadField( const c8* filename )
{
	std::string line;
	std::ifstream myfile ("Maps/test.txt");
	int linenum = 0;

	fillAll(false);

	if (myfile.is_open())
	{
		while (! myfile.eof() )
		{
			getline(myfile,line);
		//	CONSOLE.addx( "%s", line.c_str() );
			for ( int i = 0; i < 100 - 1; i += 1 )
			{
				if (line.c_str()[i] == '1')
					field[i][linenum].filled = true;
			}
			linenum++;
		}
		myfile.close();
	}
}