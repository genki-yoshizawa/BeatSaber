#pragma once
#define LAYER_NUM 4

#include <list>
#include <vector>

#include "GameObject/GameObject.h"
#include "model.h"
#include <typeinfo>

class Scene
{
protected:
	std::list<GameObject*> m_GameObject[LAYER_NUM];
	static CModel* m_NotesModelBlue;
	static CModel* m_NotesModelRed;
	static CModel* m_GripBlueModel;
	static CModel* m_GripRedModel;
	static CModel* m_Bomb;
	static CModel* m_Wall;
	static CModel* m_Table;
	static CModel* m_Light;

public:
	Scene() {}
	virtual ~Scene() {}

	static void LoadModel();
	static void UnloadModel();

	virtual void Init() = 0;	// 初期化

	virtual void Uninit();		// 解放

	virtual void Update();		// 更新

	virtual void Draw();		// 描画

	// 初期化に引数が要らないときの簡易なGameObject追加テンプレート関数
	template <typename T>
	T* AddGameObject(int layer)
	{
		T* gameObject = new T();
		m_GameObject[layer].push_back(gameObject);
		gameObject->Init();

		return gameObject;
	}

	void AddGameObject(GameObject* game_object, int layer) { m_GameObject[layer].push_back(game_object); }

	// シーン中の特定のゲームオブジェクトの取得
	template <typename T>
	T* GetGameObject()
	{
		for (int i = 0; i < LAYER_NUM; i++) {
			for (GameObject* object : m_GameObject[i])
			{
				if (typeid(*object) == typeid(T))
				{
					return (T*)object;
				}
			}
		}
		return NULL;
	}

	// シーン中の特定の複数あるゲームオブジェクトをvector配列で取得
	template <typename T>
	std::vector<T*> GetGameObjects()
	{
		std::vector<T*> objects;
		for (int i = 0; i < LAYER_NUM; i++) {
			for (GameObject* object : m_GameObject[i])
			{
				if (typeid(*object) == typeid(T))
				{
					objects.push_back((T*)object);
				}
			}
		}
		return objects;
	}

	static CModel* GetNotesModelBlue() { return m_NotesModelBlue; }
	static CModel* GetNotesModelRed() { return m_NotesModelRed; }
	static CModel* GetGripModel(bool is_right) { return is_right ? m_GripBlueModel : m_GripRedModel; }
	static CModel* GetBomb() { return m_Bomb; }
	static CModel* GetWall() { return m_Wall; }
	static CModel* GetTable() { return m_Table; }
	static CModel* GetLight() { return m_Light; }
};