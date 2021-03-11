#pragma once
#include "GameObject/GameObject.h"

class CCube  : public GameObject{
private:
	/*D3DXVECTOR3*/ XMFLOAT3 m_Position;
	/*D3DXVECTOR3*/ XMFLOAT4 m_Rotation;
	/*D3DXVECTOR3*/ XMFLOAT3 m_Scale;

	/*D3DXQUATERNION*/ XMVECTOR m_Quaternion;

	XMFLOAT3 m_HeadPosition;

	// 前方宣言(ポインタのみ)
	class CModel *m_Model;

public:
	CCube() {}
	~CCube() {}

	void Init();
	void Uninit();
	void Update();
	void Draw(/*XMMATRIX * projView*/);

	/*D3DXVECTOR3*/ XMFLOAT3 GetPosition() { return m_Position; }
	/*D3DXVECTOR3*/ XMFLOAT4 GetRotation() { return m_Rotation; }
	/*D3DXVECTOR3*/ XMFLOAT3 GetScale() { return m_Scale; }

	//void SetPosition(D3DXVECTOR3 Position) { m_Position = Position; }
	//void SetRotation(D3DXVECTOR3 Rotation) { m_Rotation = Rotation; }
	//void SetScale(D3DXVECTOR3 Scale) { m_Scale = Scale; }

	void SetPosition(float x, float y, float z) { m_Position = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { m_Rotation = XMFLOAT4(x, y, z, 1.0f); }
	void SetScale(float x, float y, float z) { m_Scale = XMFLOAT3(x, y, z); }

	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(CCube), __alignof(CCube));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};