#include "common.hlsl"

Texture2D Texture : register(t0);
SamplerState Linear : register(s0);

float4 main(in PS_IN In) : SV_Target
{
    float4 TexCol = Texture.Sample(Linear, In.TexCoord);
    if (TexCol.a == 0)
        clip(-1); // If alpha = 0, don't draw
    
    float4 outDiffuse = In.Diffuse * TexCol;
    
    //線形フォグ

    float dist = distance(In.WorldPosition.xyz, CameraPosition.xyz);
    float fog = saturate(dist * 0.02f);
    
    outDiffuse.rgb = outDiffuse.rgb * (1.0f - fog) + float3(0.2f, 0.2f, 0.2f) * fog;
    
    
    return outDiffuse;
}