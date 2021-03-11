// ライト構造体
struct LIGHT
{
    bool Enable;
    bool3 Dummy; //16bit境界用
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
};

// マテリアル構造体
struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float Shininess;
    float3 Dummy; //16bit境界用
};

struct GAUSS_WEIGHT
{
    float4 Weight0;
    float4 Weight1;
};

float4x4 World;
float4x4 View;
float4x4 Projection;
LIGHT Light;
MATERIAL Material;
float4 CameraPosition;
GAUSS_WEIGHT Weight;
float4 SCREEN;

struct VS_IN
{
    float4 Position : POSITION;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct PS_IN
{
    float4 Position : SV_Position;
    float4 WorldPosition : POSITION0;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
};