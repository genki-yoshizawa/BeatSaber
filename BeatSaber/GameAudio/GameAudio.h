#pragma once

#include "Main/Main.h"
#include "xaudio2.h"						// �T�E���h�����ŕK�v

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
// �p�����[�^�\���̒�`
//*****************************************************************************
typedef struct
{
	char *Filename;	// �t�@�C����
	int CntLoop;		// ���[�v�J�E���g
} SOUNDPARAM;

class GameAudio
{
private:
	static GameAudio* m_Instance;
	GameAudio() {}
	~GameAudio() {}
	static void Create()
	{
		// �C���X�^���X������CREATE_INSTANCE_RETRY��܂Ŏ���
		for (int i = 0; i < CREATE_INSTANCE_RETRY; i++)
		{
			m_Instance = new GameAudio();
			if (m_Instance)
			{
				return;
			}
		}
	}

	IXAudio2 *m_XAudio2 = NULL;								// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice *m_MasteringVoice = NULL;			// �}�X�^�[�{�C�X
	IXAudio2SourceVoice *m_SourceVoice[SOUND_LABEL_MAX] = {};	// �\�[�X�{�C�X
	BYTE *m_DataAudio[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^
	DWORD m_SizeAudio[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^�T�C�Y
	SOUNDPARAM m_Param[SOUND_LABEL_MAX] =
	{

		{(char *)"asset/SE/notes_hit.wav", 0},
		{(char *)"asset/SE/notes_miss.wav", 0},
		{(char *)"asset/SE/select.wav", 0},

		{(char *)"asset/BGM/BGM1.wav", -1},
	};

	// json�t�@�C���̖��O�����т���ϐ��BSE,���ʂ̂Ȃ�BGM�ɂ͓K���Ȗ��O������
	std::string m_SoundName[SOUND_LABEL_MAX] =
	{
		"notes_hit",
		"notes_miss",
		"select",

		"Music1"
	};

	std::unordered_map<SOUND_LABEL, std::string> m_SoundLabel;

	DWORD m_BytePerSample[SOUND_LABEL_MAX];					// 1�T���v��������Ɏg�p����r�b�g��(������g���΍Đ����Ԃ�������)
	DWORD m_SampleRate[SOUND_LABEL_MAX];					// �e�Ȃ̃T���v�����[�g
	UINT32 m_PlayStartSample[SOUND_LABEL_MAX];				// PlaySound()�g�p���̃T���v�����B���[�v�̂��Ƃ��l���������̂ŁAisFinishSound()�Ŏg��(���[�v�╔�����[�v�̂��͍̂l�����Ă��Ȃ�)

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

	// ���[�v�ݒ肵�Ă�ꍇ�A�ŏ�����Ō�𖳌����[�v����֐�(nCntLoop < -1�̎��f�t�H���g�Őݒ肵�Ă��郋�[�v�񐔂�ݒ肷��)
	void PlaySound(SOUND_LABEL label, int nCntLoop = -2);
	// UINT32(1/44100�P��)�ł��ꂼ��w�肷��֐�
	void PlaySound(SOUND_LABEL label, UINT32 playbegin, UINT32 playend, UINT32 loopbegin, UINT32 loopend, int nCntLoop = -2);
	// float(1�b�P��)�ł��ꂼ��w�肷��֐�
	void PlaySound(SOUND_LABEL label, float playbegin, float playend, float loopbegin, float loopend, int nCntLoop = -2);
	// �ŏ�����X�^�[�g����֐�
	void PlaySound(SOUND_LABEL label, UINT32 playend, UINT32 loopbegin, UINT32 loopend, int nCntLoop = -2);
	void PlaySound(SOUND_LABEL label, float playend, float loopbegin, float loopend, int nCntLoop = -2);



	// �Ȃ��ꎞ��~����֐�
	void PauseSound(SOUND_LABEL label);
	void PauseSound(void);
	// �Ȃ��~����֐�
	void StopSound(SOUND_LABEL label);
	void StopSound(void);

	void SetVolumeSound(SOUND_LABEL label, float volume);

	// ���x���Ŏw�肵���Ȃ̌��݂̍Đ����Ԃ̕b����float�l�ŕԂ�(�f�t�H���g��0��Ԃ�)
	float GetPlaybackSecond(SOUND_LABEL label);

	// ���x���Ŏw�肵���Ȃ��I����Ă��邩���ׂ�(���[�v�ɑ΂��Ė��Ή��B���ȏ�I����Ă��邩�͒��ׂ���)
	bool isFinishSound(SOUND_LABEL label);

	std::string GetSoundName(SOUND_LABEL sound_label) { return m_SoundLabel[sound_label]; }	// SOUND_LABEL����json�t�@�C�����̖��O��Ԃ�
};