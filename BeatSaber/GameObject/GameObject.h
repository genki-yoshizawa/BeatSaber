#pragma once

class GameObject
{
protected:
	bool m_Destroy = false;		// �j��t���O

public:
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Init() = 0;	// ������
	virtual void Uninit() = 0;	// ���
	virtual void Update() = 0;	// �X�V
	virtual void Draw() = 0;	// �`��

	void SetDestroy() { m_Destroy = true; }	// �j��t���O�𗧂Ă�
	bool GetDestroy() { return m_Destroy; }	// �j��t���O�̎擾
	bool Destroy();							// �j�󏈗�
};