#include "MainSceneHandEffect.h"
#include "Renderer/Renderer.h"
#include "VR/VRHandle/VRHandle.h"
#include "VR/VRCamera/VRCamera.h"
#include "GameManager/GameManager.h"

void MainSceneHandEffect::Init(bool is_right, float length)
{
	// ��̈ʒu�Ɗp�x���擾
	ovrPoseStatef hand_state;
	if (is_right)
	{
		hand_state = VRHandle::GetInstance()->GetTrackingState()->HandPoses[RIGHT];
	}
	else
	{
		hand_state = VRHandle::GetInstance()->GetTrackingState()->HandPoses[LEFT];
	}
	XMFLOAT3 position = XMFLOAT3(hand_state.ThePose.Position.x, hand_state.ThePose.Position.y, hand_state.ThePose.Position.z);
	XMVECTOR rotation = XMVectorSet(hand_state.ThePose.Orientation.x, hand_state.ThePose.Orientation.y, hand_state.ThePose.Orientation.z, hand_state.ThePose.Orientation.w);

	// ��̉�]�ʂ���������o�����������߂�
	XMFLOAT3 forward = XMFLOAT3(0.0f, 0.0f, -1.0f);	//�f�t�H���g�̕���
	XMVECTOR vec = XMVector3Rotate(XMLoadFloat3(&forward), rotation);	//XMVECTOR�Ōv�Z
	forward = XMFLOAT3(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec));	//XMFLOAT3�ɕϊ�
	forward = XMFloat3Normalize(forward);	//�P�ʃx�N�g���ɂ���

	{// ���_�o�b�t�@
		for (int i = 0; i < EFFECT_POLYGON_NUM + 1; i++)
		{
			m_PointLog[2 * i] = position;
			m_PointLog[2 * i + 1] = XMFloat3Add(position, XMFloat3Dot(forward, length));
		}

		VERTEX_3D vertex[2 * EFFECT_POLYGON_NUM + 2];

		for (int i = 0; i < EFFECT_POLYGON_NUM + 1; i++)
		{
			vertex[2 * i].Position = D3DXVECTOR3(m_PointLog[2 * i].x, m_PointLog[2 * i].y, m_PointLog[2 * i].z);
			vertex[2 * i].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			vertex[2 * i].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
			vertex[2 * i].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

			vertex[2 * i + 1].Position = D3DXVECTOR3(m_PointLog[2 * i + 1].x, m_PointLog[2 * i + 1].y, m_PointLog[2 * i + 1].z);
			vertex[2 * i + 1].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			vertex[2 * i + 1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f - (i / (float)(EFFECT_POLYGON_NUM + 1)));
			vertex[2 * i + 1].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
		}
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D)  * (2 * EFFECT_POLYGON_NUM + 2);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertex;

		Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}

	{// �C���f�b�N�X�o�b�t�@
		unsigned int index[2 * EFFECT_POLYGON_NUM + 2];
		for (int i = 0; i < 2 * EFFECT_POLYGON_NUM + 2; i++)
		{
			index[i] = 0;
		}
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned int) *(2 * EFFECT_POLYGON_NUM + 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}

	D3DX11CreateShaderResourceViewFromFile(Renderer::GetInstance()->GetDevice(),
		"asset/texture/life_gage.png",
		NULL,
		NULL,
		&m_Texture,
		NULL);

	assert(m_Texture);
}

void MainSceneHandEffect::Uninit()
{
	m_Texture->Release();
	m_IndexBuffer->Release();
	m_VertexBuffer->Release();
}

void MainSceneHandEffect::Update(XMFLOAT3 position, XMFLOAT3 forward, float length)
{
	for (int i = EFFECT_POLYGON_NUM ; i > 0 ; i--)
	{
		m_PointLog[2 * i] = m_PointLog[2 * i - 2];
		m_PointLog[2 * i + 1] = m_PointLog[2 * i - 1];
	}

	m_PointLog[0] = position;
	m_PointLog[1] = XMFloat3Add(position, XMFloat3Dot(forward, length));

	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetInstance()->GetDeviceContext()->Map(m_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// ���̈ʒu���擾
	ovrPosef head_pose = VRHandle::GetInstance()->GetTrackingState()->HeadPose.ThePose;
	//�|�[�Y����XMVECTOR�ϐ��ɃZ�b�g
	XMVECTOR head = XMVectorSet(head_pose.Position.x, head_pose.Position.y, head_pose.Position.z, 0);

	// ���C���J�����̈ʒu�Ɠ��̈ʒu�����Z
	XMVECTOR combined_pos = XMVectorAdd(GetCamera()->GetPosition(), XMVector3Rotate(head, GetCamera()->GetRotation()));

	// XMFLOAT3�ɕϊ�
	XMFLOAT3 camera_position = XMFLOAT3(XMVectorGetX(combined_pos), XMVectorGetY(combined_pos), XMVectorGetZ(combined_pos));

	int *index = (int*)msr.pData;

	// ��O�̓��ό���
	float cur_dot = 0;
	// �؂�Ԃ��J�E���^
	int return_count = 0;

	// ���̖ʂɑΉ�����ӂ��Ȃ��Ȃ邽��+1���Ȃ�
	for (int i = 0; i < EFFECT_POLYGON_NUM; i++)
	{
		// �O�ς���ʂ̖@�������߂�
		XMFLOAT3 vec1 = XMFloat3Suba(m_PointLog[2 * (i - return_count) + 1], m_PointLog[2 * (i - return_count)]);
		XMFLOAT3 vec2 = XMFloat3Suba(m_PointLog[2 * (i - return_count) + 3], m_PointLog[2 * (i - return_count)]);
		XMFLOAT3 cross = XMFloat3Cross(vec1, vec2);

		// �J���������̃x�N�g���Ɩ@���̓��ς̕����𒲂ׂ邱�Ƃɂ��\���ǂ���̖ʂɕ`�悷�邩���߂�
		XMFLOAT3 vec3 = XMFloat3Suba(camera_position, m_PointLog[2 * i]);
		float dot = XMFloat3Dot(cross, vec3);

		if (dot * cur_dot < 0.0f)
		{// �؂�Ԃ����N������
			return_count++;

			if (dot > 0.0f)
			{// ��O�̓��ς͕�
				index[2 * i] = 2 * (i - return_count) + 1;
				index[2 * i + 1] = 2 * (i - return_count);
			}
			else
			{// ��O�̓��ς͐�
				index[2 * i] = 2 * ((i - return_count));
				index[2 * i + 1] = 2 * i + ((i - return_count));
			}

			if (i++ >= EFFECT_POLYGON_NUM)
			{
				cur_dot = dot;
				break;
			}
		}

		if(dot > 0.0f)
		{// ���_�ԍ� = �C���f�b�N�X�ԍ�
			index[2 * i] = 2 * (i - return_count);
			index[2 * i + 1] = 2 * (i - return_count) + 1;
		}
		else
		{// ���_�ԍ��ƃC���f�b�N�X�ԍ��̏��Ԃ����]
			index[2 * i] = 2 * (i - return_count) + 1;
			index[2 * i + 1] = 2 * (i - return_count);
		}

		cur_dot = dot;
	}

	// �Ō�̖ʂɑΉ�����Ō�̕�
	if(cur_dot > 0.0f)
	{
		index[2 * EFFECT_POLYGON_NUM] = 2 * (EFFECT_POLYGON_NUM - return_count);
		index[2 * EFFECT_POLYGON_NUM + 1] = 2 * (EFFECT_POLYGON_NUM - return_count) + 1;
	}
	else
	{
		index[2 * EFFECT_POLYGON_NUM] = 2 * (EFFECT_POLYGON_NUM - return_count) + 1;
		index[2 * EFFECT_POLYGON_NUM + 1] = 2 * (EFFECT_POLYGON_NUM - return_count);
	}

	Renderer::GetInstance()->GetDeviceContext()->Unmap(m_IndexBuffer, 0);
}

void MainSceneHandEffect::Draw()
{
	//LIGHT light;
	//light.Enable = false;
	//light.Direction = D3DXVECTOR4(1.0f, -1.0f, 1.0f, 0.0f);
	//D3DXVec4Normalize(&light.Direction, &light.Direction);
	//light.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	//light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//Renderer::GetInstance()->SetLight(light);

	//�@�}�g���N�X�ݒ�
	Renderer::GetInstance()->SetWorldMatrix(&(XMMatrixIdentity()));

	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetInstance()->GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;

	for(int i = 0; i < EFFECT_POLYGON_NUM + 1; i++)
	{
		vertex[2 * i].Position = D3DXVECTOR3(m_PointLog[2 * i].x, m_PointLog[2 * i].y, m_PointLog[2 * i].z);
		vertex[2 * i].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vertex[2 * i].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
		vertex[2 * i].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

		vertex[2 * i + 1].Position = D3DXVECTOR3(m_PointLog[2 * i + 1].x, m_PointLog[2 * i + 1].y, m_PointLog[2 * i + 1].z);
		vertex[2 * i + 1].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vertex[2 * i + 1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f - (i / (float)(EFFECT_POLYGON_NUM + 1)));
		vertex[2 * i + 1].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	}

	Renderer::GetInstance()->GetDeviceContext()->Unmap(m_VertexBuffer, 0);

	//�@���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(
		0, 1, &m_VertexBuffer, &stride, &offset);

	//�@�C���f�b�N�X�o�b�t�@�ݒ�
	Renderer::GetInstance()->GetDeviceContext()->IASetIndexBuffer(
		m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//�@�}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer::GetInstance()->SetMaterial(material);

	//�@�e�N�X�`���ݒ�
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//�@�v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE map;
	Renderer::GetInstance()->GetDeviceContext()->Map(Renderer::GetInstance()->GetD3DBffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, Renderer::GetInstance()->GetUniformData(), Renderer::GetInstance()->UNIFORM_DATA_SIZE);
	Renderer::GetInstance()->GetDeviceContext()->Unmap(Renderer::GetInstance()->GetD3DBffer(), 0);

	//�@�|���S���`��
	Renderer::GetInstance()->GetDeviceContext()->DrawIndexed(2 * EFFECT_POLYGON_NUM + 2, 0, 0);
}