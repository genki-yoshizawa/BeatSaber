#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    float4x4 wvp;
    wvp = mul(Projection, mul(View, World));
    Out.Position = mul(wvp, In.Position);

    Out.Normal = In.Normal;
    Out.TexCoord = In.TexCoord;

    float4 worldNormal, normal;
    normal = float4(In.Normal.xyz, 0.0);
    worldNormal = mul(normal, World);
    worldNormal = normalize(worldNormal);
    
    Out.WorldPosition = mul(World, In.Position);
    
    //if (Light.Enable)
    //{
    //    float light = 0.5 - 0.5 * dot(Light.Direction.xyz, worldNormal.xyz);

    //    Out.Diffuse = In.Diffuse * Material.Diffuse * light * Light.Diffuse;
    //    Out.Diffuse += In.Diffuse * Material.Ambient * Light.Ambient;
    //    Out.Diffuse += Material.Emission;
    //}
    //else
    //{
        Out.Diffuse = In.Diffuse * Material.Diffuse;
    //}

    Out.Diffuse.a = In.Diffuse.a * Material.Diffuse.a;

}