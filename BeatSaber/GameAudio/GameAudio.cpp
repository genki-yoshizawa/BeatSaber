#include "GameAudio.h"

GameAudio* GameAudio::m_Instance = nullptr;

//=============================================================================
// ����������
//=============================================================================
bool GameAudio::InitSound(HWND hWnd)
{
	HRESULT hr;

	// COM���C�u�����̏�����
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2�I�u�W�F�N�g�̍쐬
	hr = XAudio2Create(&m_XAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2�I�u�W�F�N�g�̍쐬�Ɏ��s�I", "�x���I", MB_ICONWARNING);

		// COM���C�u�����̏I������
		CoUninitialize();

		return false;
	}

	// �}�X�^�[�{�C�X�̐���
	hr = m_XAudio2->CreateMasteringVoice(&m_MasteringVoice);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�}�X�^�[�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);

		if (m_XAudio2)
		{
			// XAudio2�I�u�W�F�N�g�̊J��
			m_XAudio2->Release();
			m_XAudio2 = NULL;
		}

		// COM���C�u�����̏I������
		CoUninitialize();

		return false;
	}

	for(int sound_ID = 0; sound_ID < SOUND_LABEL_MAX; sound_ID++)
	{
		m_SoundLabel[(SOUND_LABEL)sound_ID] = m_SoundName[sound_ID];
	}

	// �T�E���h�f�[�^�̏�����
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// �o�b�t�@�̃N���A
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// �T�E���h�f�[�^�t�@�C���̐���
		hFile = CreateFile(m_Param[nCntSound].Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return false;
		}
		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// �t�@�C���|�C���^��擪�Ɉړ�
			MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return false;
		}

		// WAVE�t�@�C���̃`�F�b�N
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return false;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return false;
		}
		if (dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(3)", "�x���I", MB_ICONWARNING);
			return false;
		}

		// �t�H�[�}�b�g�`�F�b�N
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return false;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return false;
		}

		// �I�[�f�B�I�f�[�^�ǂݍ���
		hr = CheckChunk(hFile, 'atad', &m_SizeAudio[nCntSound], &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
			return false;
		}
		m_DataAudio[nCntSound] = (BYTE*)malloc(m_SizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_DataAudio[nCntSound], m_SizeAudio[nCntSound], dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
			return false;
		}

		// �\�[�X�{�C�X�̐���
		hr = m_XAudio2->CreateSourceVoice(&m_SourceVoice[nCntSound], &(wfx.Format));
		if (FAILED(hr))
		{
			MessageBox(hWnd, "�\�[�X�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
			return false;
		}

		m_BytePerSample[nCntSound] = (DWORD)(wfx.Format.wBitsPerSample * 0.125f);
		m_SampleRate[nCntSound] = wfx.Format.nSamplesPerSec;
		// �Ȃ����T�C�Y��2�{�ɂȂ��Ă�̂Ŗ������C���BCheckChunk()�̂������2�{���Ă���C�����邪�A���͕s���Ō������킩��Ȃ����ߒ���
		//m_SizeAudio[nCntSound] *= 0.5f;

		// �o�b�t�@�̒l�ݒ�
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_SizeAudio[nCntSound];
		buffer.pAudioData = m_DataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = m_Param[nCntSound].CntLoop;

		// �I�[�f�B�I�o�b�t�@�̓o�^
		m_SourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}

	return true;
}

//=============================================================================
// �I������
//=============================================================================
void GameAudio::UninitSound(void)
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_SourceVoice[nCntSound])
		{
			// �ꎞ��~
			m_SourceVoice[nCntSound]->Stop(0);

			// �\�[�X�{�C�X�̔j��
			m_SourceVoice[nCntSound]->DestroyVoice();
			m_SourceVoice[nCntSound] = NULL;

			// �I�[�f�B�I�f�[�^�̊J��
			free(m_DataAudio[nCntSound]);
			m_DataAudio[nCntSound] = NULL;
		}
	}

	// �}�X�^�[�{�C�X�̔j��
	m_MasteringVoice->DestroyVoice();
	m_MasteringVoice = NULL;

	if (m_XAudio2)
	{
		// XAudio2�I�u�W�F�N�g�̊J��
		m_XAudio2->Release();
		m_XAudio2 = NULL;
	}

	// COM���C�u�����̏I������
	CoUninitialize();
}

//=============================================================================
// �Z�O�����g�Đ�(�Đ����Ȃ��~)
//=============================================================================
void GameAudio::PlaySound(SOUND_LABEL label, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_SizeAudio[label];
	buffer.pAudioData = m_DataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (nCntLoop < -1) {
		buffer.LoopCount = m_Param[label].CntLoop;
	}
	else
	{
		buffer.LoopCount = nCntLoop;
	}
	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_SourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_SourceVoice[label]->FlushSourceBuffers();
	}
	// �T���v�����̎擾
	m_PlayStartSample[label] = (UINT32)xa2state.SamplesPlayed;

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_SourceVoice[label]->Start(0);

}

void GameAudio::PlaySound(SOUND_LABEL label, UINT32 playbegin, UINT32 playend, UINT32 loopbegin, UINT32 loopend, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_SizeAudio[label];
	buffer.pAudioData = m_DataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (nCntLoop < -1) {
		buffer.LoopCount = m_Param[label].CntLoop;
	}
	else
	{
		buffer.LoopCount = nCntLoop;
	}

	buffer.PlayBegin = playbegin;		
	buffer.PlayLength = playend - playbegin;					//PlayBegin+PlayLength�Ŏڑ��炸�ɂȂ�ƃG���[�N����
	buffer.LoopBegin = loopbegin;
	buffer.LoopLength = loopend - loopend;
	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_SourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_SourceVoice[label]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_SourceVoice[label]->Start(0);

}

void GameAudio::PlaySound(SOUND_LABEL label, float playbegin, float playend, float loopbegin, float loopend, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_SizeAudio[label];
	buffer.pAudioData = m_DataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (nCntLoop < -1) {
		buffer.LoopCount = m_Param[label].CntLoop;
	}
	else
	{
		buffer.LoopCount = nCntLoop;
	}

	buffer.PlayBegin = (UINT32)(playbegin * m_SampleRate[label]);		//44100��1�b?(�T���v�����[�g�I��)
	buffer.PlayLength = (UINT32)((playend - playbegin) * m_SampleRate[label]);					//PlayBegin+PlayLength�Ŏڑ��炸�ɂȂ�ƃG���[�N����
	buffer.LoopBegin = (UINT32)(loopbegin * m_SampleRate[label]);
	buffer.LoopLength = (UINT32)((loopend - loopend) * m_SampleRate[label]);
	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_SourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_SourceVoice[label]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_SourceVoice[label]->Start(0);

}

void GameAudio::PlaySound(SOUND_LABEL label, UINT32 playend, UINT32 loopbegin, UINT32 loopend, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_SizeAudio[label];
	buffer.pAudioData = m_DataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (nCntLoop < -1) {
		buffer.LoopCount = m_Param[label].CntLoop;
	}
	else
	{
		buffer.LoopCount = nCntLoop;
	}

	buffer.PlayBegin = 0;		
	buffer.PlayLength = playend;					//PlayBegin+PlayLength�Ŏڑ��炸�ɂȂ�ƃG���[�N����
	buffer.LoopBegin = loopbegin;
	buffer.LoopLength = loopend - loopbegin;
	;
	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_SourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_SourceVoice[label]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_SourceVoice[label]->Start(0);

}

void GameAudio::PlaySound(SOUND_LABEL label, float playend, float loopbegin, float loopend, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_SizeAudio[label];
	buffer.pAudioData = m_DataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (nCntLoop < -1) {
		buffer.LoopCount = m_Param[label].CntLoop;
	}
	else
	{
		buffer.LoopCount = nCntLoop;
	}

	buffer.PlayBegin = 0;		
	buffer.PlayLength = (UINT32)(playend * m_SampleRate[label]);					//PlayBegin+PlayLength�Ŏڑ��炸�ɂȂ�ƃG���[�N����
	buffer.LoopBegin = (UINT32)(loopbegin * m_SampleRate[label]);
	buffer.LoopLength = (UINT32)((loopend - loopbegin) * m_SampleRate[label]);
	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_SourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_SourceVoice[label]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_SourceVoice[label]->Start(0);
}



//=============================================================================
// �Z�O�����g�ꎞ��~(���x���w��)
//=============================================================================
void GameAudio::PauseSound(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		m_SourceVoice[label]->Stop(0);
	}
}

//=============================================================================
// �Z�O�����g�ꎞ��~(�S��)
//=============================================================================
void GameAudio::PauseSound(void)
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_SourceVoice[nCntSound])
		{
			// �ꎞ��~
			m_SourceVoice[nCntSound]->Stop(0);
		}
	}
}
//=============================================================================
// �Z�O�����g��~(���x���w��)
//=============================================================================
void GameAudio::StopSound(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// ��~
		m_SourceVoice[label]->Stop(0);
		xa2state.SamplesPlayed = 0;

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_SourceVoice[label]->FlushSourceBuffers();
	}
}

//=============================================================================
// �Z�O�����g��~(�S��)
//=============================================================================
void GameAudio::StopSound(void)
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		XAUDIO2_VOICE_STATE xa2state;
		if (m_SourceVoice[nCntSound])
		{
			m_SourceVoice[nCntSound]->GetState(&xa2state);

			// ��~
			m_SourceVoice[nCntSound]->Stop(0);
			xa2state.SamplesPlayed = 0;
		}
	}
}

void GameAudio::SetVolumeSound(SOUND_LABEL label, float volume)
{
	m_SourceVoice[label]->SetVolume(volume);
}

float GameAudio::GetPlaybackSecond(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �X�^�[�g���̃T���v�������Z���邱�ƂňȑO�ɍĐ�����ĂĂ����Ȃ��Ȃ�
		UINT32 samples = (UINT32)(xa2state.SamplesPlayed - m_PlayStartSample[label]);

		return (float)(samples / (float)m_SampleRate[label]);
	}

	return 0.0f;
}

bool GameAudio::isFinishSound(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{
		if ( (xa2state.SamplesPlayed - m_PlayStartSample[label]) >= (m_SizeAudio[label] * 0.5f / m_BytePerSample[label]))
		{
			return true;
		}
	}
	return false;
}

//=============================================================================
// �`�����N�̃`�F�b�N
//=============================================================================
HRESULT GameAudio::CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//=============================================================================
// �`�����N�f�[�^�̓ǂݍ���
//=============================================================================
HRESULT GameAudio::ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
