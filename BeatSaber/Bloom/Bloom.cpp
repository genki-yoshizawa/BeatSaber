#include "Main/Main.h"
#include "Bloom.h"
#include "GameManager/GameManager.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"
#include "VR/VRHandle/VRHandle.h"
//#include "title.h"

#include <typeinfo>

Bloom* Bloom::m_Instance = nullptr;

void Bloom::Init(void)
{
	VERTEX_3D vertex[4];
	for (int eye = 0; eye < 2; eye++)
	{
		ovrSession* session = VRHandle::GetInstance()->GetOVRSession();
		ovrSizei idealSize = ovr_GetFovTextureSize(*session, (ovrEyeType)eye, VRHandle::GetInstance()->GetOVRHmd()->DefaultEyeFov[eye], 1.0f);

		vertex[0].Position = D3DXVECTOR3(idealSize.w , 0.0f, 0.0f);
		vertex[0].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

		vertex[1].Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

		vertex[2].Position = D3DXVECTOR3(idealSize.w , idealSize.h , 0.0f);
		vertex[2].Normal = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

		vertex[3].Position = D3DXVECTOR3(0.0f, idealSize.h , 0.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

		// ���_�o�b�t�@����
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * 4;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertex;

		Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer[eye]);
	}
	for (int eye = 0; eye < 2; eye++)
	{// �P�x���o�p
	// �e�N�X�`���[�쐬
		ID3D11Texture2D* luminanceTexture = nullptr;
		ovr_GetTextureSwapChainBufferDX(*VRHandle::GetInstance()->GetOVRSession(), VRHandle::GetInstance()->GetTextureSwapChain(eye), 0, IID_PPV_ARGS(&luminanceTexture));

		//�P�x���o�e�N�X�`���[�쐬
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		luminanceTexture->GetDesc(&td);
		td.BindFlags = td.BindFlags | D3D11_BIND_SHADER_RESOURCE;
		luminanceTexture = nullptr;
		Renderer::GetInstance()->GetDevice()->CreateTexture2D(&td, NULL, &luminanceTexture);

		// �����_�[�^�[�Q�b�g�r���[�쐬
		D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
		memset(&rtvd, 0, sizeof(rtvd));
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R32G32B32A32_FLOAT;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateRenderTargetView(luminanceTexture, &rtvd, &m_LuminanceRenderTargetView[eye]);
		ERROR_CHECK((hr == ERROR_SUCCESS), "VR�f�o�C�X�̃����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s���܂����B");

		// �P�x���o�p�V�F�[�_�[���\�[�X�r���[�쐬
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = rtvd.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		SRVDesc.Texture2D.MipLevels = 1;
		Renderer::GetInstance()->GetDevice()->CreateShaderResourceView(luminanceTexture, &SRVDesc, &m_LuminanceShaderResourceView[eye]);

		luminanceTexture->Release();
	}
	for (int eye = 0; eye < 2; eye++)
	{// �K�E�X�t�B���^�[�pX����
		// �e�N�X�`���[�쐬
		ID3D11Texture2D* filterTexture = nullptr;
		ovr_GetTextureSwapChainBufferDX(*VRHandle::GetInstance()->GetOVRSession(), VRHandle::GetInstance()->GetTextureSwapChain(eye), 0, IID_PPV_ARGS(&filterTexture));

		//�K�E�X�t�B���^�[�e�N�X�`���[�쐬
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		filterTexture->GetDesc(&td);
		td.BindFlags = td.BindFlags | D3D11_BIND_SHADER_RESOURCE;
		filterTexture = nullptr;
		Renderer::GetInstance()->GetDevice()->CreateTexture2D(&td, NULL, &filterTexture);

		// �����_�[�^�[�Q�b�g�r���[�쐬
		D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
		memset(&rtvd, 0, sizeof(rtvd));
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R32G32B32A32_FLOAT;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateRenderTargetView(filterTexture, &rtvd, &m_FilterXRenderTargetView[eye]);
		ERROR_CHECK((hr == ERROR_SUCCESS), "VR�f�o�C�X�̃����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s���܂����B");

		// �K�E�X�t�B���^�[�p�V�F�[�_�[���\�[�X�r���[�쐬
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = rtvd.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		SRVDesc.Texture2D.MipLevels = 1;
		Renderer::GetInstance()->GetDevice()->CreateShaderResourceView(filterTexture, &SRVDesc, &m_FilterXShaderResourceView[eye]);

		filterTexture->Release();
	}
	for (int eye = 0; eye < 2; eye++)
	{// �K�E�X�t�B���^�[�pY����
		// �e�N�X�`���[�쐬
		ID3D11Texture2D* filterTexture = nullptr;
		ovr_GetTextureSwapChainBufferDX(*VRHandle::GetInstance()->GetOVRSession(), VRHandle::GetInstance()->GetTextureSwapChain(eye), 0, IID_PPV_ARGS(&filterTexture));

		//�K�E�X�t�B���^�[�e�N�X�`���[�쐬
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		filterTexture->GetDesc(&td);
		td.BindFlags = td.BindFlags | D3D11_BIND_SHADER_RESOURCE;
		filterTexture = nullptr;
		Renderer::GetInstance()->GetDevice()->CreateTexture2D(&td, NULL, &filterTexture);

		// �����_�[�^�[�Q�b�g�r���[�쐬
		D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
		memset(&rtvd, 0, sizeof(rtvd));
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R32G32B32A32_FLOAT;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateRenderTargetView(filterTexture, &rtvd, &m_FilterYRenderTargetView[eye]);
		ERROR_CHECK((hr == ERROR_SUCCESS), "VR�f�o�C�X�̃����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s���܂����B");

		// �K�E�X�t�B���^�[�p�V�F�[�_�[���\�[�X�r���[�쐬
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = rtvd.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		SRVDesc.Texture2D.MipLevels = 1;
		Renderer::GetInstance()->GetDevice()->CreateShaderResourceView(filterTexture, &SRVDesc, &m_FilterYShaderResourceView[eye]);

		filterTexture->Release();
	}

	Renderer::GetInstance()->CreateVertexShader(&m_VertexShader, &m_VertexLayout, "bloomVS.cso");
	Renderer::GetInstance()->CreatePixelShader(&m_PixelShader, "bloomPS.cso");

	Renderer::GetInstance()->CreateVertexShader(&m_LuminanceVertexShader, &m_LuminanceVertexLayout, "bloomLuminanceVS.cso");
	Renderer::GetInstance()->CreatePixelShader(&m_LuminancePixelShader, "bloomLuminancePS.cso");

	Renderer::GetInstance()->CreateVertexShader(&m_FilterXVertexShader, &m_FilterXVertexLayout, "bloomFilterXVS.cso");
	Renderer::GetInstance()->CreatePixelShader(&m_FilterXPixelShader, "bloomFilterXPS.cso");

	Renderer::GetInstance()->CreateVertexShader(&m_FilterYVertexShader, &m_FilterYVertexLayout, "bloomFilterYVS.cso");
	Renderer::GetInstance()->CreatePixelShader(&m_FilterYPixelShader, "bloomFilterYPS.cso");

	//D3DX11CreateShaderResourceViewFromFile(Renderer::GetInstance()->GetDevice(),
	//	"asset/texture/number.png",
	//	NULL,
	//	NULL,
	//	&m_BloomTexture,
	//	NULL);

	//assert(m_BloomTexture);
}

void Bloom::Uninit(void)
{
	for(int eye = 0; eye < 2; eye++)
	{
		m_LuminanceRenderTargetView[eye]->Release();
		//m_LuminanceShaderResourceView[eye]->Release();
		m_FilterXRenderTargetView[eye]->Release();
		//m_FilterXShaderResourceView[eye]->Release(); 
		m_FilterYRenderTargetView[eye]->Release();
		//m_FilterYShaderResourceView[eye]->Release(); 
		m_VertexBuffer[eye]->Release();
	}
}

void Bloom::Update(void)
{
}

void Bloom::Draw(int eye)
{
	// ���̓��C�A�E�g�ݒ�
	Renderer::GetInstance()->GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	// �V�F�[�_�ݒ�
	Renderer::GetInstance()->GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetInstance()->GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);


	//�@�}�g���N�X�ݒ�
	Renderer::GetInstance()->SetWorldMatrix(&(XMMatrixIdentity()));
	//Renderer::GetInstance()->SetWorldViewProjection2D(VRHandle::GetInstance()->GetProjectionMatrix(eye));
	Renderer::GetInstance()->SetWorldViewProjection2D(eye);

	//�@���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(
		0, 1, &m_VertexBuffer[eye], &stride, &offset);

	//�@�}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer::GetInstance()->SetMaterial(material);

	//�@�e�N�X�`���ݒ�
	//m_BloomTexture = Renderer::GetInstance()->GetPPTexture(eye);
	ID3D11ShaderResourceView* bloom_texture = VRHandle::GetInstance()->GetPPTexture(eye);
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &bloom_texture);
	//Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, &m_LuminanceShaderResourceView[eye]);
	//Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, &m_FilterXShaderResourceView[eye]);
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, &m_FilterYShaderResourceView[eye]);
	
	//�@�v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE map;
	Renderer::GetInstance()->GetDeviceContext()->Map(Renderer::GetInstance()->GetD3DBffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, Renderer::GetInstance()->GetUniformData(), Renderer::GetInstance()->UNIFORM_DATA_SIZE);
	Renderer::GetInstance()->GetDeviceContext()->Unmap(Renderer::GetInstance()->GetD3DBffer(), 0);

	//�@�|���S���`��
	Renderer::GetInstance()->GetDeviceContext()->Draw(4, 0);
}

void Bloom::Luminance(int eye)
{
	// �����_�[�^�[�Q�b�g�ݒ�
	ID3D11DepthStencilView* depth_stencil = VRHandle::GetInstance()->GetDSV(eye);
	Renderer::GetInstance()->GetDeviceContext()->OMSetRenderTargets(1, &m_LuminanceRenderTargetView[eye], depth_stencil);

	// �o�b�N�o�b�t�@�N���A
	float ClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	Renderer::GetInstance()->GetDeviceContext()->ClearRenderTargetView(m_LuminanceRenderTargetView[eye], ClearColor);
	Renderer::GetInstance()->GetDeviceContext()->ClearDepthStencilView(depth_stencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Renderer::GetInstance()->GetDeviceContext()->RSSetViewports(1, &VRHandle::GetInstance()->GetViewPort(eye));

	// ���̓��C�A�E�g�ݒ�
	Renderer::GetInstance()->GetDeviceContext()->IASetInputLayout(m_LuminanceVertexLayout);

	// �V�F�[�_�ݒ�
	Renderer::GetInstance()->GetDeviceContext()->VSSetShader(m_LuminanceVertexShader, NULL, 0);
	Renderer::GetInstance()->GetDeviceContext()->PSSetShader(m_LuminancePixelShader, NULL, 0);

	//�@�}�g���N�X�ݒ�
	Renderer::GetInstance()->SetWorldMatrix(&(XMMatrixIdentity()));
	Renderer::GetInstance()->SetWorldViewProjection2D(eye);

	//�@���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(
		0, 1, &m_VertexBuffer[eye], &stride, &offset);

	//�@�}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer::GetInstance()->SetMaterial(material);

	//�@�e�N�X�`���ݒ�
	ID3D11ShaderResourceView*	postprocessing_texture = VRHandle::GetInstance()->GetPPTexture(eye);
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &postprocessing_texture);

	//�@�v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE map;
	Renderer::GetInstance()->GetDeviceContext()->Map(Renderer::GetInstance()->GetD3DBffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, Renderer::GetInstance()->GetUniformData(), Renderer::GetInstance()->UNIFORM_DATA_SIZE);
	Renderer::GetInstance()->GetDeviceContext()->Unmap(Renderer::GetInstance()->GetD3DBffer(), 0);

	//�@�|���S���`��
	Renderer::GetInstance()->GetDeviceContext()->Draw(4, 0);
}

void Bloom::Filter(int eye)
{
	Renderer::GetInstance()->SetGaussWeight();

	ovrSession* session = VRHandle::GetInstance()->GetOVRSession();
	ovrSizei idealSize = ovr_GetFovTextureSize(*session, (ovrEyeType)eye, VRHandle::GetInstance()->GetOVRHmd()->DefaultEyeFov[eye], 1.0f);
	Renderer::GetInstance()->SetScreen(idealSize.w, idealSize.h);

	{// X����
	// �����_�[�^�[�Q�b�g�ݒ�
		ID3D11DepthStencilView* depth_stencil = VRHandle::GetInstance()->GetDSV(eye);
		Renderer::GetInstance()->GetDeviceContext()->OMSetRenderTargets(1, &m_FilterXRenderTargetView[eye], depth_stencil);

		// �o�b�N�o�b�t�@�N���A
		float ClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
		Renderer::GetInstance()->GetDeviceContext()->ClearRenderTargetView(m_FilterXRenderTargetView[eye], ClearColor);
		Renderer::GetInstance()->GetDeviceContext()->ClearDepthStencilView(depth_stencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Renderer::GetInstance()->GetDeviceContext()->RSSetViewports(1, &VRHandle::GetInstance()->GetViewPort(eye));

		// ���̓��C�A�E�g�ݒ�
		Renderer::GetInstance()->GetDeviceContext()->IASetInputLayout(m_FilterXVertexLayout);

		// �V�F�[�_�ݒ�
		Renderer::GetInstance()->GetDeviceContext()->VSSetShader(m_FilterXVertexShader, NULL, 0);
		Renderer::GetInstance()->GetDeviceContext()->PSSetShader(m_FilterXPixelShader, NULL, 0);
		//�@�}�g���N�X�ݒ�
		Renderer::GetInstance()->SetWorldMatrix(&(XMMatrixIdentity()));
		Renderer::GetInstance()->SetWorldViewProjection2D(eye);

		//�@���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(
			0, 1, &m_VertexBuffer[eye], &stride, &offset);

		//�@�}�e���A���ݒ�
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		Renderer::GetInstance()->SetMaterial(material);

		//�@�e�N�X�`���ݒ�
		Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &m_LuminanceShaderResourceView[eye]);

		//�@�v���~�e�B�u�g�|���W�ݒ�
		Renderer::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		D3D11_MAPPED_SUBRESOURCE map;
		Renderer::GetInstance()->GetDeviceContext()->Map(Renderer::GetInstance()->GetD3DBffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		memcpy(map.pData, Renderer::GetInstance()->GetUniformData(), Renderer::GetInstance()->UNIFORM_DATA_SIZE);
		Renderer::GetInstance()->GetDeviceContext()->Unmap(Renderer::GetInstance()->GetD3DBffer(), 0);

		//�@�|���S���`��
		Renderer::GetInstance()->GetDeviceContext()->Draw(4, 0);
	}
	{// Y����
		// �����_�[�^�[�Q�b�g�ݒ�
		ID3D11DepthStencilView* depth_stencil = VRHandle::GetInstance()->GetDSV(eye);
		Renderer::GetInstance()->GetDeviceContext()->OMSetRenderTargets(1, &m_FilterYRenderTargetView[eye], depth_stencil);

		// �o�b�N�o�b�t�@�N���A
		float ClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
		Renderer::GetInstance()->GetDeviceContext()->ClearRenderTargetView(m_FilterYRenderTargetView[eye], ClearColor);
		Renderer::GetInstance()->GetDeviceContext()->ClearDepthStencilView(depth_stencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Renderer::GetInstance()->GetDeviceContext()->RSSetViewports(1, &VRHandle::GetInstance()->GetViewPort(eye));

		// ���̓��C�A�E�g�ݒ�
		Renderer::GetInstance()->GetDeviceContext()->IASetInputLayout(m_FilterYVertexLayout);

		// �V�F�[�_�ݒ�
		Renderer::GetInstance()->GetDeviceContext()->VSSetShader(m_FilterYVertexShader, NULL, 0);
		Renderer::GetInstance()->GetDeviceContext()->PSSetShader(m_FilterYPixelShader, NULL, 0);
		//�@�}�g���N�X�ݒ�
		Renderer::GetInstance()->SetWorldMatrix(&(XMMatrixIdentity()));
		Renderer::GetInstance()->SetWorldViewProjection2D(eye);

		//�@���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(
			0, 1, &m_VertexBuffer[eye], &stride, &offset);

		//�@�}�e���A���ݒ�
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		Renderer::GetInstance()->SetMaterial(material);

		//�@�e�N�X�`���ݒ�
		Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &m_FilterXShaderResourceView[eye]);

		//�@�v���~�e�B�u�g�|���W�ݒ�
		Renderer::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		D3D11_MAPPED_SUBRESOURCE map;
		Renderer::GetInstance()->GetDeviceContext()->Map(Renderer::GetInstance()->GetD3DBffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		memcpy(map.pData, Renderer::GetInstance()->GetUniformData(), Renderer::GetInstance()->UNIFORM_DATA_SIZE);
		Renderer::GetInstance()->GetDeviceContext()->Unmap(Renderer::GetInstance()->GetD3DBffer(), 0);

		//�@�|���S���`��
		Renderer::GetInstance()->GetDeviceContext()->Draw(4, 0);
	}
}