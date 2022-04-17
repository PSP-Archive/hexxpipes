// ############################################################################ 
// #     ________                                                             #
// #    /  \__/  \    HexxPipes                                               #
// #   /\__/  \__/\                                                           #
// #  / /  \__/  \ \  A cross platform game for Sony PSP and Win32/OpenGL     #
// #  \ \__/  \__/ /                                                          #
// #   \/  \__/  \/                                                           #
// #    \__/__\__/                                                            #
// #                                                                          #
// ############################################################################ 
// #                                                                          #
// # This file contains functions for playing music and sound fx on Win32.    #
// #                                                                          #
// #                                                                          #
// ############################################################################ 
#ifdef WIN32

#include "Win32Sound.h"
#include "../common/messagetext.h"
#include <windows.h>
#include <mmsystem.h>
#include "ModulePlayerThread.h"

//-----------------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------------

CModulePlayerThread m_Player;

/*
#define NUM_SAMPLES 8192
#define NUM_BUFFERS	16
#define SAMPLE_SIZE	4

float			m_fVolume;
DWORD			m_dwThreadID = 0;
HANDLE			m_hThread = 0;
BOOL			m_bStopThread = FALSE;
WAVEHDR			m_whdr[NUM_BUFFERS];
HWAVEOUT		m_hwo = 0;
char			*m_pcBuf = 0;
DUH_SIGRENDERER *m_pdr = 0;
DUH				*m_ModuleFile = 0;


	void CModulePlayerThread::SetVolume(float fVolume)
	{
		m_fVolume = fVolume;
		TRACE("CModulePlayerThread::SetVolume(%f)\n", fVolume);
	}

	static DWORD CALLBACK 

protected:


	// Thread-Funktion
};

DWORD ThreadProc()
{
	int iBuf;
	long l;

	// WaveHeader initialisieren
	for(iBuf=0; iBuf<NUM_BUFFERS; iBuf++)
	{
		ZeroMemory(&m_whdr[iBuf], sizeof(WAVEHDR));
		m_whdr[iBuf].dwBufferLength = NUM_SAMPLES*SAMPLE_SIZE;
		m_whdr[iBuf].lpData = m_pcBuf + iBuf*NUM_SAMPLES*SAMPLE_SIZE;
		waveOutPrepareHeader(m_hwo, &m_whdr[iBuf], sizeof(WAVEHDR));
		m_whdr[iBuf].dwFlags |= WHDR_DONE;
	}

	// Hauptschleife
	iBuf = 0;
	while(m_bStopThread==FALSE)
	{
		// warten, bis dieser Puffer abgespielt wurde
		while((m_whdr[iBuf].dwFlags&WHDR_DONE)==0);

		// Puffer löschen
		ZeroMemory(m_pcBuf + iBuf*NUM_SAMPLES*SAMPLE_SIZE,
			NUM_SAMPLES*SAMPLE_SIZE);

		// Puffer mit Daten füllen
		l = duh_render(m_pdr, 16, 0, m_fVolume, 65536.0f / 44100.0f,
			NUM_SAMPLES, m_pcBuf+iBuf*NUM_SAMPLES*SAMPLE_SIZE);

		// Puffer in Warteschlange stellen
		if(l) waveOutWrite(m_hwo, &m_whdr[iBuf], sizeof(WAVEHDR));

		iBuf++; if(iBuf>=NUM_BUFFERS) iBuf = 0;
	}

	return 0;
}
*/

//-----------------------------------------------------------------------------
// sndInit
//
// Sound (MikMod) gets initialized here. Don't forget to call DeInitSound()
// when sound is no longer needed.
//
// -> iHighQuality: if !=0, MikMod is initialized with high quality mixing,
//                  that is, with flags DMODE_INTERP and DMODE_HQMIXER
// <- 0: OK
//    1: there was an error
//-----------------------------------------------------------------------------
int sndInit(int iHighQuality)
{
/*
	dumb_register_stdfiles();
*/
	return 0;
}

//-----------------------------------------------------------------------------
// sndDeinit
//
// Clean up everything related to sound output
//
// -> --
// <- --
//-----------------------------------------------------------------------------
void sndDeInit(void)
{
/*
	int i;

	// stop thread
	if(m_hThread)
	{
		m_bStopThread = TRUE;
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread = NULL;
	}

	// stop WaveOut
	if(m_hwo)
	{
		for(i=0; i<NUM_BUFFERS; i++)
			waveOutUnprepareHeader(m_hwo, &m_whdr[i], sizeof(WAVEHDR));

		waveOutClose(m_hwo);
		m_hwo = NULL;
	}

	// stop DUMB
	if(m_pdr)
	{
		duh_end_sigrenderer(m_pdr);
		m_pdr = NULL;
	}

	if(m_ModuleFile)
	{
		unload_duh(m_ModuleFile);
		m_ModuleFile = NULL;
	}

	if(m_pcBuf) delete m_pcBuf;
	
	dumb_exit();
*/
}

//-----------------------------------------------------------------------------
// sndPlayMusic
//
// Load and play a module with MikMod
//
// -> cFilename = file to play (e. g. "ms0:/MUSIC.IT")
// <- 0: OK
//    1: there was an error
//-----------------------------------------------------------------------------
int sndPlayMusic(char *cFilename)
{
	
	m_Player.Start(cFilename);
/*
	WAVEFORMATEX wfx;

	m_fVolume = 1.0f;

	// load file
	m_ModuleFile = dumb_load_it(cFilename);
	if(m_ModuleFile==NULL) return 1;

	// start music renderer
	m_pdr = duh_start_sigrenderer(m_ModuleFile, 0, 2, 0);
	if(m_pdr==NULL) return 1;

	// setup buffer
	if(m_pcBuf==NULL)
	{
		m_pcBuf = new char[NUM_SAMPLES*NUM_BUFFERS*SAMPLE_SIZE];
		if(m_pcBuf==NULL) return 1;
	}

	// start WaveOut
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nChannels = 2;
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

	if(waveOutOpen(&m_hwo, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL)
		!=MMSYSERR_NOERROR) return 1;

	// setup thread
	m_hThread = CreateThread(NULL, NULL, ThreadProc,
		(void*)this, NULL, &m_dwThreadID);
	if(m_hThread==NULL) return 1;
*/
	return 0;
}

//-----------------------------------------------------------------------------
// sndStopMusic
//
// Stop and unload the currently playing module
//
// -> --
// <- --
//-----------------------------------------------------------------------------
void sndStopMusic(void)
{
	m_Player.Stop();
}

#endif	// WIN32
