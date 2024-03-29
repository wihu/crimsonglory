// Copyright (C) 2002-2005 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// 
// This file was originally written by Salvatore Russo.
// I (Nikolaus Gebhardt) did some minor modifications changes to it and integrated 
// it into Irrlicht:
// - removed STL dependency
// - removed log file and replaced it with irrlicht logging
// - adapted code formatting a bit to Irrlicht style
// - removed memory leaks
// Thanks a lot to Salvatore for his work on this and that he gave me
// his permission to add it into Irrlicht. 

// This support library has been made by Salvatore Russo and is released under GNU public license for general uses.
// For uses in Irrlicht core and only for Irrlicht related uses I release this library under zlib license.
// It uses standard template libraries to create String class and StringList class used in DeleD
// plugins made by me.

#ifndef __DMF_SUPPORT2_H_INCLUDED__
#define __DMF_SUPPORT2_H_INCLUDED__

#include "../Engine/engine.h"
#include <stdlib.h>

#include "../App/app.h"
#include "../World/world.h"
#include "../World/map.h"
#include "../Irrlicht/IrrlichtTask.h"
#include "../IrrConsole/console.h"
#include "../World/calc.h"
#include "../Irrlicht/CLensFlaresSceneNode.h"
#include "../Irrlicht/CBackSpriteSceneNode.h"

using namespace irr;
using namespace video;

#define EXCLUDE !(wat1[0]==String("water") && wat[2]==String("0") ) && !(wat1[0]==String("sprite")) && !(wat1[0]==String("respawn")) \
	&& !(wat1[0]==String("sky")) && !(wat1[0]==String("fog")) && !(wat1[0]==String("zone")) && !(wat1[0]==String("cloud")) \
	&& !(wat1[0]==String("sun")) && !(wat1[0]==String("backstrip"))

#define IS_MYOBJECT ( ( wat1[0]==String("sprite") || wat1[0]==String("respawn") || wat1[0]==String("sky") || wat1[0]==String("fog") \
	 || wat1[0]==String("zone") || wat1[0]==String("cloud") || wat1[0]==String("sun") || wat1[0]==String("backstrip") \
	 ) && ( wat[2]==String("0") || wat[2]==String("1") ) )
	

/** A structure rapresenting some DeleD infos. 
 This structure contains data about DeleD level file like: version, ambient colour, number of objects etc...*/
struct dmfHeader 
{ 
	//main file header
	f32 dmfVersion;     //!<File version
	char dmfName[128]; //!<Scene name
	SColor dmfAmbient; //!<Ambient color
	f32 dmfShadow;     //!<Shadow intensity
    u32 numObjects;    //!<Number of objects in this scene
	u32 numMaterials;  //!<Number of materials in this scene
	u32 numVertices;   //!<Total number of vertices faces*(vertices for each face)
	u32 numFaces;      //!<Total number of faces
	u32 numLights;     //!<Number of dynamic lights in this scene
	u32 numWatVertices; //!<Total number of vertices of water plains watfaces*(vertices for each face)
	u32 numWatFaces;   //!<Total number of faces for water plains.Note that each water plane is a rectangle with one face only.
};


/** A structure rapresenting a DeleD material.
This structure contains texture names, an ID and some flags.*/
struct dmfMaterial
{
    u32 materialID;//!<This material unique ID.
    u32 textureFlag;//!<First texture Flag (0=Normal, 1=Color).
    u32 lightmapFlag;//!<Lightmap Flag (0=Normal, others not considered).
    u32 textureBlend;//!<Texture Blend mode used to support alpha maps (4=Alpha map, others not implemented yet).
    char textureName[64];//!<Name of first texture (only file name, no path).
    char lightmapName[64];//!<Name of lightmap (only file name, no path).             
};


/** A structure rapresenting a single face.
This structure contains first vertice index, number of vertices and the material used.*/
struct dmfFace 
{
	u32 firstVert;//!<First vertice index.
	u32 numVerts;//!<Number of vertices for this face.
	u32 materialID;//!<Material used for this face.
};


/** A structure rapresenting a single vertice.
This structure contains vertice position coordinates and texture an lightmap UV.*/
struct dmfVert 
{
	f32 tc[2];//!<Texture UV coords.
	f32 lc[2];//!<Lightmap UV coords.
	f32 pos[3];//!<Position of vertice x,y,z
};


/** A structure rapresenting a single dynamic light.
This structure contains light position coordinates, diffuse colour, specular colour and maximum radius of light.*/
struct dmfLight 
{
	f32 pos[3];//!<Position of this light.
	SColorf diffuseColor;//!<Diffuse color.
	SColorf specularColor;//!<Specular color.
    f32 radius;//!<Maximun radius of light.
};
		
/** A structure rapresenting a single water plane.
This structure contains light position coordinates, diffuse colour, specular colour and maximum radius of light.*/
struct dmfWaterPlain
{
	u32 waterID;//!<ID of specified water plane.
	u32 numFaces;//!<number of faces that make this plain.Owing to the fact that this is a rectangle you'll have 1 every time.
	u32 firstFace;//!<first face of this plain.
	core::dimension2d<s32> tileNum;//!<number of tiles of this water plain.
	f32 waveHeight;//!<height of waves.
	f32 waveSpeed;//!<waves speed.
	f32 waveLength;//!<waves lenght.			
};


/** A function to convert a 2byte hexstring to a int.
This function converts an hex string of 2 bytes (i.e. FF) to its int value (i.e. 255).
\return An int rapresenting the hex input value.*/
int axtoi(const char *hexStg)
{
  int n = 0;         // position in string
  int m = 0;         // position in digit[] to shift
  int count;         // loop index
  int intValue = 0;  // integer value of hex string
  int digit[5];      // hold values to convert

  while (n < 2) 
  {
     if (hexStg[n]=='\0')
        break;

     if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
        digit[n] = hexStg[n] & 0x0f;            //convert to int
     else 
	 if (hexStg[n] >='a' && hexStg[n] <= 'f') //if a to f
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
     else 
     if (hexStg[n] >='A' && hexStg[n] <= 'F') //if A to F
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
     else 
		 break;

    n++;
  }

  count = n;
  m = n - 1;
  n = 0;

  while(n < count)
  {
     // digit[n] is value of hex digit at position n
     // (m << 2) is the number of positions to shift
     // OR the bits into return value
     intValue = intValue | (digit[n] << (m << 2));
     m--;   // adjust the position to set
     n++;   // next digit to process
  }

  return (intValue);
}


/** A standard string.
A standard string created with standard template libraries.*/
typedef core::stringc stdstring;
typedef core::stringc String;

//Define a class StringList based on core::array and the defined class String
/** A simple stringlist class based on core::array.
This StringList class is based on core::array and the defined String class.
*/
class StringList : public core::array<String> 
{
public:

    /**Basic Constructor*/
    StringList()
	{
	}

    /**Constructor based on file loading.
    Look at LoadFromFile specifications.*/
	StringList(io::IReadFile* file)
	{
		LoadFromFile(file);
	}

    /**Basic destructor.*/
    ~StringList()
	{
		clear();
	}

    //Adds a String to StringList
    /** Add a string to this StringList.*/
    void Add(String str/*<String to add to the current StringList*/)
	{
		push_back(str);
	}
        
    //Loads a stringlist from a file
    //note that each String added to StringList
    //is separed by a \\n character and it's present
    //at the end of line.
    /** Loads a StringList from a file.
    This function loads a StringList from a file where each string is divided by a \\n char.*/
    void LoadFromFile(io::IReadFile* file)
	{
		int sz = file->getSize();
		char* buf = new char[sz+1];
		file->read(buf, sz);
		buf[sz] = 0;
		char* p = buf;
		char* start = p;

		while(*p)
		{
			if (*p == '\n')
			{
				core::stringc str(start, p - start - 1);
				str.trim();
				Add(str);
				start = p+1;
			}		

			++p;
		}

		if (p - start > 1)
		{
			core::stringc str(start, p - start - 1);
			str.trim();
			Add(str);
		}

		delete [] buf;
    }
        
	
	//GetText returns a String containing all the text included in
    //the StringList
    /** Returns all strings contained in the StringList as a single string.
    \return A single String containing all items.*/
    String GetText()
	{
        String str;
        for(int i=0; i<(int)size(); ++i)
            str += (*this)[i];

        return str;
    }   

};

  //This function subdivides a string in a list of strings
/** This function subdivides strings divided by divider in a list of strings.
\return A StringList made of all strings divided by divider.*/
StringList SubdivideString(String str, String divider)
{
    StringList strings; //returned StringList
    strings.clear();    //clear returned stringlist
    
    int c=0;
	int l=str.size();
    
    //process entire string
    while(c<l)
	{
        String resultstr;
        resultstr = "";
        //read characters until divider is encountered
        while((str[c]!=divider[0]) && c<l)
		{
            resultstr += str[c];
            c++;
        }    

        //Remove spaces \t and \n from string in my implementation...
        //pay attention or change it in dll.h if you don't want to remove
        //a particular char.
        resultstr.trim();//trims string resultstr
        strings.Add(resultstr);//add trimmed string
        c++;
    }    

	return strings;
}


//Get DeleD informations and convert in dmfHeader
/**This function extract a dmfHeader from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile.
\return true if function succeed or false on fail.*/
bool GetDMFHeader (StringList RawFile,
                   dmfHeader & header)
{
          StringList temp=SubdivideString(RawFile[0],String(";")); //file info

	      if ( temp[0] != String("DeleD Map File") )
			  return false; //not a deled file

	      temp.clear();
          temp = SubdivideString(RawFile[1],String(" "));//get version
	      StringList temp1=SubdivideString(temp[1],String(";"));

          if (atof(temp1[0].c_str()) < 0.91) 
			  return false;//not correct version

          header.dmfVersion = (float)atof(temp1[0].c_str());//save version
          temp.clear(); 
          temp1.clear();
          temp = SubdivideString(RawFile[2],String(";"));//get name,ambient color and shadow opacity
          sprintf(header.dmfName,"%s",temp[0].c_str());//save name

          String red=String(""), green=String(""), blue=String("");
          
          //get color value in hex
          blue.append((char*)&temp[1][2]);
		  blue.append((char*)&temp[1][3]);
          green.append((char*)&temp[1][4]);
		  green.append((char*)&temp[1][5]);
          red.append((char*)&temp[1][6]);
		  red.append((char*)&temp[1][7]);
          
          //set ambient color
          header.dmfAmbient.set(0, axtoi(red.c_str()),
			  axtoi(green.c_str()), axtoi(blue.c_str()));
          
          //set Shadow intensity
          header.dmfShadow = (float)atof(temp[2].c_str());
          
          //set current position
          int offs=3;
          
          //set Materials Number
          header.numMaterials=atoi(RawFile[offs].c_str());
          offs=offs + header.numMaterials;
          offs++;
          
          //set Object Number
          header.numObjects=atoi(RawFile[offs].c_str());
          
          //retrieve face and vertices number
          header.numVertices=0;
          header.numFaces=0;
          header.numWatFaces=0;
          header.numWatVertices=0;
          offs++;

          s32 fac;
		  int i;

          for(i=0; i < (int)header.numObjects; i++)
		  {
             StringList wat=SubdivideString(RawFile[offs],String(";"));
             StringList wat1=SubdivideString(wat[0],String("_"));

             offs++;
             offs = offs + atoi(RawFile[offs].c_str());
             offs++;

             fac=atoi(RawFile[offs].c_str());

             if( EXCLUDE )
				 header.numFaces = header.numFaces + fac;
             else
				 header.numWatFaces = header.numWatFaces + fac;

             offs++;
             
			 for(int j=0; j<fac; j++)
                 if( EXCLUDE )
					 header.numVertices=header.numVertices + atoi(RawFile[offs+j].c_str());
                 else 
					 header.numWatVertices=header.numWatVertices + atoi(RawFile[offs + j].c_str());      

             offs = offs + fac;
          }
          
          //retrieve number of dynamic lights
          header.numLights=0;
          temp.clear();
          temp1.clear();
          s32 lit = atoi(RawFile[offs].c_str());

          for (i=0; i<lit; i++)
		  {
             offs++;
             temp=SubdivideString(RawFile[offs],String(";"));

             if(atoi(temp[0].c_str())==1){
                temp1=SubdivideString(temp[18],String("_"));

             if(temp1[0]==String("dynamic")) 
				 header.numLights++;
          }
             temp.clear();
             temp1.clear();
      }
             
   return true; //everything is OK so loading is correct
}



/**This function extract an array of dmfMaterial from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile.
\return true if function succeed or false on fail.*/          
bool GetDMFMaterials(StringList RawFile /**<StringList representing a DMF file.*/,
                     dmfMaterial materials[]/**<Materials returned.*/,
                     int num_material/**<Number of materials contained in DMF file.*/,
                     bool use_material_dirs=false/**<Here you can choose to use default DeleD structure for material dirs.*/){
     int offs=4;
     StringList temp;
     StringList temp1;
     StringList temp2;

     //Checking if this is a DeleD map File of version >= 0.91
     temp=SubdivideString(RawFile[0],String(";"));//file info

     if ( temp[0] != String("DeleD Map File") ) 
		 return false;//not a deled file

     temp.clear();
     temp=SubdivideString(RawFile[1],String(" "));//get version
     temp1=SubdivideString(temp[1],String(";"));

     if (atof(temp1[0].c_str()) < 0.91) 
		 return false;//not correct version

     //end checking
     temp.clear();
     temp1.clear();
     
     for(int i=0;i<num_material; ++i)
	 {
             temp=SubdivideString(RawFile[offs+i],";");
             materials[i].materialID = i;
             temp1=SubdivideString(temp[5],",");

             materials[i].textureFlag = atoi(temp1[0].c_str());
             if(!use_material_dirs){
             temp2=SubdivideString(temp1[1],"\\");

             sprintf(materials[i].textureName, "%s", temp2[temp2.size()-1].c_str());
             }
             else sprintf(materials[i].textureName, "%s", temp1[1].c_str());
             materials[i].textureBlend = atoi(temp1[2].c_str());
             temp1.clear();
             temp2.clear();
             int a=temp.size();
/*             sprintf(buffer,"Size material %d: %d",i,a);
             MessageBox (NULL,"Debug",buffer,0);*/
             if(a>=9)
			 {
                temp1=SubdivideString(temp[temp.size() - 1],",");
                materials[i].lightmapFlag=atoi(temp1[0].c_str());
                if(!use_material_dirs){
                temp2=SubdivideString(temp1[1],"\\");
                sprintf(materials[i].lightmapName,"%s",temp2[temp2.size() - 1].c_str());
                }
                else sprintf(materials[i].lightmapName,"%s",temp1[1].c_str());
			}
             else 
			 {
                  materials[i].lightmapFlag=1;
                  sprintf(materials[i].lightmapName,"");
             }
             temp1.clear();
             temp2.clear();
     }
     return true;
}


/**This function extract an array of dmfMaterial from a DMF file considering 1st an 2nd layer for water plains.
You must give in input a StringList representing a DMF file loaded with LoadFromFile.
\return true if function succeed or false on fail.*/          
bool GetDMFWaterMaterials(StringList RawFile /**<StringList representing a DMF file.*/,
                     dmfMaterial materials[]/**<Materials returned.*/,
                     int num_material/**<Number of materials contained in DMF file.*/){
     int offs=4;
     StringList temp;
     StringList temp1;
     StringList temp2;
     //Checking if this is a DeleD map File of version >= 0.91
     temp=SubdivideString(RawFile[0],String(";"));//file info

     if ( temp[0] != String("DeleD Map File") ) 
		 return false;//not a deled file

     temp.clear();
     temp=SubdivideString(RawFile[1],String(" "));//get version
     temp1=SubdivideString(temp[1],String(";"));

     if (atof(temp1[0].c_str()) < 0.91) 
		 return false;//not correct version

     //end checking
     temp.clear();
     temp1.clear();

     for(int i=0;i<num_material;i++)
	 {
             temp = SubdivideString(RawFile[offs+i],";");
             materials[i].materialID=i;

             temp1 = SubdivideString(temp[5],",");
             materials[i].textureFlag=atoi(temp1[0].c_str());
             temp2 = SubdivideString(temp1[1],"\\");

             sprintf(materials[i].textureName,"%s",temp2[temp2.size()-1].c_str());
             temp1.clear();
             temp2.clear();
             int a=temp.size();
             if(a==7)
			 {
                temp1=SubdivideString(temp[6],",");
                materials[i].lightmapFlag=atoi(temp1[0].c_str());
                temp2=SubdivideString(temp1[1],"\\");
                sprintf(materials[i].lightmapName,"%s",temp2[temp2.size() - 1].c_str());
             }
             else
			 {
                  materials[i].lightmapFlag=1;
                  sprintf(materials[i].lightmapName,"FFFFFFFF");
             }
             temp1.clear();
             temp2.clear();
     }
     return true;
}
/**This function extract an array of dmfVert and dmfFace from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile and two arrays long enough.
Please use GetDMFHeader() before this function to know number of vertices and faces.
\return true if function succeed or false on fail.*/          
bool GetDMFVerticesFaces(StringList RawFile/**<StringList representing a DMF file.*/,
                         dmfVert vertices[]/**<Vertices returned*/,
                         dmfFace faces[]/**Faces returned*/){
     int offs=3;
     int offs1=0;
     StringList temp,temp1;
     
      //Checking if this is a DeleD map File of version >= 0.91
     temp=SubdivideString(RawFile[0],String(";"));//file info

     if ( temp[0] != String("DeleD Map File") ) 
		 return false;//not a deled file

     temp.clear();
     temp=SubdivideString(RawFile[1],String(" "));//get version
     temp1=SubdivideString(temp[1],String(";"));

     if (atof(temp1[0].c_str()) < 0.91) 
		 return false;//not correct version
     //end checking
     
     temp.clear();
     temp1.clear();
     offs=offs + atoi(RawFile[offs].c_str());
     offs++;

     s32 objs = atoi(RawFile[offs].c_str());
     s32 fac=0, vert=0, tmp_sz=0, vert_cnt=0, face_cnt=0;
     offs++;

     for (int i=0; i<objs; i++)
	 {
             StringList wat=SubdivideString(RawFile[offs],";");
             StringList wat1=SubdivideString(wat[0],"_");

             offs++;
             offs1=offs;
             offs=offs + atoi(RawFile[offs].c_str());
             offs++;
             offs1++;
             fac=atoi(RawFile[offs].c_str());
             offs++;
             if ( EXCLUDE )
			 {
             for(int j=0;j<fac;j++)
			 {
                     temp=SubdivideString(RawFile[offs+j],";");
                     
                     //first value is vertices number for this face
                     faces[face_cnt].numVerts=atoi(temp[0].c_str());
                     vert=faces[face_cnt].numVerts;
                     //second is material ID
                     faces[face_cnt].materialID=atoi(temp[1].c_str());
                     //vertices are ordined
                     faces[face_cnt].firstVert=vert_cnt;
                     
                     //now we'll create vertices structure
                     for(int k=0;k<vert;k++)
					 {
                             //get vertex position
                             temp1=SubdivideString(RawFile[offs1+atoi(temp[2+k].c_str())],";");
                             //copy x,y,z values
                             vertices[vert_cnt].pos[0] = (float)atof(temp1[0].c_str());
                             vertices[vert_cnt].pos[1] = (float)atof(temp1[1].c_str());
                             vertices[vert_cnt].pos[2] = (float)-atof(temp1[2].c_str());
                             //get uv coords for tex and light if any
                             vertices[vert_cnt].tc[0] = (float)atof(temp[2+vert+(2*k)].c_str());
                             vertices[vert_cnt].tc[1] = (float)atof(temp[2+vert+(2*k)+1].c_str());
                             tmp_sz=temp.size();
                             vertices[vert_cnt].lc[0] = (float)atof(temp[tmp_sz-(2*vert)+(2*k)].c_str());
                             vertices[vert_cnt].lc[1] = (float)atof(temp[tmp_sz-(2*vert)+(2*k)+1].c_str());
                             vert_cnt++; 
                             temp1.clear();
                     }

                     face_cnt++;
                     temp.clear();
                 }
             }

             offs=offs+fac;
	 }

  return true;
}

/**This function extract an array of dmfLights from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile
and one array long enough.
Please use GetDMFHeader() before this function to know number of dynamic lights.
\return true if function succeed or false on fail.*/          
bool GetDMFLights(StringList RawFile/**<StringList representing a DMF file.*/,
                         dmfLight lights[]/**<Lights returned.*/){
     
     int offs=3;
     int offs1=0;
     StringList temp,temp1;
     
      //Checking if this is a DeleD map File of version >= 0.91
     temp=SubdivideString(RawFile[0],String(";"));//file info

     if ( temp[0] != String("DeleD Map File") ) 
		 return false;//not a deled file

     temp.clear();
     temp=SubdivideString(RawFile[1],String(" "));//get version
     temp1=SubdivideString(temp[1],String(";"));

     if (atof(temp1[0].c_str()) < 0.91) 
		 return false;//not correct version

     //end checking
     
     temp.clear();
     temp1.clear();
     offs=offs + atoi(RawFile[offs].c_str());
     offs++;
     s32 objs = atoi(RawFile[offs].c_str());
     s32 lit=0;
     s32 d_lit=0;
     offs++;
     
     //let's get position of lights in file
	 int i;
     for(i=0;i<objs;i++)
	 {
        offs++;

        offs = offs + atoi(RawFile[offs].c_str());
        offs++;

        offs = offs + atoi(RawFile[offs].c_str());
        offs++; 
     }
     
     //let's find dynamic lights        
     lit = atoi(RawFile[offs].c_str());

     for(i=0;i<lit;i++)
	 {
             offs++;
             temp=SubdivideString(RawFile[offs],String(";"));
             if(atoi(temp[0].c_str())==1){
                temp1=SubdivideString(temp[18],String("_"));
                if(temp1[0]==String("dynamic")) 
				{
                   lights[d_lit].radius = (float)atof(temp[4].c_str());
                   lights[d_lit].pos[0] = (float)atof(temp[5].c_str());
                   lights[d_lit].pos[1] = (float)atof(temp[6].c_str());
                   lights[d_lit].pos[2] = (float)-atof(temp[7].c_str());

                   lights[d_lit].diffuseColor = SColorf(
					   SColor(255, atoi(temp[10].c_str()), atoi(temp[11].c_str()),
					     atoi(temp[12].c_str())));

                   lights[d_lit].specularColor = SColorf(
					   SColor(255, atoi(temp[13].c_str()), atoi(temp[14].c_str()),
					     atoi(temp[15].c_str())));

                   d_lit++;
                }
             }
             temp.clear();
             temp1.clear();
          }

    return true;
}
/**This function extract an array of dmfWaterPlain,dmfVert and dmfFace from a DMF file.
You must give in input a StringList representing a DMF file loaded with LoadFromFile and three arrays long enough.
Please use GetDMFHeader() before this function to know number of water plains and water faces as well as water vertices.
\return true if function succeed or false on fail.*/               
bool GetDMFWaterPlains(StringList RawFile/**<StringList representing a DMF file.*/,
                         dmfWaterPlain wat_planes[]/**<Water planes returned.*/,
                         dmfVert vertices[]/**<Vertices returned*/,
                         dmfFace faces[]/**Faces returned*/){
     int offs=3;
     int offs1=0;
     StringList temp,temp1;
     
      //Checking if this is a DeleD map File of version >= 0.91
     temp=SubdivideString(RawFile[0],String(";"));//file info

     if ( temp[0] != String("DeleD Map File") ) 
		 return false;//not a deled file

     temp.clear();
     temp=SubdivideString(RawFile[1],String(" "));//get version
     temp1=SubdivideString(temp[1],String(";"));

     if (atof(temp1[0].c_str()) < 0.91) 
		 return false;//not correct version

     //end checking
     
     temp.clear();
     temp1.clear();
     offs=offs+atoi(RawFile[offs].c_str());
     offs++;
     s32 objs=atoi(RawFile[offs].c_str());
     s32 fac=0,vert=0,tmp_sz=0,vert_cnt=0,face_cnt=0,wat_id=0;
     core::dimension2d<s32> tilenum(40,40);
     f32 waveheight=3.0f;
     f32 wavespeed=300.0f;
     f32 wavelength=80.0f;
     s32 tmp_s32=0;
     f32 tmp_f32=0.0f;
     offs++;

     for(int i=0;i<objs;i++)
	 {
             StringList wat=SubdivideString(RawFile[offs],";");
             StringList wat1=SubdivideString(wat[0],"_");
             offs++;
             offs1=offs;
             offs=offs+atoi(RawFile[offs].c_str());
             offs++;
             offs1++;
             fac=atoi(RawFile[offs].c_str());
             offs++;

             if(wat1[0]==String("water") && wat[2]==String("0"))
			 {
                StringList userinfo=SubdivideString(wat[7],",");
                for (int i=0; i<(int)userinfo.size(); i++)
				{
                        switch(i)
						{
                          case 0:
                               if(atoi(userinfo[0].c_str())) 
								   tilenum.Width = atoi(userinfo[0].c_str());
                               break;
                          case 1:
                               if(atoi(userinfo[1].c_str())) 
								   tilenum.Height = atoi(userinfo[1].c_str());
                               break;
                          case 2:
                               if(atof(userinfo[2].c_str())) 
								   waveheight = (float)atof(userinfo[2].c_str());
                               break;
                          case 3:
                               if(atof(userinfo[3].c_str())) 
								   wavespeed = (float)atof(userinfo[3].c_str());
                               break;
                          case 4:
                               if(atof(userinfo[4].c_str())) 
								   wavelength = (float)atof(userinfo[4].c_str());
                               
                               break;
				}
               }                         
                wat_planes[wat_id].waterID=wat_id;
                wat_planes[wat_id].numFaces=fac;
                wat_planes[wat_id].firstFace=face_cnt;
                wat_planes[wat_id].tileNum=tilenum;
                wat_planes[wat_id].waveHeight=waveheight;
                wat_planes[wat_id].waveSpeed=wavespeed;
                wat_planes[wat_id].waveLength=wavelength;

             for(int j=0;j<fac;j++)
			 {
                     temp=SubdivideString(RawFile[offs+j],";");
                     
                     //first value is vertices number for this face
                     faces[face_cnt].numVerts=atoi(temp[0].c_str());
                     vert=faces[face_cnt].numVerts;
                     //second is material ID
                     faces[face_cnt].materialID=atoi(temp[1].c_str());
                     //vertices are ordined
                     faces[face_cnt].firstVert=vert_cnt;
                     
                     //now we'll create vertices structure
                     for(int k=0;k<vert;k++)
					 {
                             //get vertex position
                             temp1=SubdivideString(RawFile[offs1+atoi(temp[2+k].c_str())], ";");

                             //copy x,y,z values
                             vertices[vert_cnt].pos[0]=(float)atof(temp1[0].c_str());
                             vertices[vert_cnt].pos[1]=(float)atof(temp1[1].c_str());
                             vertices[vert_cnt].pos[2]=(float)-atof(temp1[2].c_str());

                             //get uv coords for tex and light if any
                             vertices[vert_cnt].tc[0]=(float)atof(temp[2+vert+(2*k)].c_str());
                             vertices[vert_cnt].tc[1]=(float)atof(temp[2+vert+(2*k)+1].c_str());
                             tmp_sz=temp.size();

                             vertices[vert_cnt].lc[0]=(float)atof(temp[tmp_sz-(2*vert)+(2*k)].c_str());
                             vertices[vert_cnt].lc[1]=(float)atof(temp[tmp_sz-(2*vert)+(2*k)+1].c_str());
                             vert_cnt++; 
                             temp1.clear();
                      }
                     face_cnt++;
                     temp.clear();
                     };
             }
             offs=offs+fac;
       }
 
	 return true;
} 


////////////////////////////////////////////

bool GetDMFSprites(StringList RawFile, vector3df vScale)
{
	 int offs=3;
     int offs1=0;
     StringList temp,temp1;
     
      //Checking if this is a DeleD map File of version >= 0.91
     temp=SubdivideString(RawFile[0],String(";"));//file info

     if ( temp[0] != String("DeleD Map File") ) 
		 return false;//not a deled file

     temp.clear();
     temp=SubdivideString(RawFile[1],String(" "));//get version
     temp1=SubdivideString(temp[1],String(";"));

     if (atof(temp1[0].c_str()) < 0.91) 
		 return false;//not correct version

     //end checking
     
     temp.clear();
     temp1.clear();
     offs=offs+atoi(RawFile[offs].c_str());
     offs++;
     s32 objs=atoi(RawFile[offs].c_str());
     s32 fac=0,vert=0,tmp_sz=0,vert_cnt=0,face_cnt=0,wat_id=0;
     offs++;

	 dmfVert vertices[4*6];
	 dmfFace faces[2*6];

     for(int i=0;i<objs;i++)
	 {
             StringList wat=SubdivideString(RawFile[offs],";");
             StringList wat1=SubdivideString(wat[0],"_");
             offs++;
             offs1=offs;
             offs=offs+atoi(RawFile[offs].c_str());
             offs++;
             offs1++;
             fac=atoi(RawFile[offs].c_str());
             offs++;

			 face_cnt = vert_cnt = 0;

			// printf("LOL\n");
             if IS_MYOBJECT
			 {
				//CONSOLE.addx( " Loading %s", wat1[0] );
                StringList userinfo=SubdivideString(wat[7],",");
                for (int i=0; i<(int)userinfo.size(); i++)
				{
					//CONSOLE.add( userinfo[i].c_str() );
                }    


				for(int j=0;j<fac;j++)
				{
						temp=SubdivideString(RawFile[offs+j],";");
	                     
						//first value is vertices number for this face
						faces[face_cnt].numVerts=atoi(temp[0].c_str());
						vert=faces[face_cnt].numVerts;
						//second is material ID
						faces[face_cnt].materialID=atoi(temp[1].c_str());
						//vertices are ordined
						faces[face_cnt].firstVert=vert_cnt;

			//			CONSOLE.addx( " vert %i", vert );
	                     
						//now we'll create vertices structure
						for(int k=0;k<vert;k++)
						{
								//get vertex position
								temp1=SubdivideString(RawFile[offs1+atoi(temp[2+k].c_str())], ";");

								//copy x,y,z values
								vertices[vert_cnt].pos[0]=(float)atof(temp1[0].c_str()) * vScale.X;
								vertices[vert_cnt].pos[1]=(float)atof(temp1[1].c_str()) * vScale.Y;
								vertices[vert_cnt].pos[2]=(float)-atof(temp1[2].c_str()) * vScale.Z;

								//get uv coords for tex and light if any
								vertices[vert_cnt].tc[0]=(float)atof(temp[2+vert+(2*k)].c_str()) * vScale.X;
								vertices[vert_cnt].tc[1]=(float)atof(temp[2+vert+(2*k)+1].c_str()) * vScale.Y;
								tmp_sz=temp.size();

								vertices[vert_cnt].lc[0]=(float)atof(temp[tmp_sz-(2*vert)+(2*k)].c_str()) * vScale.X;
								vertices[vert_cnt].lc[1]=(float)atof(temp[tmp_sz-(2*vert)+(2*k)+1].c_str()) * vScale.Y;
								vert_cnt++; 
						//		CONSOLE.addx( " vert_cnt %i", vert_cnt );
								temp1.clear();
						}
						face_cnt++;
						temp.clear();
						};

					// CREATE SPRITES
					CAnimSpriteSceneNode* Sprite = 0;

					// STATIC SPRITE
					if ( userinfo[0] == "static" )
						if (userinfo.size() > 4)
						{
							Sprite = new CAnimSpriteSceneNode(IRR.smgr->getRootSceneNode(), IRR.smgr, -1); 

							Sprite->Load( APP.useFile( userinfo[1].c_str() ).c_str(), (bool) atoi(userinfo[3].c_str()), (bool) atoi(userinfo[4].c_str()) ); 

							vector3df vert[4];
							for (int i=0; i < 4; i++ )
							{
								vert[i].X = vertices[i].pos[0];
								vert[i].Y = vertices[i].pos[1];
								vert[i].Z = vertices[i].pos[2];
								//CONSOLE_VECTOR(vert[i]);						
							}	 
							Sprite->setVertice( 0, vert[3] ); 
							Sprite->setVertice( 1, vert[2] ); 
							Sprite->setVertice( 2, vert[1] ); 
							Sprite->setVertice( 3, vert[0] ); 

							Sprite->setMaterialType( (E_MATERIAL_TYPE) atoi(userinfo[2].c_str()) );

							Sprite->updateAbsolutePosition();
						}

					// ANIMATED SPRITE
					if ( userinfo[0] == "animated" )
						if (userinfo.size() > 10)
						{												
							Sprite = new CAnimSpriteSceneNode(IRR.smgr->getRootSceneNode(), IRR.smgr, -1); 

							Sprite->Load( APP.useFile( userinfo[1].c_str() ).c_str(), 
								atoi(userinfo[5].c_str()), atoi(userinfo[6].c_str()), atoi(userinfo[7].c_str()), atoi(userinfo[8].c_str()),
								atoi(userinfo[9].c_str()), atoi(userinfo[10].c_str()),
								(bool) atoi(userinfo[3].c_str()), (bool) atoi(userinfo[4].c_str()) ); 
							
							//Sprite->Load( userinfo[1].c_str(), clrkey); 

							vector3df vert[4];
							for (int i=0; i < 4; i++ )
							{
								vert[i].X = vertices[i].pos[0];
								vert[i].Y = vertices[i].pos[1];
								vert[i].Z = vertices[i].pos[2];
								//CONSOLE_VECTOR(vert[i]);						
							}	 
							Sprite->setVertice( 0, vert[3] ); 
							Sprite->setVertice( 1, vert[2] ); 
							Sprite->setVertice( 2, vert[1] ); 
							Sprite->setVertice( 3, vert[0] ); 

							Sprite->setMaterialType( (E_MATERIAL_TYPE) atoi(userinfo[2].c_str()) );

							Sprite->setSpeed( atoi(userinfo[11].c_str()) );

							Sprite->updateAbsolutePosition();
						}


					if (Sprite)
						WORLD.GetMap()->addSprite( Sprite );


					// CREATE RESPAWN
					if ( wat1[0]==String("respawn") )
						if (userinfo.size() > 2)
						{
							vector3df vert[4];
							for (int i=0; i < 4; i++ )
							{
								vert[i].X = vertices[i].pos[0];
								vert[i].Y = vertices[i].pos[1];
								vert[i].Z = vertices[i].pos[2];
							}	 
							vector3df vCenter = (vert[0] + vert[1] + vert[2] + vert[3]) / 4;

							String pactor = "";
							String pactorScript = "";
							if (userinfo.size() > 4)
							{
								pactor = userinfo[3];
								pactorScript = userinfo[4];
							}

							if ( WORLD.GetMap() )
								WORLD.GetMap()->GetRespawn()->AddPoint( vCenter, userinfo[0].c_str(),
															atoi(userinfo[1].c_str()), atoi(userinfo[2].c_str()),
															pactor.c_str(), pactorScript.c_str() );
						//	CONSOLE.add( pactor.c_str() );
						//	CONSOLE.add( pactorScript.c_str() );
						}

					// CREATE SKY
					if ( wat1[0]==String("sky") )
						if (userinfo.size() > 2)
						{
							WORLD.GetMap()->NewSky( userinfo[0].c_str(), atoi(userinfo[1].c_str()), atoi(userinfo[2].c_str()) );
						}

					// CREATE FOG
					if ( wat1[0]==String("fog") )
						if (userinfo.size() > 8)
						{
							WORLD.GetMap()->NewFog( video::SColor(0,atoi(userinfo[0].c_str()),atoi(userinfo[1].c_str()),atoi(userinfo[2].c_str())),
							(bool)atoi(userinfo[3].c_str()), atof(userinfo[4].c_str()), atof(userinfo[5].c_str()),
							atof(userinfo[6].c_str()), (bool)atoi(userinfo[7].c_str()), (bool)atoi(userinfo[8].c_str()) );
						}

					// CREATE ZONE
					if ( wat1[0]==String("zone") )
						if (userinfo.size() > 0)
						{
							vector3df vert[4*6];
							for (int i=0; i < 4*6; i++ )
							{
								vert[i].X = vertices[i].pos[0];
								vert[i].Y = vertices[i].pos[1];
								vert[i].Z = vertices[i].pos[2];
//								CONSOLE_VECTOR( vert[i] );
							}
							WORLD.GetMap()->AddZone( aabbox3df( vert[15], vert[1] ), atoi(userinfo[0].c_str()) );
						}

					// CREATE CLOUD
					if ( wat1[0]==String("cloud") )
						if (userinfo.size() > 0)
						{
							vector3df vert[4*6];
							for (int i=0; i < 4*6; i++ )
							{
								vert[i].X = vertices[i].pos[0];
								vert[i].Y = vertices[i].pos[1];
								vert[i].Z = vertices[i].pos[2];
							}
							CMap_Cloud *cloud = new CMap_Cloud( aabbox3df( vert[15], vert[1] ), atoi(userinfo[0].c_str()) );
							WORLD.GetMap()->addCloud( cloud );
						}

					//// CREATE SUN
					if ( wat1[0]==String("sun") )
						if (userinfo.size() > 0)
						{
							//vector3df vert[4];
							//for (int i=0; i < 4; i++ )
							//{
							//	vert[i].X = vertices[i].pos[0];
							//	vert[i].Y = vertices[i].pos[1];
							//	vert[i].Z = vertices[i].pos[2];
							//}	 
							//vector3df vCenter = (vert[0] + vert[1] + vert[2] + vert[3]) / 4;

							//CLensFlareSceneNode* lf = new CLensFlareSceneNode( IRR.smgr->getRootSceneNode(), IRR.smgr, -1, 
							//	vCenter, (bool)atoi(userinfo[1].c_str()) );
							//lf->drop();
							//lf->setMaterialTexture( 0, IRR.video->getTexture( APP.useFile( userinfo[0].c_str() ).c_str() ) );						
							//WORLD.GetMap()->setSunLight( lf );
						}

					// CREATE BACKSTRIP SPRITEcba
					if ( wat1[0]==String("backstrip") )
						if (userinfo.size() > 2)
						{
							vector3df vert[4];
							for (int i=0; i < 4; i++ )
							{
								vert[i].X = vertices[i].pos[0];
								vert[i].Y = vertices[i].pos[1];
								vert[i].Z = vertices[i].pos[2];
							}	 
							vector3df vCenter = (vert[0] + vert[1] + vert[2] + vert[3]) / 4;

							ISceneNode *billnode = new CBackSpriteSceneNode(IRR.smgr->getRootSceneNode(), IRR.smgr, -1, 
							vCenter, dimension2d<f32>(atof(userinfo[1].c_str()), atof(userinfo[1].c_str())), atoi(userinfo[2].c_str()) );
							billnode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
							billnode->setMaterialTexture(0, IRR.video->getTexture( userinfo[0].c_str() ) );
						}

				}
				offs=offs+fac;

       }
 
	 return true;
} 



#endif /* __DMF_SUPPORT_H__ */

