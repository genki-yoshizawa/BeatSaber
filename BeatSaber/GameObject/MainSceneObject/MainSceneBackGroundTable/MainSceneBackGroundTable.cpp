#include "MainSceneBackGroundTable.h"
#include "Scene/Scene.h"
#include "Renderer/Renderer.h"

void MainSceneBackGroundTable::Init() 
{
	// 初期値を設定
	m_Position = XMFLOAT3(0.0f,-30.0f,-10.0f);
	m_Rotation = XMQuaternionIdentity();
	m_Scale = XMFLOAT3(6.0f, fabs(m_Position.y) * 2, 200.0f);

	m_TableModel = Scene::GetTable();

	Renderer::GetInstance()->CreateVertexShader(&m_VertexShader, &m_VertexLayout, "mainSceneBackGroundTableVS.cso");
	Renderer::GetInstance()->CreatePixelShader(&m_PixelShader, "mainSceneBackGroundTablePS.cso");
}

void MainSceneBackGroundTable::Uninit() 
{
}
void MainSceneBackGroundTable::Update() {}

void MainSceneBackGroundTable::Draw() 
{
	// 入力レイアウト設定
	Renderer::GetInstance()->GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	// シェーダ設定
	Renderer::GetInstance()->GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);

	Renderer::GetInstance()->GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);


	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_Rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	m_TableModel->Draw();
}
