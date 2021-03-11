#include "common.hlsl"

Texture2D PPTexture : register(t0);
Texture2D BloomTexture : register(t1);
SamplerState Linear : register(s0);

float4 main(in PS_IN In) : SV_Target
{
    float4 PPTexCol = PPTexture.Sample(Linear, In.TexCoord);
    float4 BloomTexCol = BloomTexture.Sample(Linear, In.TexCoord);
    
    float4 Out = PPTexCol + BloomTexCol;
    Out.a = 1.0;
    
    return Out;
}