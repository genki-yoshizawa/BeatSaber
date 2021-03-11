#include <fstream>
#include "nlohmann/json.hpp"

#include "CreateMusicScore.h"
#include "Scene/MainScene/MainScene.h"
#include "GameObject/MainSceneObject/MainSceneBomb/MainSceneBomb.h"
#include "GameObject/MainSceneObject/MainSceneWall/MainScenewall.h"

CreateMusicScore* CreateMusicScore::m_Instance = nullptr;

void CreateMusicScore::Init()
{
	LoadMusicScore();

	GameAudio::GetInstance()->PlaySound(m_MusicScore);
	//GameAudio::GetInstance()->SetVolumeSound(m_MusicScore, 0.1f);
}

void CreateMusicScore::Uninit()
{
		for (MainSceneNotes* note : m_Notes)
		{
			note->Uninit();
			delete note;
		}

		m_Notes.clear();

		GameAudio::GetInstance()->StopSound(m_MusicScore);
}

void CreateMusicScore::Update()
{
	// 出現時間を過ぎているノーツがあればシーン内に追加し、削除する

	// remove_if()を使う方法もあり。C++20以降ならerase_if()も
	std::list<MainSceneNotes*>::iterator it = m_Notes.begin();
	
	for (it; it != m_Notes.end();)
	{
		MainSceneNotes* notes = *it;

		// 出現時間でなければ次のノーツに移る
		if (GameAudio::GetInstance()->GetPlaybackSecond(m_MusicScore) < notes->GetAppereTime())
		{
			++it;
			continue;
		}

		// 出現時間を過ぎていればMainSceneにノーツを追加する
		if (notes->GetNotesType() == 3)
		{// 壁オブジェクト
			MainScene::GetInstance()->AddNotesObject(notes, 2);
		}
		else
		{
			MainScene::GetInstance()->AddNotesObject(notes, 1);
		}
		//delete *it ノーツを消去して次のノーツに移る
		it = m_Notes.erase(it);
	}
}

void CreateMusicScore::LoadMusicScore()
{
	// json形式のファイルを開く
	std::ifstream ifs(m_PathToJson.c_str());
	nlohmann::json json_file;

	if (ifs.good())
	{
		ifs >> json_file;
	}

	nlohmann::json music_score_json = json_file[GameAudio::GetInstance()->GetSoundName(m_MusicScore)][m_Difficulty];

	for(auto music_score : music_score_json["ScoreDataModels"])
	{
		MainSceneNotes* note;
		float judge_time = music_score["Time"];
		float end_judge_time;
		switch((int)music_score["NotesType"])
		{
		case 0/*左手ノーツ*/:
		case 1/*右手ノーツ*/:
			note = new MainSceneNotes((judge_time - 2.25f), music_score["AppereLane"], music_score["JudgeLane"], music_score["NotesType"], music_score["Direction"]);
			note->Init();
			m_Notes.push_back(note);
			break;

		case 2/*ボム*/:
			note = new MainSceneBomb((judge_time - 2.25f), music_score["AppereLane"], music_score["JudgeLane"], music_score["NotesType"], music_score["Direction"]);
			note->Init();
			m_Notes.push_back(note);
			break;

		case 3/*壁*/:
			end_judge_time = music_score["EndTime"];
			note = new MainSceneWall((judge_time - 2.25f), (end_judge_time - 2.25f), music_score["AppereLane"], music_score["JudgeLane"], music_score["NotesType"], music_score["Direction"]);
			note->Init();
			m_Notes.push_back(note);

			break;
		default:
			break;
		}
	}

	// ファイルを閉じる
	ifs.close();
}