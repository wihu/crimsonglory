
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
//float fTime;       // Light color
float4x4 mWorldViewProj;  // World * View * Projection transformation
float4x4 mInvWorld;       // Inverted world matrix
float4x4 mTransWorld;     // Transposed world matrix
float3 mLightPos;         // Light position
float4 mLightColor;       // Light color


// Vertex shader output structure
struct VS_OUTPUT
{
	float4 Position   : POSITION;   // vertex position 
	float4 Diffuse    : COLOR0;     // vertex diffuse color
	float2 TexCoord   : TEXCOORD0;  // tex coords
};


VS_OUTPUT vertexMain( in float4 vPosition : POSITION,
                      in float3 vNormal   : NORMAL,
                      float2 texCoord     : TEXCOORD0 )
{
	VS_OUTPUT Output;

	// transform position to clip space 
	Output.Position = mul(vPosition, mWorldViewProj);
	
	// transform normal 
	float3 normal = mul(vNormal, mInvWorld);
	
	// renormalize normal 
	normal = normalize(normal);
	
	// position in world coodinates
	float3 worldpos = mul(mTransWorld, vPosition);
	
	// calculate light vector, vtxpos - lightpos
	float3 lightVector = worldpos - mLightPos;
	
	// normalize light vector 
	lightVector = normalize(lightVector);
	
	// calculate light color 
	float3 tmp = dot(-lightVector, normal);
	tmp = lit(tmp.x, tmp.y, 1.0);
	
	tmp = mLightColor * tmp.y;
	Output.Diffuse = float4(tmp.x, tmp.y, tmp.z, 0);
	Output.TexCoord = texCoord;
	
	return Output;
}



// Pixel shader output structure
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


sampler2D tex0;
sampler2D tex1;
PS_OUTPUT pixelMain( float2 TexCoord : TEXCOORD0,
                     float4 Position : POSITION,
                     float4 Diffuse  : COLOR0 ) 
{ 
	PS_OUTPUT Output;
float fTime=0;
	//float4 col = tex2D( tex0, TexCoord );  // sample color map
	
mLightColor=sin(mLightColor*0.5);
	TexCoord.y = TexCoord.y + (sin(TexCoord.x*20)*mLightColor*0.05); 
	//TexCoord.x = TexCoord.x + (cos(TexCoord.y*20)*mLightColor*0.05); 
//TexCoord.x  = (TexCoord.x  *mLightColor)-1000; 



	float4  col= tex2D( tex1, TexCoord.xy); 
	//float4  col2= tex2D( tex1, TexCoord.yx); 
	// multiply with diffuse and do other senseless operations
	Output.RGBColor = col;
	Output.RGBColor.z *= 5.43*0.1;// makes it blueish
	Output.RGBColor.y *= 2.75*0.1;// makes it blueish
Output.RGBColor.x *= 1.5*.1;// makes it blueish
	return Output;
}

