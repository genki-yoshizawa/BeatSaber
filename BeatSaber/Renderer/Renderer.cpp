#include "Renderer/Renderer.h"
#include "VR/VRHandle/VRHandle.h"
#include <io.h>

Renderer* Renderer::m_Instance = nullptr;

bool Renderer::Init(bool windowed, int scale)
{
	m_WinSizeW = VRHandle::GetInstance()->GetOVRHmd()->Resolution.w / 2;
	m_WinSizeH = VRHandle::GetInstance()->GetOVRHmd()->Resolution.h / 2;
	const LUID* pLuid = reinterpret_cast<LUID*>(VRHandle::GetInstance()->GetOVRLuId());
	
	if (scale == 0)
	{
		scale = 1;
	}

	// windowの生成
	RECT size = { 0, 0, m_WinSizeW / scale, m_WinSizeH / scale };
	AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, false);
	const UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
	// ShowWindowと同じ(flagsにSWP_SHOWWINDOWが含まれてるため)
	if (!SetWindowPos(GetWindow(), nullptr, 0, 0, size.right - size.left, size.bottom - size.top, flags))
	{
		return false;
	}
		
	// DXGIFactoryの生成
	IDXGIFactory * DXGIFactory = nullptr;
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&DXGIFactory));
	ERROR_CHECK((hr == ERROR_SUCCESS), "DXGIFacrotyの生成に失敗しました。");

	// Adapterの生成
	// OVRのLuidではないものに設定する（多分…）
	IDXGIAdapter * Adapter = nullptr;
	for (UINT iAdapter = 0; DXGIFactory->EnumAdapters(iAdapter, &Adapter) != DXGI_ERROR_NOT_FOUND; ++iAdapter)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		Adapter->GetDesc(&adapterDesc);
		if ((pLuid == nullptr) || memcmp(&adapterDesc.AdapterLuid, pLuid, sizeof(LUID)) == 0) 
		{
			break;
		}
		Release(Adapter);
	}

	// デバイス、デバイスコンテキストの作成
	UINT deviceCreateFlags = 0;

	auto DriverType = Adapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE;
	hr = D3D11CreateDevice(Adapter, DriverType, 0, deviceCreateFlags, 0, 0, D3D11_SDK_VERSION, &m_D3DDevice, 0, &m_ImmediateContext);

	Release(Adapter);
	ERROR_CHECK((hr == ERROR_SUCCESS), "デバイス、デバイスコンテキストの作成に失敗しました。");

	// スワップチェイン作成
	DXGI_SWAP_CHAIN_DESC scDesc;
	memset(&scDesc, 0, sizeof(scDesc));
	scDesc.BufferCount = 2;
	scDesc.BufferDesc.Width = m_WinSizeW;
	scDesc.BufferDesc.Height = m_WinSizeH;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = GetWindow();
	scDesc.SampleDesc.Count = 1;
	scDesc.Windowed = windowed;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	hr = DXGIFactory->CreateSwapChain(m_D3DDevice, &scDesc, &m_SwapChain);
	Release(DXGIFactory);
	ERROR_CHECK((hr == ERROR_SUCCESS), "スワップチェインの作成に失敗しました。");

	// レンダーターゲットビュー作成、設定
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_RenderTargetBackBuffer);
	hr = m_D3DDevice->CreateRenderTargetView(m_RenderTargetBackBuffer, nullptr, &m_RenderTargetView);
	ERROR_CHECK((hr == ERROR_SUCCESS), "レンダーターゲットビューの作成に失敗しました。");

	// ステンシル用テクスチャー作成
	DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT;
	D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Width = m_WinSizeW;
	dsDesc.Height = m_WinSizeH;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.Format = format;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.MiscFlags = 0;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ID3D11Texture2D * Tex;
	m_D3DDevice->CreateTexture2D(&dsDesc, nullptr, &Tex);

	// ステンシルターゲット設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = dsDesc.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags = 0;
	m_D3DDevice->CreateDepthStencilView(Tex, &dsvd, &m_DepthStencilView);
	Tex->Release();

	m_ImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	// Buffer for shader constants
	//UniformBufferGen = new DataBuffer(m_D3DDevice, D3D11_BIND_CONSTANT_BUFFER, nullptr, UNIFORM_DATA_SIZE);
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = (unsigned)UNIFORM_DATA_SIZE;
	D3D11_SUBRESOURCE_DATA sr;
	sr.pSysMem = nullptr;
	sr.SysMemPitch = sr.SysMemSlicePitch = 0;
	m_D3DDevice->CreateBuffer(&desc, nullptr, &m_D3DBuffer);

	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_D3DBuffer);
	m_ImmediateContext->PSSetConstantBuffers(0, 1, &m_D3DBuffer);
	{
		// サンプラーステート設定;
		D3D11_SAMPLER_DESC ss; memset(&ss, 0, sizeof(ss));
		ss.AddressU = ss.AddressV = ss.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		ss.Filter = D3D11_FILTER_ANISOTROPIC;
		ss.MaxAnisotropy = 8;
		ss.MaxLOD = 15;

		ID3D11SamplerState* SamplerState = nullptr;
		m_D3DDevice->CreateSamplerState(&ss, &SamplerState);

		m_ImmediateContext->PSSetSamplers(0, 1, &SamplerState);

		// ラスタライザステート設定
		D3D11_RASTERIZER_DESC rs; memset(&rs, 0, sizeof(rs));
		rs.AntialiasedLineEnable = rs.DepthClipEnable = true;
		rs.CullMode = D3D11_CULL_BACK;
		rs.FillMode = D3D11_FILL_SOLID;
		rs.FrontCounterClockwise = TRUE;

		ID3D11RasterizerState *Rasterizer;
		m_D3DDevice->CreateRasterizerState(&rs, &Rasterizer);

		m_ImmediateContext->RSSetState(Rasterizer);

		// 深度ステンシルステート設定
		D3D11_DEPTH_STENCIL_DESC dss;
		memset(&dss, 0, sizeof(dss));
		dss.DepthEnable = true;
		dss.DepthFunc = D3D11_COMPARISON_LESS;
		dss.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		m_D3DDevice->CreateDepthStencilState(&dss, &m_DepthStateEnable);

		//depthStencilDesc.DepthEnable = FALSE;
		dss.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		m_D3DDevice->CreateDepthStencilState(&dss, &m_DepthStateDisable);//深度無効ステート

		m_ImmediateContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);

		// ブレンドステート設定
		D3D11_BLEND_DESC bm;
		memset(&bm, 0, sizeof(bm));
		bm.RenderTarget[0].BlendEnable = true;
		bm.RenderTarget[0].BlendOp = bm.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bm.RenderTarget[0].SrcBlend = bm.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		bm.RenderTarget[0].DestBlend = bm.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		bm.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ID3D11BlendState* BlendState = NULL;
		m_D3DDevice->CreateBlendState(&bm, &BlendState);
		m_ImmediateContext->OMSetBlendState(BlendState, BlendFactor, 0xffffffff);

	}
	//for(int eye = 0; eye < 2; eye++)
	//{
	//	ovrSession* session = VRHandle::GetInstance()->GetOVRSession();
	//	ovrSizei idealSize = ovr_GetFovTextureSize(*session, (ovrEyeType)eye, VRHandle::GetInstance()->GetOVRHmd()->DefaultEyeFov[eye], 1.0f);
	//
	//	// ポストプロセス用テクスチャー作成
	//	ID3D11Texture2D* ppTexture = nullptr;
	//	D3D11_TEXTURE2D_DESC td;
	//	ZeroMemory(&td, sizeof(td));
	//	td.Width = idealSize.w;
	//	td.Height = idealSize.h;
	//	td.MipLevels = 1;
	//	td.ArraySize = 1;
	//	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //DXGI_FORMAT_R8G8B8A8_TYPELESS; 
	//	td.SampleDesc = scDesc.SampleDesc;
	//	td.Usage = D3D11_USAGE_DEFAULT;
	//	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//	td.CPUAccessFlags = 0;
	//	td.MiscFlags = 0;
	//	m_D3DDevice->CreateTexture2D(&td, NULL, &ppTexture);
	//
	//	assert(ppTexture);
	//
	//	// レンダーターゲットビュー作成
	//	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	//	memset(&rtvDesc, 0, sizeof(rtvDesc));
	//	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //DXGI_FORMAT_R8G8B8A8_UNORM;
	//	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//	m_D3DDevice->CreateRenderTargetView(ppTexture, &rtvDesc, &m_PPRenderTargetView[eye]);
	//
	//	assert(m_PPRenderTargetView);
	//
	//	//シェーダーリソースビュー作成
	//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	//	SRVDesc.Format = rtvDesc.Format;
	//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//	SRVDesc.Texture2D.MipLevels = 1;
	//	m_D3DDevice->CreateShaderResourceView(ppTexture, &SRVDesc, &m_PPShaderResourceView[eye]);
	//
	//	assert(m_PPShaderResourceView);
	//
	//	ppTexture->Release();
	//}
	CreateVertexShader(&m_VertexShader, &m_VertexLayout, "vertexShader.cso");
	CreatePixelShader(&m_PixelShader, "pixelShader.cso");

	// 入力レイアウト設定
	m_ImmediateContext->IASetInputLayout(m_VertexLayout);
	
	// シェーダ設定
	m_ImmediateContext->VSSetShader(m_VertexShader, NULL, 0);
	m_ImmediateContext->PSSetShader(m_PixelShader, NULL, 0);

	// 最大遅延フレームを1に設定
	IDXGIDevice1* DXGIDevice1 = nullptr;
	hr = m_D3DDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&DXGIDevice1);
	ERROR_CHECK((hr == ERROR_SUCCESS), "クエリインターフェースが失敗しました");
	DXGIDevice1->SetMaximumFrameLatency(1);
	Release(DXGIDevice1);

	{// ガウスフィルターのウェイト計算
		float disperision = 10000000.0f; //分散
		float total = 0.0f;
		for(int i = 0; i < WEIGHT_NUM; i++)
		{
			float pos = 1.0f + 2.0f * (float)i;
			m_Weight[i] = std::expf(-0.5f * pos * pos / disperision);

			if (i == 0)
			{
				total += m_Weight[i];
			}
			else
			{
				total += 2.0f * m_Weight[i];
			}
		}
		for(int i = 0; i < WEIGHT_NUM; i++)
		{
			m_Weight[i] /= total;
		}
		SetGaussWeight();
	}

	return true;
}

void Renderer::Uninit()
{
	// オブジェクト解放
	m_D3DBuffer->Release();
	m_RenderTargetBackBuffer->Release();

	m_SwapChain->SetFullscreenState(FALSE, nullptr);

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	m_ImmediateContext->ClearState();
	m_RenderTargetView->Release();
	m_SwapChain->Release();
	m_ImmediateContext->Release();
	m_D3DDevice->Release();
}

//void Renderer::BloomTextureBegin(ID3D11DepthStencilView* depthStencil)
//{
//	m_ImmediateContext->OMSetRenderTargets(1, &m_BloomRenderTargetView, depthStencil);
//
//	// バックバッファクリア
//	float ClearColor[4] = { 0.6f, 0.6f, 1.0f, 1.0f };
//	m_ImmediateContext->ClearRenderTargetView(m_BloomRenderTargetView, ClearColor);
//	m_ImmediateContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//}

void Renderer::Begin(ID3D11RenderTargetView * rendertarget, ID3D11DepthStencilView* depthStencil)
{
	// depthStencilいらない？
	float ClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	m_ImmediateContext->OMSetRenderTargets(1, &rendertarget, depthStencil);
	m_ImmediateContext->ClearRenderTargetView(rendertarget, ClearColor);
	if (depthStencil)
	{
		m_ImmediateContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
}

void Renderer::End()
{
	m_SwapChain->Present(0, 0);
}

void Renderer::SetViewport(float vpW, float vpH, float vpX, float vpY)
{
	D3D11_VIEWPORT D3Dvp;
	D3Dvp.Width = vpW;    D3Dvp.Height = vpH;
	D3Dvp.MinDepth = 0;   D3Dvp.MaxDepth = 1;
	D3Dvp.TopLeftX = vpX; D3Dvp.TopLeftY = vpY;
	m_ImmediateContext->RSSetViewports(1, &D3Dvp);
}

void Renderer::SetDepthEnable(bool Enable)
{
	if (Enable)
		m_ImmediateContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);
	else
		m_ImmediateContext->OMSetDepthStencilState(m_DepthStateDisable, NULL);
}

void Renderer::SetWorldViewProjection2D(int eye)
{
	XMMATRIX world;
	world = XMMatrixIdentity();
	world = XMMatrixTranspose(world);

	memcpy(m_UniformData + WORLD_MATRIX_START, &world, MATRIX_SIZE);

	XMMATRIX view;
	view = XMMatrixIdentity();
	view = XMMatrixTranspose(view);
	memcpy(m_UniformData + VIEW_MATRIX_START, &view, MATRIX_SIZE);

	XMMATRIX projection;
	ovrSession* session = VRHandle::GetInstance()->GetOVRSession();
	ovrSizei idealSize = ovr_GetFovTextureSize(*session, (ovrEyeType)eye, VRHandle::GetInstance()->GetOVRHmd()->DefaultEyeFov[eye], 1.0f);
	projection = XMMatrixOrthographicOffCenterLH(0.0f, idealSize.w, idealSize.h, 0.0f, 0.0f, 1.0f);

	memcpy(m_UniformData + PROJECTION_MATRIX_START, &projection, MATRIX_SIZE);

}

void Renderer::SetWorldMatrix(XMMATRIX *WorldMatrix)
{
	memcpy(m_UniformData + WORLD_MATRIX_START, WorldMatrix, MATRIX_SIZE);
}

void Renderer::SetViewMatrix(XMMATRIX *ViewMatrix)
{
	memcpy(m_UniformData + VIEW_MATRIX_START, ViewMatrix, MATRIX_SIZE);
}

void Renderer::SetProjectionMatrix(XMMATRIX *ProjectionMatrix)
{
	memcpy(m_UniformData + PROJECTION_MATRIX_START, ProjectionMatrix, MATRIX_SIZE);
}

void Renderer::SetLight(LIGHT Light)
{
	memcpy(m_UniformData + LIGHT_START, &Light, LIGHT_SIZE);
}

void Renderer::SetMaterial(MATERIAL Material)
{
	memcpy(m_UniformData + MATERIAL_START, &Material, MATERIAL_SIZE);
}

void Renderer::SetCameraPosition(XMVECTOR CameraPosition)
{
	memcpy(m_UniformData + CAMERA_POSITION_START, &XMFLOAT4(XMVectorGetX(CameraPosition), XMVectorGetY(CameraPosition), XMVectorGetZ(CameraPosition), XMVectorGetW(CameraPosition)), CAMERA_POSITION_SIZE);
}

void Renderer::SetCulling(D3D11_CULL_MODE Cull) 
{
	// Create rasterizer
	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rs; memset(&rs, 0, sizeof(rs));
	rs.AntialiasedLineEnable = rs.DepthClipEnable = true;
	rs.CullMode = Cull;
	rs.FillMode = D3D11_FILL_SOLID;
	rs.FrontCounterClockwise = TRUE;

	ID3D11RasterizerState *Rasterizer;
	m_D3DDevice->CreateRasterizerState(&rs, &Rasterizer);

	m_ImmediateContext->RSSetState(Rasterizer);
}

void Renderer::SetGaussWeight()
{
	memcpy(m_UniformData + GAUSS_WEIGHT_START, &m_Weight, GAUSS_WEIGHT_SIZE);
}

void Renderer::SetScreen(float w, float h)
{
	XMFLOAT4 screen = { w,h,0.0f,0.0f };
	memcpy(m_UniformData + SCREEN_START, &screen, SCREEN_SIZE);
}

void Renderer::SetUsualShader()
{
	// シェーダ設定
	Renderer::GetInstance()->GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);

	Renderer::GetInstance()->GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);
}

// 頂点シェーダ生成
void Renderer::CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
{

	// 入力レイアウト生成
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_D3DDevice->CreateVertexShader(buffer, fsize, NULL, VertexShader);

	UINT numElements = ARRAYSIZE(layout);

	m_D3DDevice->CreateInputLayout(layout,
		numElements,
		buffer,
		fsize,
		VertexLayout);

	delete[] buffer;
}

// ピクセルシェーダ生成
void Renderer::CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName)
{
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_D3DDevice->CreatePixelShader(buffer, fsize, NULL, PixelShader);

	delete[] buffer;
}


