#pragma once

#include "GameObject/MainSceneObject/MainSceneNotes/MainSceneNotes.h"

class MainSceneBomb : public MainSceneNotes
{
private:

public:
	MainSceneBomb() = delete;
	MainSceneBomb(float time, int appere_lane, int judge_lane, int notes_type, int direction)
		:MainSceneNotes(time, appere_lane, judge_lane, notes_type, direction){}
	~MainSceneBomb(){}


	void Init() override;		// èâä˙âª
	void Update() override;		// çXêV


	static void* operator new(std::size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(MainSceneBomb), __alignof(MainSceneBomb));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
};