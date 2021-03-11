#include "Scene.h"

CModel* Scene::m_NotesModelBlue = nullptr;
CModel* Scene::m_NotesModelRed = nullptr;
CModel* Scene::m_GripBlueModel = nullptr;
CModel* Scene::m_GripRedModel = nullptr;
CModel* Scene::m_Bomb = nullptr;
CModel* Scene::m_Wall = nullptr;
CModel* Scene::m_Table = nullptr;
CModel* Scene::m_Light = nullptr;

void Scene::LoadModel()
{
	m_NotesModelBlue = new CModel();
	m_NotesModelBlue->Load("asset\\model\\notes_blue.obj");

	m_NotesModelRed = new CModel();
	m_NotesModelRed->Load("asset\\model\\notes_red.obj");

	m_GripBlueModel = new CModel();
	m_GripBlueModel->Load("asset\\model\\light_saber_blue.obj");

	m_GripRedModel = new CModel();
	m_GripRedModel->Load("asset\\model\\light_saber_red.obj");

	m_Bomb = new CModel();
	m_Bomb->Load("asset\\model\\bomb.obj");

	m_Wall = new CModel();
	m_Wall->Load("asset\\model\\wall.obj");

	m_Table = new CModel();
	m_Table->Load("asset\\model\\background_table.obj");

	m_Light = new CModel();
	m_Light->Load("asset\\model\\background_light.obj");

}

void Scene::UnloadModel()
{
	m_NotesModelBlue->Unload();
	delete m_NotesModelBlue;

	m_NotesModelRed->Unload();
	delete m_NotesModelRed;

	m_GripRedModel->Unload();
	delete m_GripRedModel;

	m_GripBlueModel->Unload();
	delete m_GripBlueModel;

	m_Bomb->Unload();
	delete m_Bomb;

	m_Wall->Unload();
	delete m_Wall;

	m_Table->Unload();
	delete m_Table;

	m_Light->Unload();
	delete m_Light;
}

void Scene::Uninit()
{
	for (int i = 0; i < LAYER_NUM; i++) {
		for (GameObject* object : m_GameObject[i])
		{
			object->Uninit();
			delete object;
		}

		m_GameObject[i].clear();
	}
}

void Scene::Update()
{
	for (int i = 0; i < LAYER_NUM; i++) {
		for (GameObject* object : m_GameObject[i])
		{
			object->Update();
		}

		m_GameObject[i].remove_if([](GameObject* object) { return object->Destroy(); });
	}
}

void Scene::Draw()
{
	for (int i = 0; i < LAYER_NUM; i++) {
		for (GameObject* object : m_GameObject[i])
		{
			object->Draw();
		}
	}
}