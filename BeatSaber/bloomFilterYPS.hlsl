#include "common.hlsl"

Texture2D Texture : register(t0);
SamplerState Linear : register(s0);

float4 main(in PS_IN In) : SV_Target
{
    float4 TexCol = Weight.Weight0.x * Texture.Sample(Linear, In.TexCoord);
    TexCol += Weight.Weight0.y
     * (Texture.Sample(Linear, In.TexCoord + float2(0, +2.0f / SCREEN.y))
     + Texture.Sample(Linear, In.TexCoord + float2(0, -2.0f / SCREEN.y)));
    TexCol += Weight.Weight0.z
     * (Texture.Sample(Linear, In.TexCoord + float2(0, +4.0f / SCREEN.y))
     + Texture.Sample(Linear, In.TexCoord + float2(0, -4.0f / SCREEN.y)));
    TexCol += Weight.Weight0.w
     * (Texture.Sample(Linear, In.TexCoord + float2(0, +6.0f / SCREEN.y))
     + Texture.Sample(Linear, In.TexCoord + float2(0, -6.0f / SCREEN.y)));
    TexCol += Weight.Weight1.x
     * (Texture.Sample(Linear, In.TexCoord + float2(0, +8.0f / SCREEN.y))
     + Texture.Sample(Linear, In.TexCoord + float2(0, -8.0f / SCREEN.y)));
    TexCol += Weight.Weight1.y
     * (Texture.Sample(Linear, In.TexCoord + float2(0, +10.0f / SCREEN.y))
     + Texture.Sample(Linear, In.TexCoord + float2(0, -10.0f / SCREEN.y)));
    TexCol += Weight.Weight1.z
     * (Texture.Sample(Linear, In.TexCoord + float2(0, +12.0f / SCREEN.y))
     + Texture.Sample(Linear, In.TexCoord + float2(0, -12.0f / SCREEN.y)));
    TexCol += Weight.Weight1.w
     * (Texture.Sample(Linear, In.TexCoord + float2(0, +14.0f / SCREEN.y))
     + Texture.Sample(Linear, In.TexCoord + float2(0, -14.0f / SCREEN.y)));
    
    TexCol.a = 1.0;

    return (In.Diffuse * TexCol);
}