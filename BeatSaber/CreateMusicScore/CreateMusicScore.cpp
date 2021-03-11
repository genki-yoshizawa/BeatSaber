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
	// �o�����Ԃ��߂��Ă���m�[�c������΃V�[�����ɒǉ����A�폜����

	// remove_if()���g�����@������BC++20�ȍ~�Ȃ�erase_if()��
	std::list<MainSceneNotes*>::iterator it = m_Notes.begin();
	
	for (it; it != m_Notes.end();)
	{
		MainSceneNotes* notes = *it;

		// �o�����ԂłȂ���Ύ��̃m�[�c�Ɉڂ�
		if (GameAudio::GetInstance()->GetPlaybackSecond(m_MusicScore) < notes->GetAppereTime())
		{
			++it;
			continue;
		}

		// �o�����Ԃ��߂��Ă����MainScene�Ƀm�[�c��ǉ�����
		if (notes->GetNotesType() == 3)
		{// �ǃI�u�W�F�N�g
			MainScene::GetInstance()->AddNotesObject(notes, 2);
		}
		else
		{
			MainScene::GetInstance()->AddNotesObject(notes, 1);
		}
		//delete *it �m�[�c���������Ď��̃m�[�c�Ɉڂ�
		it = m_Notes.erase(it);
	}
}

void CreateMusicScore::LoadMusicScore()
{
	// json�`���̃t�@�C�����J��
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
		case 0/*����m�[�c*/:
		case 1/*�E��m�[�c*/:
			note = new MainSceneNotes((judge_time - 2.25f), music_score["AppereLane"], music_score["JudgeLane"], music_score["NotesType"], music_score["Direction"]);
			note->Init();
			m_Notes.push_back(note);
			break;

		case 2/*�{��*/:
			note = new MainSceneBomb((judge_time - 2.25f), music_score["AppereLane"], music_score["JudgeLane"], music_score["NotesType"], music_score["Direction"]);
			note->Init();
			m_Notes.push_back(note);
			break;

		case 3/*��*/:
			end_judge_time = music_score["EndTime"];
			note = new MainSceneWall((judge_time - 2.25f), (end_judge_time - 2.25f), music_score["AppereLane"], music_score["JudgeLane"], music_score["NotesType"], music_score["Direction"]);
			note->Init();
			m_Notes.push_back(note);

			break;
		default:
			break;
		}
	}

	// �t�@�C�������
	ifs.close();
}