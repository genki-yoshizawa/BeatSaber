#pragma once

#include "Main/Main.h"
#include "xaudio2.h"						// サウンド処理で必要

#include <unordered_map>

enum SOUND_LABEL
{
	SOUND_LABEL_SE_NOTES_HIT = 0,
	SOUND_LABEL_SE_NOTES_MISS,
	SOUND_LABEL_SE_SELECT,

	SOUND_LABEL_BGM_BGM1,

	SOUND_LABEL_MAX,

};

//*****************************************************************************
// パラメータ構造体定義
//*****************************************************************************
typedef struct
{
	char *Filename;	// ファイル名
	int CntLoop;		// ループカウント
} SOUNDPARAM;

class GameAudio
{
private:
	static GameAudio* m_Instance;
	GameAudio() {}
	~GameAudio() {}
	static void Create()
	{
		// インスタンス生成をCREATE_INSTANCE_RETRY回まで試す
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new GameAudio();
			if (m_Instance)
			{
				return;
			}
		}
	}

	IXAudio2 *m_XAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_MasteringVoice = NULL;			// マスターボイス
	IXAudio2SourceVoice *m_SourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
	BYTE *m_DataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
	DWORD m_SizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ
	SOUNDPARAM m_Param[SOUND_LABEL_MAX] =
	{

		{(char *)"asset/SE/notes_hit.wav", 0},
		{(char *)"asset/SE/notes_miss.wav", 0},
		{(char *)"asset/SE/select.wav", 0},

		{(char *)"asset/BGM/BGM1.wav", -1},
	};

	// jsonファイルの名前を結びつける変数。SE,譜面のないBGMには適当な名前をつける
	std::string m_SoundName[SOUND_LABEL_MAX] =
	{
		"notes_hit",
		"notes_miss",
		"select",

		"Music1"
	};

	std::unordered_map<SOUND_LABEL, std::string> m_SoundLabel;

	DWORD m_BytePerSample[SOUND_LABEL_MAX];					// 1サンプルあたりに使用するビット数(これを使えば再生時間が分かる)
	DWORD m_SampleRate[SOUND_LABEL_MAX];					// 各曲のサンプルレート
	UINT32 m_PlayStartSample[SOUND_LABEL_MAX];				// PlaySound()使用時のサンプル数。ループのことも考慮したもので、isFinishSound()で使う(ループや部分ループのものは考慮していない)

	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
	
public:
	GameAudio(const GameAudio&) = delete;
	GameAudio operator=(const GameAudio&) = delete;
	GameAudio(GameAudio&&) = delete;
	GameAudio operator=(GameAudio&&) = delete;
	static void Delete()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
	static GameAudio* GetInstance()
	{
		if (!m_Instance)
		{
			Create();
		}
		return m_Instance;
	}


	bool InitSound(HWND hWnd);
	void UninitSound(void);

	// ループ設定してる場合、最初から最後を無限ループする関数(nCntLoop < -1の時デフォルトで設定しているループ回数を設定する)
	void PlaySound(SOUND_LABEL label, int nCntLoop = -2);
	// UINT32(1/44100単位)でそれぞれ指定する関数
	void PlaySound(SOUND_LABEL label, UINT32 playbegin, UINT32 playend, UINT32 loopbegin, UINT32 loopend, int nCntLoop = -2);
	// float(1秒単位)でそれぞれ指定する関数
	void PlaySound(SOUND_LABEL label, float playbegin, float playend, float loopbegin, float loopend, int nCntLoop = -2);
	// 最初からスタートする関数
	void PlaySound(SOUND_LABEL label, UINT32 playend, UINT32 loopbegin, UINT32 loopend, int nCntLoop = -2);
	void PlaySound(SOUND_LABEL label, float playend, float loopbegin, float loopend, int nCntLoop = -2);



	// 曲を一時停止する関数
	void PauseSound(SOUND_LABEL label);
	void PauseSound(void);
	// 曲を停止する関数
	void StopSound(SOUND_LABEL label);
	void StopSound(void);

	void SetVolumeSound(SOUND_LABEL label, float volume);

	// ラベルで指定した曲の現在の再生時間の秒数をfloat値で返す(デフォルトで0を返す)
	float GetPlaybackSecond(SOUND_LABEL label);

	// ラベルで指定した曲が終わっているか調べる(ループに対して未対応。一回以上終わっているかは調べられる)
	bool isFinishSound(SOUND_LABEL label);

	std::string GetSoundName(SOUND_LABEL sound_label) { return m_SoundLabel[sound_label]; }	// SOUND_LABELからjsonファイル内の名前を返す
};