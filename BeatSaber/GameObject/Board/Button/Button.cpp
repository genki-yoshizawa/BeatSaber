#include "Button.h"
#include "GameManager/GameManager.h"
#include "Fade/Fade.h"
#include "CreateMusicScore/CreateMusicScore.h"
#include "GameAudio/GameAudio.h"

ID3D11Buffer* Button::m_VertexBuffer = nullptr;

void Button::Init(BUTTON_ACTION action, XMFLOAT2 position, float width, float height, const char* texture, const char* texture_ray_hit)
{
	if (!m_VertexBuffer)
	{
		VERTEX_3D vertex[4];

		vertex[0].Position = D3DXVECTOR3(0.5f, 0.5f, 0.0f);
		vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

		vertex[1].Position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

		vertex[2].Position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
		vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

		vertex[3].Position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * 4;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertex;

		Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}

	D3DX11CreateShaderResourceViewFromFile(Renderer::GetInstance()->GetDevice(),
		texture,
		NULL,
		NULL,
		&m_Texture,
		NULL);

	assert(m_Texture);

	D3DX11CreateShaderResourceViewFromFile(Renderer::GetInstance()->GetDevice(),
		texture_ray_hit,
		NULL,
		NULL,
		&m_HandRayHitTexture,
		NULL);

	assert(m_HandRayHitTexture);

	m_Position = position;
	m_Width = width;
	m_Height = height;

	// actionによってButtonActionの振る舞いを変える
	// 関数ポインタを用いてButtonActionの関数を指定する
	switch(action)
	{
	case ACTION_TEST:
		ButtonAction = ActionTest;
		break;

	case ACTION_SCENE_CHANGE_TO_MAIN:
		ButtonAction = ActionSceneChangeToMain;
		break;

	case ACTION_SCENE_CHANGE_TO_TITLE:
		ButtonAction = ActionSceneChangeToTitle;
		break;

	default:
		ButtonAction = nullptr;
		break;
	}
}

void Button::Uninit()
{
	if (!m_HandRayHitTexture)
	{
		m_HandRayHitTexture->Release();
		m_HandRayHitTexture = nullptr;
	}
	if (!m_Texture)
	{
		m_Texture->Release();
		m_Texture = nullptr;
	}
	if (!m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = nullptr;
	}

}

void Button::Update()
{}

void Button::Draw(XMFLOAT3 board_pos, XMVECTOR board_rotation)
{
	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&XMFLOAT3(m_Width, m_Height, 1.0f))), XMMatrixRotationQuaternion(board_rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&XMFLOAT3(board_pos.x + m_Position.x, board_pos.y + m_Position.y, board_pos.z))));

	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	//　頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(
		0, 1, &m_VertexBuffer, &stride, &offset);

	//　マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer::GetInstance()->SetMaterial(material);

	//　テクスチャ設定
	if (m_isHandRayHit)
	{
		Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &m_HandRayHitTexture);
	}
	else
	{
		Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
	}

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

void Button::WhenTriggerInput()
{
	if(!m_isHandRayHit)
	{
		return;
	}

	if (ButtonAction)
	{
		ButtonAction(this);
	}
}

bool Button::isCollisionHandRay(XMFLOAT2 ray_hit_position)
{
	m_isHandRayHit = false;

	if(ray_hit_position.x > m_Position.x - m_Width * 0.5f && ray_hit_position.x < m_Position.x + m_Width * 0.5f
		&& ray_hit_position.y > m_Position.y - m_Height * 0.5f && ray_hit_position.y < m_Position.y + m_Height * 0.5f)
	{
		// 自身が存在する範囲にあった
		m_isHandRayHit = true;
	}

	return m_isHandRayHit;
}


void Button::ActionTest(Button* const this_button)
{
	this_button->m_Width += 0.5f;
	this_button->m_Height += 0.5f;
}

void Button::ActionSceneChangeToMain(Button* const this_button)
{
	Fade::GetInstance()->SetFade(SCENE_MAIN);
	CreateMusicScore::GetInstance()->SetMusicScore(SOUND_LABEL_BGM_BGM1);
	CreateMusicScore::GetInstance()->SetDifficulty("Easy");
	GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_SELECT);
}

void Button::ActionSceneChangeToTitle(Button* const this_button)
{
	Fade::GetInstance()->SetFade(SCENE_TITLE);
	GameAudio::GetInstance()->PlaySound(SOUND_LABEL_SE_SELECT);
}