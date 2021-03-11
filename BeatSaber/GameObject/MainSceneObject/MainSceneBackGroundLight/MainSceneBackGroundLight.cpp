#include "MainSceneBackGroundLight.h"
#include "Scene/Scene.h"

void MainSceneBackGroundLight::Init()
{
	XMFLOAT3* pos = new XMFLOAT3[100];
	int i = 0;
	for (int z = 0; z < 100; z++)
	{
		pos[z] = XMFLOAT3(0.0f, 0.0f, -5.0f * z);
	}

	// ストラクチャードバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMFLOAT3) * 100;
	bd.StructureByteStride = sizeof(XMFLOAT3);
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pos;

	Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_PositionBuffer);

	delete[] pos;

	// シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_UNKNOWN;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvd.Buffer.FirstElement = 0;
	srvd.Buffer.NumElements = 100;
	Renderer::GetInstance()->GetDevice()->CreateShaderResourceView(m_PositionBuffer, &srvd, &m_PositionSRV);

	for (int i = 0; i < 100; i++) 
	{
		m_isLight[i] = FALSE;
	}

	// ストラクチャードバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BOOL) * 100;
	bd.StructureByteStride = sizeof(BOOL);
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = m_isLight;

	Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_isLightBuffer);

	// シェーダーリソースビュー作成
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_UNKNOWN;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvd.Buffer.FirstElement = 0;
	srvd.Buffer.NumElements = 100;
	Renderer::GetInstance()->GetDevice()->CreateShaderResourceView(m_isLightBuffer, &srvd, &m_isLightSRV);

	m_LightModel = Scene::GetLight();

	Renderer::GetInstance()->CreateVertexShader(&m_VertexShader, &m_VertexLayout, "mainSceneBackGroundLightVS.cso");
	Renderer::GetInstance()->CreatePixelShader(&m_PixelShader, "mainSceneBackGroundLightPS.cso");

	D3DX11CreateShaderResourceViewFromFile(Renderer::GetInstance()->GetDevice(),
		"asset/texture/fade.png",
		NULL,
		NULL,
		&m_BlackTexture,
		NULL);

	assert(m_BlackTexture);

	// 初期値を設定
	m_PositionLeftLight = XMFLOAT3(-3.5f, 2.0f, -5.0f);
	m_PositionRightLight = XMFLOAT3(3.5f, 2.0f, -5.0f);
	m_RotationLeftLight = XMQuaternionIdentity();
	m_RotationRightLight = XMQuaternionIdentity();
	m_Scale = XMFLOAT3(0.2f, 0.05f, 5.0f);

	XMVECTOR rot;
	rot = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XM_PI / 2.0f);
	m_RotationLeftLight = XMQuaternionMultiply(m_RotationLeftLight, rot);
	m_RotationRightLight = XMQuaternionMultiply(m_RotationRightLight, rot);

	XMVECTOR rot_left = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XM_PI / 3.0f);
	m_RotationLeftLight = XMQuaternionMultiply(m_RotationLeftLight, rot_left);

	XMVECTOR rot_right = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), -XM_PI / 3.0f);
	m_RotationRightLight = XMQuaternionMultiply(m_RotationRightLight, rot_right);

	m_FrameCount = 0;
}

void MainSceneBackGroundLight::Uninit() 
{
}
void MainSceneBackGroundLight::Update() 
{
	for (int i = 0; i < 100; i++)
	{
		if (cosf(8 * (-(m_FrameCount / 5) + i) * (XM_PI / 180.0f)) > 0.99f)
		{
			m_isLight[i] = TRUE;
		}
		else
		{
			m_isLight[i] = FALSE;
		}
	}

	Renderer::GetInstance()->GetDeviceContext()->UpdateSubresource(m_isLightBuffer, 0, NULL, m_isLight, 0, 0);
	m_FrameCount++;
}

void MainSceneBackGroundLight::Draw()
{
	// 入力レイアウト設定
	Renderer::GetInstance()->GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	// シェーダ設定
	Renderer::GetInstance()->GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);

	Renderer::GetInstance()->GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//　テクスチャ設定
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, &m_BlackTexture);

	// ストラクチャードバッファ設定
	Renderer::GetInstance()->GetDeviceContext()->VSSetShaderResources(2, 1, &m_PositionSRV);
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(3, 1, &m_isLightSRV);

	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_RotationLeftLight));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_PositionLeftLight)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	m_LightModel->DrawInstanced(100);

	modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_RotationRightLight));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_PositionRightLight)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	m_LightModel->DrawInstanced(100);

	Renderer::GetInstance()->SetUsualShader();
}
