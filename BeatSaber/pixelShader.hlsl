#include "common.hlsl"

Texture2D Texture   : register(t0);
SamplerState Linear : register(s0); 

float4 main(in PS_IN In) : SV_Target
{ 
	float4 TexCol = Texture.Sample(Linear, In.TexCoord); 
    if (TexCol.a==0) clip(-1);// If alpha = 0, don't draw

    return(In.Diffuse * TexCol); 
}