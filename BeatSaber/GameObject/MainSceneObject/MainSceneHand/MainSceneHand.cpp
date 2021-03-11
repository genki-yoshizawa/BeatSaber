#include "MainSceneHand.h"
#include "GameManager/GameManager.h"
#include "VR/VRHandle/VRHandle.h"

MainSceneHand::MainSceneHand(bool isright_hand)
	:m_isRightHand(isright_hand)
{
}

void MainSceneHand::Init()
{
	m_GripModel = Scene::GetGripModel(m_isRightHand);

	m_SaberModel = new CModel();
	if (m_isRightHand)
	{
		m_SaberModel->Load("asset\\model\\saber_blue.obj");
	}
	else
	{
		m_SaberModel->Load("asset\\model\\saber_red.obj");
	}
	m_Scale = XMFLOAT3(0.03f, 0.03f, 0.03f);
	m_SaberLength = 1.0f;

	m_Effect = new MainSceneHandEffect();
	m_Effect->Init(m_isRightHand, m_SaberLength);
}

void MainSceneHand::Uninit()
{
	//m_Effect.Uninit();

	m_SaberModel->Unload();
	delete m_SaberModel;
}

void MainSceneHand::Update()
{
	// ��̈ʒu�Ɗp�x���擾
	ovrPoseStatef hand_state;
	if (m_isRightHand)
	{
		hand_state = VRHandle::GetInstance()->GetTrackingState()->HandPoses[RIGHT];
	}
	else
	{
		hand_state = VRHandle::GetInstance()->GetTrackingState()->HandPoses[LEFT];
	}

	XMFLOAT3 cur_position = m_Position;
	XMFLOAT3 cur_forward = m_Forward;

	m_Position = XMFLOAT3(hand_state.ThePose.Position.x, hand_state.ThePose.Position.y, hand_state.ThePose.Position.z);
	m_Rotation = XMVectorSet(hand_state.ThePose.Orientation.x, hand_state.ThePose.Orientation.y, hand_state.ThePose.Orientation.z, hand_state.ThePose.Orientation.w);

	// ��̉�]�ʂ���������o�����������߂�
	m_Forward = XMFLOAT3(0.0f, 0.0f, -1.0f);	//�f�t�H���g�̕���
	XMVECTOR vec = XMVector3Rotate(XMLoadFloat3(&m_Forward), m_Rotation);	//XMVECTOR�Ōv�Z
	m_Forward = XMFLOAT3(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec));	//XMFLOAT3�ɕϊ�
	m_Forward = XMFloat3Normalize(m_Forward);	//�P�ʃx�N�g���ɂ���

	if(isHitNotes(cur_position, cur_forward))
	{

	}

	m_Effect->Update(m_Position, m_Forward, m_SaberLength);
}

void MainSceneHand::Draw()
{
	// �O���b�v���f���̕`��
	XMMATRIX modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale)), XMMatrixRotationQuaternion(m_Rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	m_GripModel->Draw();

	// �����f���̕`��
	XMFLOAT3 scale = { m_Scale.x, m_Scale.y, m_SaberLength};
	modelMat = XMMatrixMultiply(XMMatrixScalingFromVector(XMLoadFloat3(&scale)), XMMatrixRotationQuaternion(m_Rotation));
	modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position)));
	Renderer::GetInstance()->SetWorldMatrix(&modelMat);

	m_SaberModel->Draw();

	//m_Effect.Draw();
}

bool MainSceneHand::isHitNotes(XMFLOAT3 cur_position, XMFLOAT3 cur_forward)
{
	// �ʂ�12�{�̐����Ɣ�����s�������
	// ������AABB�̓����蔻����s��

	bool hit = false;

	// �V�[�����ɂ���m�[�c�̎擾
	std::list<MainSceneNotes*> notes = MainScene::GetInstance()->GetNotesObject();

	for (auto note : notes)
	{
		// ��O�̌��̏������ɍ�����ʂ�12�{�̐����Ƃ̏Փˌ��o
		XMFLOAT3 note_pos = note->GetPosition();
		XMFLOAT3 note_scale = note->GetScale();

		// �����͈̂̔͂����߂�
		float x_max = note_pos.x + note_scale.x * 0.5f;
		float x_min = note_pos.x - note_scale.x * 0.5f;
		float y_max = note_pos.y + note_scale.y * 0.5f;
		float y_min = note_pos.y - note_scale.y * 0.5f;
		float z_max = note_pos.z + note_scale.z * 0.5f;
		float z_min = note_pos.z - note_scale.z * 0.5f;

		// abcd-efgh�̗����̂̒��_�����
		XMFLOAT3 point[8];

		point[0] = { x_min,y_max,z_max };	//a
		point[1] = { x_max,y_max,z_max };	//b
		point[2] = { x_max,y_max,z_min };	//c
		point[3] = { x_min,y_max,z_min };	//d
		point[4] = { x_min,y_min,z_max };	//e
		point[5] = { x_max,y_min,z_max };	//f
		point[6] = { x_max,y_min,z_min };	//g
		point[7] = { x_min,y_min,z_min };	//h

		// 12�{�̐���(�x�N�g��)�����
		XMFLOAT3 vec[12];

		vec[0] = XMFloat3Suba(point[1], point[0]);	//ab
		vec[1] = XMFloat3Suba(point[2], point[1]);	//bc
		vec[2] = XMFloat3Suba(point[3], point[2]);	//cd
		vec[3] = XMFloat3Suba(point[0], point[3]);	//da
		vec[4] = XMFloat3Suba(point[5], point[4]);	//ef
		vec[5] = XMFloat3Suba(point[6], point[5]);	//fg
		vec[6] = XMFloat3Suba(point[7], point[6]);	//gh
		vec[7] = XMFloat3Suba(point[4], point[7]);	//he
		vec[8] = XMFloat3Suba(point[4], point[0]);	//ae
		vec[9] = XMFloat3Suba(point[5], point[1]);	//bf
		vec[10] = XMFloat3Suba(point[6], point[2]);	//cg
		vec[11] = XMFloat3Suba(point[7], point[3]);	//dh

		// �ʂ̖@���x�N�g�������߂�
		XMFLOAT3 normal = XMFloat3Cross(cur_forward, m_Forward);
		normal = XMFloat3Normalize(normal);

		// ���ꂼ��̐����Ō��؂���
		for (int i = 0; i < 12; i++)
		{
			// ���ʂƒ����̏Փ˔���
			if (XMFloat3Dot(vec[i], normal) == 0)
			{// �@���Ƃ̓��ς�0�Ȃ畽�s
				continue;
			}

			// ���ʂƐ����̏Փ˔���
			// �����̗��[�_p1,p2
			XMFLOAT3 p1 = point[i % 8];		//abcd efgh��abcd
			XMFLOAT3 p2 = XMFloat3Add(p1, vec[i]);
			// m_Position�ƒ[�_�̃x�N�g�������߂�
			XMFLOAT3 v1 = XMFloat3Suba(p1, m_Position);
			XMFLOAT3 v2 = XMFloat3Suba(p2, m_Position);

			if (XMFloat3Dot(v1, normal) * XMFloat3Dot(v2, normal) > 0)
			{//���ʏ�̓_��p1,p2�̂��ꂼ��̃x�N�g���Ɩ@���̓��ς��������Ȃ�ΏՓ˂��Ă��Ȃ�
				continue;
			}

			// �����Ɣ|���S���̏Փ˔���
			// ���ʂƂ̏Փ˓_�����߂�
			// �����̒[�_�ƕ��ʂ̋������Z�o
			float d1 = fabs(XMFloat3Dot(v1, normal));
			float d2 = fabs(XMFloat3Dot(v2, normal));

			float t = d1 / (d1 + d2);	//�ꎞ�ϐ�t
			XMFLOAT3 v3 = XMFloat3Add(XMFloat3Dot(v1, 1 - t), XMFloat3Dot(v2, t));
			v3 = XMFloat3Add(m_Position, v3);

			// �Փ˓_���|���S���ɂ��邩���ׂ�
			// �|���S����4���_
			XMFLOAT3 vertex1 = cur_position;
			XMFLOAT3 vertex2 = XMFloat3Add(cur_position, cur_forward);
			XMFLOAT3 vertex3 = XMFloat3Add(m_Position, m_Forward);
			XMFLOAT3 vertex4 = m_Position;

			// �e���_�ƏՓ˓_�̃x�N�g��
			XMFLOAT3 vec1 = XMFloat3Suba(v3, vertex1);
			XMFLOAT3 vec2 = XMFloat3Suba(v3, vertex2);
			XMFLOAT3 vec3 = XMFloat3Suba(v3, vertex3);
			XMFLOAT3 vec4 = XMFloat3Suba(v3, vertex4);

			// 1���ڂ̎O�p�|���S��
			if (isOnPolygon(vertex1, vertex2, vertex3, vec1) &&
				isOnPolygon(vertex2, vertex3, vertex1, vec2) &&
				isOnPolygon(vertex3, vertex1, vertex2, vec3))
			{//���̎O�p�|���S���ɏՓ˓_��������
				note->SetHitSaber(m_isRightHand);
				hit = true;
				break;
			}

			// 2���ڂ̎O�p�|���S��
			if (isOnPolygon(vertex1, vertex3, vertex4, vec1) &&
				isOnPolygon(vertex3, vertex4, vertex1, vec3) &&
				isOnPolygon(vertex4, vertex1, vertex3, vec4))
			{//���̎O�p�|���S���ɏՓ˓_��������
				note->SetHitSaber(m_isRightHand);
				hit = true;
				break;
			}
		}

		if (hit)
		{
			return true;
		}

		// ������AABB�̓����蔻����s��
		XMFLOAT3 saber_end_point = XMFloat3Add(m_Position, XMFloat3Dot(m_Forward, m_SaberLength));

		if (m_Forward.x == 0.0f || m_Forward.y == 0.0f || m_Forward.z == 0.0f)
		{
			continue;
		}

		// ���_�̈ʒu�𑊑Έʒu�ɂ���
		x_min -= m_Position.x;
		y_min -= m_Position.y;
		z_min -= m_Position.z;
		x_max -= m_Position.x;
		y_max -= m_Position.y;
		z_max -= m_Position.z;

		// ���̌�������ő�l�ƍŏ��l�𕪂���Forward�x�N�g���̔{����3�����߂�
		if (m_Forward.x > 0.0f)
		{
			x_min /= m_Forward.x;
			x_max /= m_Forward.x;
		}
		else
		{
			float temp = x_min;
			x_min = x_max / m_Forward.x;
			x_max = temp / m_Forward.x;
		}

		if (m_Forward.y > 0.0f)
		{
			y_min /= m_Forward.y;
			y_max /= m_Forward.y;
		}
		else
		{
			float temp = y_min;
			y_min = y_max / m_Forward.y;
			y_max = temp / m_Forward.y;
		}

		if (m_Forward.z > 0.0f)
		{
			z_min /= m_Forward.z;
			z_max /= m_Forward.z;
		}
		else
		{
			float temp = z_min;
			z_min = z_max / m_Forward.z;
			z_max = temp / m_Forward.z;
		}

		// 3���ʂɂ����Ĉ�ԋ߂��ʁA��ԉ����ʂ����߂�
		float xyz_min_max = FloatMax(z_min, FloatMax(x_min, y_min));
		float xyz_max_min = FloatMin(z_max, FloatMin(x_max, y_max));

		if (xyz_min_max < xyz_max_min)
		{
			if (xyz_min_max < 0.0f || xyz_min_max > m_SaberLength)
			{
				continue;
			}
			hit = true;
			note->SetHitSaber(m_isRightHand);
			break;
		}
	}

	return hit;
}

bool MainSceneHand::isOnPolygon(XMFLOAT3 vertex1, XMFLOAT3 vertex2, XMFLOAT3 vertex3, XMFLOAT3 vec)
{
	XMFLOAT3 vertex_vec1 = XMFloat3Suba(vertex2, vertex1);
	XMFLOAT3 vertex_vec2 = XMFloat3Suba(vertex3, vertex2);
	if (XMFloat3Dot(XMFloat3Cross(vertex_vec1, vertex_vec2), XMFloat3Cross(vertex_vec1, vec)) < 0)
	{// �ٕ����Ȃ炱�̎O�p�|���S���ɏՓ˓_�͂Ȃ�
		return false;
	}
	return true;
}