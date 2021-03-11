#include "GameAudio.h"

GameAudio* GameAudio::m_Instance = nullptr;

//=============================================================================
// 初期化処理
//=============================================================================
bool GameAudio::InitSound(HWND hWnd)
{
	HRESULT hr;

	// COMライブラリの初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&m_XAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return false;
	}

	// マスターボイスの生成
	hr = m_XAudio2->CreateMasteringVoice(&m_MasteringVoice);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if (m_XAudio2)
		{
			// XAudio2オブジェクトの開放
			m_XAudio2->Release();
			m_XAudio2 = NULL;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return false;
	}

	for(int sound_ID = 0; sound_ID < SOUND_LABEL_MAX; sound_ID++)
	{
		m_SoundLabel[(SOUND_LABEL)sound_ID] = m_SoundName[sound_ID];
	}

	// サウンドデータの初期化
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile(m_Param[nCntSound].Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
			return false;
		}
		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
			return false;
		}

		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return false;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return false;
		}
		if (dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
			return false;
		}

		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return false;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return false;
		}

		// オーディオデータ読み込み
		hr = CheckChunk(hFile, 'atad', &m_SizeAudio[nCntSound], &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
			return false;
		}
		m_DataAudio[nCntSound] = (BYTE*)malloc(m_SizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_DataAudio[nCntSound], m_SizeAudio[nCntSound], dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
			return false;
		}

		// ソースボイスの生成
		hr = m_XAudio2->CreateSourceVoice(&m_SourceVoice[nCntSound], &(wfx.Format));
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
			return false;
		}

		m_BytePerSample[nCntSound] = (DWORD)(wfx.Format.wBitsPerSample * 0.125f);
		m_SampleRate[nCntSound] = wfx.Format.nSamplesPerSec;
		// なぜかサイズが2倍になってるので無理やり修正。CheckChunk()のあたりで2倍している気がするが、実力不足で原因がわからないため注意
		//m_SizeAudio[nCntSound] *= 0.5f;

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_SizeAudio[nCntSound];
		buffer.pAudioData = m_DataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = m_Param[nCntSound].CntLoop;

		// オーディオバッファの登録
		m_SourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}

	return true;
}

//=============================================================================
// 終了処理
//=============================================================================
void GameAudio::UninitSound(void)
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_SourceVoice[nCntSound])
		{
			// 一時停止
			m_SourceVoice[nCntSound]->Stop(0);

			// ソースボイスの破棄
			m_SourceVoice[nCntSound]->DestroyVoice();
			m_SourceVoice[nCntSound] = NULL;

			// オーディオデータの開放
			free(m_DataAudio[nCntSound]);
			m_DataAudio[nCntSound] = NULL;
		}
	}

	// マスターボイスの破棄
	m_MasteringVoice->DestroyVoice();
	m_MasteringVoice = NULL;

	if (m_XAudio2)
	{
		// XAudio2オブジェクトの開放
		m_XAudio2->Release();
		m_XAudio2 = NULL;
	}

	// COMライブラリの終了処理
	CoUninitialize();
}

//=============================================================================
// セグメント再生(再生中なら停止)
//=============================================================================
void GameAudio::PlaySound(SOUND_LABEL label, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
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
	// 状態取得
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_SourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_SourceVoice[label]->FlushSourceBuffers();
	}
	// サンプル数の取得
	m_PlayStartSample[label] = (UINT32)xa2state.SamplesPlayed;

	// オーディオバッファの登録
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_SourceVoice[label]->Start(0);

}

void GameAudio::PlaySound(SOUND_LABEL label, UINT32 playbegin, UINT32 playend, UINT32 loopbegin, UINT32 loopend, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
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
	buffer.PlayLength = playend - playbegin;					//PlayBegin+PlayLengthで尺足らずになるとエラー起きる
	buffer.LoopBegin = loopbegin;
	buffer.LoopLength = loopend - loopend;
	// 状態取得
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_SourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_SourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_SourceVoice[label]->Start(0);

}

void GameAudio::PlaySound(SOUND_LABEL label, float playbegin, float playend, float loopbegin, float loopend, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
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

	buffer.PlayBegin = (UINT32)(playbegin * m_SampleRate[label]);		//44100で1秒?(サンプルレート的に)
	buffer.PlayLength = (UINT32)((playend - playbegin) * m_SampleRate[label]);					//PlayBegin+PlayLengthで尺足らずになるとエラー起きる
	buffer.LoopBegin = (UINT32)(loopbegin * m_SampleRate[label]);
	buffer.LoopLength = (UINT32)((loopend - loopend) * m_SampleRate[label]);
	// 状態取得
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_SourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_SourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_SourceVoice[label]->Start(0);

}

void GameAudio::PlaySound(SOUND_LABEL label, UINT32 playend, UINT32 loopbegin, UINT32 loopend, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
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
	buffer.PlayLength = playend;					//PlayBegin+PlayLengthで尺足らずになるとエラー起きる
	buffer.LoopBegin = loopbegin;
	buffer.LoopLength = loopend - loopbegin;
	;
	// 状態取得
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_SourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_SourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_SourceVoice[label]->Start(0);

}

void GameAudio::PlaySound(SOUND_LABEL label, float playend, float loopbegin, float loopend, int nCntLoop)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
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
	buffer.PlayLength = (UINT32)(playend * m_SampleRate[label]);					//PlayBegin+PlayLengthで尺足らずになるとエラー起きる
	buffer.LoopBegin = (UINT32)(loopbegin * m_SampleRate[label]);
	buffer.LoopLength = (UINT32)((loopend - loopbegin) * m_SampleRate[label]);
	// 状態取得
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_SourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_SourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_SourceVoice[label]->Start(0);
}



//=============================================================================
// セグメント一時停止(ラベル指定)
//=============================================================================
void GameAudio::PauseSound(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_SourceVoice[label]->Stop(0);
	}
}

//=============================================================================
// セグメント一時停止(全て)
//=============================================================================
void GameAudio::PauseSound(void)
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (m_SourceVoice[nCntSound])
		{
			// 一時停止
			m_SourceVoice[nCntSound]->Stop(0);
		}
	}
}
//=============================================================================
// セグメント停止(ラベル指定)
//=============================================================================
void GameAudio::StopSound(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 停止
		m_SourceVoice[label]->Stop(0);
		xa2state.SamplesPlayed = 0;

		// オーディオバッファの削除
		m_SourceVoice[label]->FlushSourceBuffers();
	}
}

//=============================================================================
// セグメント停止(全て)
//=============================================================================
void GameAudio::StopSound(void)
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		XAUDIO2_VOICE_STATE xa2state;
		if (m_SourceVoice[nCntSound])
		{
			m_SourceVoice[nCntSound]->GetState(&xa2state);

			// 停止
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

	// 状態取得
	m_SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// スタート時のサンプルを減算することで以前に再生されてても問題なくなる
		UINT32 samples = (UINT32)(xa2state.SamplesPlayed - m_PlayStartSample[label]);

		return (float)(samples / (float)m_SampleRate[label]);
	}

	return 0.0f;
}

bool GameAudio::isFinishSound(SOUND_LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
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
// チャンクのチェック
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
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
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
// チャンクデータの読み込み
//=============================================================================
HRESULT GameAudio::ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
