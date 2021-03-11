#include "common.hlsl"

Texture2D Texture : register(t0);
Texture2D BlackTexture : register(t1);
SamplerState Linear : register(s0);

StructuredBuffer<int> isLight : register(t3);

float4 main(in PS_IN In, in uint inInstanceId : SV_InstanceID) : SV_Target
{
    //float4 TexCol = Texture.Sample(Linear, In.TexCoord) * step(0.5, isLight[inInstanceId]) + BlackTexture.Sample(Linear, In.TexCoord) + step(0.5, !isLight[inInstanceId]);
    float4 TexCol;
    if (isLight[inInstanceId])
    {
        TexCol = Texture.Sample(Linear, In.TexCoord);

    }
    else
    {
        TexCol = BlackTexture.Sample(Linear, In.TexCoord);
    }
    
    
    if (TexCol.a == 0)
        clip(-1); // If alpha = 0, don't draw
    
    float4 outDiffuse = In.Diffuse * TexCol;
    
    //線形フォグ

    float dist = distance(In.WorldPosition.xyz, CameraPosition.xyz) - 30.0f;
    float fog = saturate(dist * 0.02f);
    
    outDiffuse.rgb = outDiffuse.rgb * (1.0f - fog) + float3(0.2f, 0.2f, 0.2f) * fog;
    
    return outDiffuse;
}