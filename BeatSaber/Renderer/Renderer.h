#pragma once
#include "Main/Main.h"

// 頂点構造体
struct VERTEX_3D
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR4 Diffuse;
	D3DXVECTOR2 TexCoord;
};

// マテリアル構造体
struct MATERIAL
{
	D3DXCOLOR	Ambient;
	D3DXCOLOR	Diffuse;
	D3DXCOLOR	Specular;
	D3DXCOLOR	Emission;
	float		Shininess;
	float		Dummy[3];//16byte境界用
};

// マテリアル構造体
struct DX11_MODEL_MATERIAL
{
	MATERIAL		Material;
	class CTexture*	Texture;
};

// 描画サブセット構造体
struct DX11_SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};

struct LIGHT
{
	BOOL		Enable;
	BOOL		Dummy[3];//16byte境界用
	D3DXVECTOR4	Direction;
	D3DXCOLOR	Diffuse;
	D3DXCOLOR	Ambient;
};

struct GAUSS_WEIGHT
{
	XMFLOAT4 Weight0;
	XMFLOAT4 Weight1;
};

class CVertexBuffer;
class CIndexBuffer;
class CTexture;

class Renderer
{
private:
	// シングルトン
	static Renderer* m_Instance;
	Renderer(){}
	~Renderer(){}

	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new Renderer();
			if (m_Instance)
			{
				return;
			}
		}
	}

	int							m_WinSizeW = 0;
	int							m_WinSizeH = 0;

	ID3D11Device*				m_D3DDevice = nullptr;
	ID3D11DeviceContext*		m_ImmediateContext = nullptr;
	IDXGISwapChain*				m_SwapChain = nullptr;

	ID3D11VertexShader*			m_VertexShader = nullptr;
	ID3D11PixelShader*			m_PixelShader = nullptr;
	ID3D11InputLayout*			m_VertexLayout = nullptr;

	ID3D11DepthStencilState*	m_DepthStateEnable = nullptr;
	ID3D11DepthStencilState*	m_DepthStateDisable = nullptr;

	ID3D11Texture2D*			m_RenderTargetBackBuffer = nullptr;

public:
	static const int			UNIFORM_DATA_SIZE = 2000;
	static const int			MATRIX_SIZE = 64;
	static const int			LIGHT_SIZE = 64;
	static const int			MATERIAL_SIZE = 80;
	static const int			CAMERA_POSITION_SIZE = 16;
	static const int			GAUSS_WEIGHT_SIZE = 32;
	static const int			SCREEN_SIZE = 16;

	static const int			WORLD_MATRIX_START = 0;
	static const int			VIEW_MATRIX_START = 64;
	static const int			PROJECTION_MATRIX_START = 128;
	static const int			LIGHT_START = 192;
	static const int			MATERIAL_START = 256;
	static const int			CAMERA_POSITION_START = 336;
	static const int			GAUSS_WEIGHT_START = 352;
	static const int			SCREEN_START = 384;

	// defineの方が良い
	static const int WEIGHT_NUM = 8;
private:
	unsigned char				m_UniformData[UNIFORM_DATA_SIZE] = {};
	ID3D11Buffer*				m_D3DBuffer = nullptr;		//UNIFORM_DATA_SIZU分の大きさで作る
	ID3D11RenderTargetView*		m_RenderTargetView = nullptr;	//VRHandle内のRTVを使っているのでいらなそう
	ID3D11DepthStencilView*		m_DepthStencilView = nullptr;

	float m_Weight[WEIGHT_NUM];	// ガウシアンフィルター用ウェイト

public:
	Renderer(const Renderer&) = delete;
	Renderer & operator =(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer & operator=(Renderer&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static Renderer* GetInstance()
	{
		if (!m_Instance)
		{
			Create();
		}
		return m_Instance;
	}

	bool Init(bool windowed = true, int scale = 1);
	void Uninit();
	//void BloomTextureBegin(ID3D11DepthStencilView* depthStencil);
	void Begin(ID3D11RenderTargetView * rendertarget, ID3D11DepthStencilView* depthStencil);
	void End();

	void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);

	void SetViewport(float vpW, float vpH, float vpX = 0, float vpY = 0);

	void SetDepthEnable(bool Enable);
	//void SetWorldViewProjection2D(XMMATRIX projection_matrix);
	void SetWorldViewProjection2D(int eye);
	void SetWorldMatrix(XMMATRIX * WorldMatrix);
	void SetViewMatrix(XMMATRIX * ViewMatrix);
	void SetProjectionMatrix(XMMATRIX * ProjectionMatrix);
	void SetMaterial(MATERIAL Material);
	void SetLight(LIGHT Light);
	void SetCameraPosition(XMVECTOR CameraPosition);
	void SetCulling(D3D11_CULL_MODE Cull);
	void SetGaussWeight();
	void SetScreen(float w, float h);
	void SetUsualShader();

	int GetWinsizeW(void) { return m_WinSizeW; }
	int GetWinsizeH(void) { return m_WinSizeH; }

	ID3D11Device* GetDevice(void) { return m_D3DDevice; }
	ID3D11DeviceContext* GetDeviceContext(void) { return m_ImmediateContext; }

	ID3D11Texture2D* GetRenderTargetBackBuffer(void) { return m_RenderTargetBackBuffer; }

	unsigned char* GetUniformData(void) { return m_UniformData; }
	ID3D11Buffer* GetD3DBffer(void) { return m_D3DBuffer; }

//private:
//	ID3D11RenderTargetView*			m_PPRenderTargetView[2];
//	ID3D11ShaderResourceView*		m_PPShaderResourceView[2];
//
//public:
//	ID3D11RenderTargetView*	 GetPPRenderTargetView(int eye) { return m_PPRenderTargetView[eye]; }
//	ID3D11ShaderResourceView* GetPPTexture(int eye) { return m_PPShaderResourceView[eye]; }

};
