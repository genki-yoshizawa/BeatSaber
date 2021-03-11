#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    float4x4 wvp;
    wvp = mul(Projection, mul(View, World));
    Out.Position = mul(wvp, In.Position);

    Out.Normal = In.Normal;
    Out.TexCoord = In.TexCoord;

    Out.Diffuse = In.Diffuse * Material.Diffuse;
    //Out.Diffuse.a = In.Diffuse.a * Material.Diffuse.a;

}