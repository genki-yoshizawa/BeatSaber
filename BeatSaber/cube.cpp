#include "Main/Main.h"
#include "VR/VRCamera/VRCamera.h"
#include "VR/VRHandle/VRHandle.h"
#include "model.h"
#include "cube.h"

void CCube::Init()
{
	m_Model = new CModel();
	m_Model->Load("asset\\model\\xyz.obj");

	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);

	m_Quaternion = XMQuaternionIdentity();
}


void CCube::Uninit()
{
	m_Model->Unload();
	delete m_Model;
}

void CCube::Update()
{

}

void CCube::Draw()
{
	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_Quaternion));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);


	m_Model->Draw();
}

