#include "MainSceneLife.h"
#include "Renderer/Renderer.h"

void MainSceneLife::Init()
{
	VERTEX_3D vertex[4];

	vertex[0].Position = D3DXVECTOR3(1.0f, 0.5f, 0.0f);
	vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

	vertex[1].Position = D3DXVECTOR3(0.0f, 0.5f, 0.0f);
	vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	vertex[2].Position = D3DXVECTOR3(1.0f, -0.5f, 0.0f);
	vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

	vertex[3].Position = D3DXVECTOR3(0.0f, -0.5f, 0.0f);
	vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

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


	D3DX11CreateShaderResourceViewFromFile(Renderer::GetInstance()->GetDevice(),
		"asset/texture/life_gage.png",
		NULL,
		NULL,
		&m_Texture,
		NULL);

	assert(m_Texture);

	// 変数の初期値を決定
	m_Position = XMFLOAT3(POSITION.x, POSITION.y, POSITION.z);
	m_Rotation = XMQuaternionIdentity();

	m_Life = START_LIFE;

	m_Scale = XMFLOAT3(m_Life * 0.05f, 0.1f, 1.0f);
}

void MainSceneLife::Uninit()
{
	m_Texture->Release();
	m_VertexBuffer->Release();
}

void MainSceneLife::Draw() 
{
	m_Scale.x = m_Life * 0.05f;

	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_Rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));

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
				  
void MainSceneLife::AddLife(int add)
{
	if(add < 0)
	{
		return;
	}

	m_Life += add;
	if(m_Life > MAX_LIFE)
	{
		m_Life = MAX_LIFE;
	}

	return;
}

void MainSceneLife::DecreaseLife(int decrease)
{
	if(decrease < 0)
	{
		return;
	}

	m_Life -= decrease;
	if(m_Life < 0)
	{
		m_Life = 0;
	}

	return;
}