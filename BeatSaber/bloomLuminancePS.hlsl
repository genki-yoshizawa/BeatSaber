#include "common.hlsl"

Texture2D Texture : register(t0);
SamplerState Linear : register(s0);

float4 main(in PS_IN In) : SV_Target
{
    float4 TexCol = Texture.Sample(Linear, In.TexCoord);
    
    float Bright = TexCol.r * 0.299 + TexCol.g * 0.587 + TexCol.b * 0.114;
    
    float4 Out;
    if (Bright < 0.7)
    {
        Out.rgb = 0;
    }
    else
    {
        Out.rgb = Bright;
        Out.a = 1.0f;
    }
    return Out;
}