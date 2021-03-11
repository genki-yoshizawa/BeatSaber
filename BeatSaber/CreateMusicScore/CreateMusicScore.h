#pragma once

#include <list>
#include <string>
#include "GameAudio/GameAudio.h"
#include "GameObject/MainSceneObject/MainSceneNotes/MainSceneNotes.h"

class CreateMusicScore
{
private:
	// シングルトン
	static CreateMusicScore* m_Instance;
	CreateMusicScore() {}
	~CreateMusicScore() {}
	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new CreateMusicScore();
			if (m_Instance)
			{
				return;
			}
		}
	}

	std::list<MainSceneNotes*> m_Notes;	// ノーツオブジェクト

	SOUND_LABEL m_MusicScore;			// 楽譜
	std::string m_Difficulty;			// 難易度

	const std::string m_PathToJson = "nlohmann/music_score.json";	// 読み込むjsonファイルのパス

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

	void LoadMusicScore();	// 楽譜の読み込み

	void Init();	// 初期化
	void Uninit();	// 解放
	void Update();	// 更新
	

	void SetMusicScore(SOUND_LABEL music_score) { m_MusicScore = music_score; }	// 楽譜のセット
	SOUND_LABEL GetMusicScore() { return m_MusicScore; }						// 楽譜の取得
	void SetDifficulty(std::string difficulty) { m_Difficulty = difficulty; }	// 難易度のセット
};