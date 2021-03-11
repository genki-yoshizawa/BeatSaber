#pragma once

#include <list>
#include <string>
#include "GameAudio/GameAudio.h"
#include "GameObject/MainSceneObject/MainSceneNotes/MainSceneNotes.h"

class CreateMusicScore
{
private:
	// �V���O���g��
	static CreateMusicScore* m_Instance;
	CreateMusicScore() {}
	~CreateMusicScore() {}
	static void Create()
	{
		// �C���X�^���X������CREATE_INSTANCE_RETRY��܂Ŏ���
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new CreateMusicScore();
			if (m_Instance)
			{
				return;
			}
		}
	}

	std::list<MainSceneNotes*> m_Notes;	// �m�[�c�I�u�W�F�N�g

	SOUND_LABEL m_MusicScore;			// �y��
	std::string m_Difficulty;			// ��Փx

	const std::string m_PathToJson = "nlohmann/music_score.json";	// �ǂݍ���json�t�@�C���̃p�X

public:
	CreateMusicScore(const CreateMusicScore&) = delete;
	CreateMusicScore operator=(const CreateMusicScore&) = delete;
	CreateMusicScore(CreateMusicScore&&) = delete;
	CreateMusicScore operator=(CreateMusicScore&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static CreateMusicScore* GetInstance()
	{
		if (!m_Instance)
		{
			Create();
		}
		return m_Instance;
	}

	void LoadMusicScore();	// �y���̓ǂݍ���

	void Init();	// ������
	void Uninit();	// ���
	void Update();	// �X�V
	

	void SetMusicScore(SOUND_LABEL music_score) { m_MusicScore = music_score; }	// �y���̃Z�b�g
	SOUND_LABEL GetMusicScore() { return m_MusicScore; }						// �y���̎擾
	void SetDifficulty(std::string difficulty) { m_Difficulty = difficulty; }	// ��Փx�̃Z�b�g
};