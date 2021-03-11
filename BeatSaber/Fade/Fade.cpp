#include "Main/Main.h"
#include "Fade/Fade.h"
#include "GameManager/GameManager.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"
#include "VR/VRHandle/VRHandle.h"
//#include "title.h"

#include <typeinfo>

Fade* Fade::m_Instance = nullptr;

void Fade::Init(void)
{
	//VERTEX_3D vertex[4];
	//
	//vertex[0].Position = D3DXVECTOR3(-Renderer::GetInstance()->GetWinsizeW() * 0.5f, -Renderer::GetInstance()->GetWinsizeH() * 0.5f, -0.02f);
	//vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//vertex[0].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
	//vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	//
	//vertex[1].Position = D3DXVECTOR3(Renderer::GetInstance()->GetWinsizeW() * 0.5f, -Renderer::GetInstance()->GetWinsizeH() * 0.5f, -0.02f);
	//vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//vertex[1].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
	//vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
	//
	//vertex[2].Position = D3DXVECTOR3(-Renderer::GetInstance()->GetWinsizeW() * 0.5f, Renderer::GetInstance()->GetWinsizeH() * 0.5f, -0.02f);
	//vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//vertex[2].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
	//vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
	//
	//vertex[3].Position = D3DXVECTOR3(Renderer::GetInstance()->GetWinsizeW() * 0.5f, Renderer::GetInstance()->GetWinsizeH() * 0.5f, -0.02f);
	//vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//vertex[3].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
	//vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);
	//
	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));
	//bd.Usage = D3D11_USAGE_DYNAMIC;
	//bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//
	//D3D11_SUBRESOURCE_DATA sd;
	//ZeroMemory(&sd, sizeof(sd));
	//sd.pSysMem = vertex;
	//
	//Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	VERTEX_3D vertex[4];

	for (int eye = 0; eye < 2; eye++)
	{
		ovrSession* session = VRHandle::GetInstance()->GetOVRSession();
		ovrSizei idealSize = ovr_GetFovTextureSize(*session, (ovrEyeType)eye, VRHandle::GetInstance()->GetOVRHmd()->DefaultEyeFov[eye], 1.0f);

		vertex[0].Position = D3DXVECTOR3(idealSize.w, 0.0f, 0.0f);
		vertex[0].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
		vertex[0].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

		vertex[1].Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
		vertex[1].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

		vertex[2].Position = D3DXVECTOR3(idealSize.w, idealSize.h, 0.0f);
		vertex[2].Normal = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
		vertex[2].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

		vertex[3].Position = D3DXVECTOR3(0.0f, idealSize.h, 0.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
		vertex[3].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertex;

		Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer[eye]);
	}

	D3DX11CreateShaderResourceViewFromFile(Renderer::GetInstance()->GetDevice(),
		"asset/texture/fade.png",
		NULL,
		NULL,
		&m_Texture,
		NULL);

	assert(m_Texture);
}

void Fade::Uninit(void)
{
	m_Texture->Release();
	m_VertexBuffer[0]->Release();
	m_VertexBuffer[1]->Release();
}

void Fade::Update(void)
{
	if (m_Fade == FADE_STATE_OUT) {
		if (m_FadeAlpha >= 1.0f) {
			m_FadeAlpha = 1.0f;
			m_Fade = FADE_STATE_IN;
			GameManager::GetInstance()->SetScene(m_NextScene);
			m_NextScene = SCENE_NONE;
		}
		m_FadeAlpha += m_AddAlpha;
	}
	else if (m_Fade == FADE_STATE_IN) {
		if (m_FadeAlpha <= 0.0f) {
			m_FadeAlpha = 0.0f;
			m_Fade = FADE_STATE_NONE;
		}
		m_FadeAlpha -= m_AddAlpha;
	}
}

void Fade::Draw(int eye)
{
	if (m_Fade == FADE_STATE_NONE) {
		return;
	}

	m_fadecolor.a = m_FadeAlpha;

	// シェーダー設定
	Renderer::GetInstance()->SetUsualShader();

	//　マトリクス設定
	Renderer::GetInstance()->SetWorldMatrix(&(XMMatrixIdentity()));

	//Renderer::GetInstance()->SetWorldViewProjection2D(VRHandle::GetInstance()->GetProjectionMatrix(eye));
	Renderer::GetInstance()->SetWorldViewProjection2D(eye);

	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetInstance()->GetDeviceContext()->Map(m_VertexBuffer[eye], 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;
	ovrSession* session = VRHandle::GetInstance()->GetOVRSession();
	ovrSizei idealSize = ovr_GetFovTextureSize(*session, (ovrEyeType)eye, VRHandle::GetInstance()->GetOVRHmd()->DefaultEyeFov[eye], 1.0f);

	vertex[0].Position = D3DXVECTOR3(idealSize.w, 0.0f, 0.0f);
	vertex[0].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
	vertex[0].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	vertex[1].Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[1].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
	vertex[1].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

	vertex[2].Position = D3DXVECTOR3(idealSize.w, idealSize.h, 0.0f);
	vertex[2].Normal = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
	vertex[2].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	vertex[3].Position = D3DXVECTOR3(0.0f, idealSize.h, 0.0f);
	vertex[3].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = D3DXVECTOR4(m_fadecolor.r, m_fadecolor.g, m_fadecolor.b, m_fadecolor.a);
	vertex[3].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

	Renderer::GetInstance()->GetDeviceContext()->Unmap(m_VertexBuffer[eye], 0);

	//　頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(
		0, 1, &m_VertexBuffer[eye], &stride, &offset);

	//　マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer::GetInstance()->SetMaterial(material);

	//　テクスチャ設定
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//　プリミティブトポロジ設定
	Renderer::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE map;
	Renderer::GetInstance()->GetDeviceContext()->Map(Renderer::GetInstance()->GetD3DBffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, Renderer::GetInstance()->GetUniformData(), Renderer::GetInstance()->UNIFORM_DATA_SIZE);
	Renderer::GetInstance()->GetDeviceContext()->Unmap(Renderer::GetInstance()->GetD3DBffer(), 0);

	//　ポリゴン描画
	Renderer::GetInstance()->GetDeviceContext()->Draw(4, 0);
}

void Fade::SetFade(SCENE_CATEGORY scene)
{
	m_Fade = FADE_STATE_OUT;

	m_NextScene = scene;
}