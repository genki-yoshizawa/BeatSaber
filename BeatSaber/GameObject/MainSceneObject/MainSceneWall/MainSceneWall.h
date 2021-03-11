#pragma once

#include "GameObject/MainSceneObject/MainSceneNotes/MainSceneNotes.h"

class MainSceneWall : public MainSceneNotes
{
private:
	
public:
	MainSceneWall() = delete;
	MainSceneWall(float appere_time, float end_time,int appere_lane, int judge_lane, int notes_type, int direction)
		:MainSceneNotes(appere_time, appere_lane, judge_lane, notes_type, direction) 
	{
		m_EndTime = end_time;
	}
	~MainSceneWall() {}

	void Init() override;		// èâä˙âª
	void Update() override;		// çXêV
	void Draw() override;		// ï`âÊ

	void SetHitSaber() = delete;


	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(MainSceneWall), __alignof(MainSceneWall));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};