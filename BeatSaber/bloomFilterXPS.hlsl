#include "common.hlsl"

Texture2D Texture : register(t0);
SamplerState Linear : register(s0);

float4 main(in PS_IN In) : SV_Target
{
    float4 TexCol = Weight.Weight0.x * Texture.Sample(Linear, In.TexCoord);
    TexCol += Weight.Weight0.y
     * (Texture.Sample(Linear, In.TexCoord + float2(+2.0f / SCREEN.x, 0))
     + Texture.Sample(Linear, In.TexCoord + float2(-2.0f / SCREEN.x, 0)));
    TexCol += Weight.Weight0.z
     * (Texture.Sample(Linear, In.TexCoord + float2(+4.0f / SCREEN.x, 0))
     + Texture.Sample(Linear, In.TexCoord + float2(-4.0f / SCREEN.x, 0)));
    TexCol += Weight.Weight0.w
     * (Texture.Sample(Linear, In.TexCoord + float2(+6.0f / SCREEN.x, 0))
     + Texture.Sample(Linear, In.TexCoord + float2(-6.0f / SCREEN.x, 0)));
    TexCol += Weight.Weight1.x
     * (Texture.Sample(Linear, In.TexCoord + float2(+8.0f / SCREEN.x, 0))
     + Texture.Sample(Linear, In.TexCoord + float2(-8.0f / SCREEN.x, 0)));
    TexCol += Weight.Weight1.y
     * (Texture.Sample(Linear, In.TexCoord + float2(+10.0f / SCREEN.x, 0))
     + Texture.Sample(Linear, In.TexCoord + float2(-10.0f / SCREEN.x, 0)));
    TexCol += Weight.Weight1.z
     * (Texture.Sample(Linear, In.TexCoord + float2(+12.0f / SCREEN.x, 0))
     + Texture.Sample(Linear, In.TexCoord + float2(-12.0f / SCREEN.x, 0)));
    TexCol += Weight.Weight1.w
     * (Texture.Sample(Linear, In.TexCoord + float2(+14.0f / SCREEN.x, 0))
     + Texture.Sample(Linear, In.TexCoord + float2(-14.0f / SCREEN.x, 0)));

    TexCol.a = 1.0;

    return (In.Diffuse * TexCol);
}