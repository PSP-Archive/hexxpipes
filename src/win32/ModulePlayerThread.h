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
// # This file contains a class which handles music output in a Win32         #
// # environment.                                                             #
// #                                                                          #
// ############################################################################ 

#include <dumb/dumb.h>
#include <mmsystem.h>

class CModulePlayerThread
{
public:

#define NUM_SAMPLES 8192
#define NUM_BUFFERS	16
#define SAMPLE_SIZE	4

	// constructor
	CModulePlayerThread::CModulePlayerThread()
	{
		m_hwo = NULL;
		m_pdr = NULL;
		m_pDUH = NULL;
		m_pcBuf = NULL;
		m_hThread = NULL;
		m_dwThreadID = 0;
		m_bStopThread = FALSE;
		dumb_register_stdfiles();
	}

	// destructor
	CModulePlayerThread::~CModulePlayerThread()
	{
		CModulePlayerThread::Stop();

		if(m_pcBuf) delete m_pcBuf;

		dumb_exit();
	}

	void CModulePlayerThread::Stop()
	{
		int i;

		// stop thread
		if(m_hThread)
		{
			m_bStopThread = TRUE;
			WaitForSingleObject(m_hThread, INFINITE);
			m_hThread = NULL;
		}

		// stop waveOut 
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

		if(m_pDUH)
		{
			unload_duh(m_pDUH);
			m_pDUH = NULL;
		}
	}

	void CModulePlayerThread::SetVolume(float fVolume)
	{
		m_fVolume = fVolume;
		TRACE("CModulePlayerThread::SetVolume(%f)\n", fVolume);
	}

	int CModulePlayerThread::Start(char *cFile)
	{
		WAVEFORMATEX wfx;

		m_fVolume = 1.0f;

		// load file
		m_pDUH = dumb_load_it(cFile);
		if(m_pDUH==NULL) return 0;

		// start renderer
		m_pdr = duh_start_sigrenderer(m_pDUH, 0, 2, 0);
		if(m_pdr==NULL) return 0;

		// create buffer
		if(m_pcBuf==NULL)
		{
			m_pcBuf = new char[NUM_SAMPLES*NUM_BUFFERS*SAMPLE_SIZE];
			if(m_pcBuf==NULL) return 0;
		}

		// start waveOut
		wfx.nSamplesPerSec = 44100;
		wfx.wBitsPerSample = 16;
		wfx.nChannels = 2;
		wfx.cbSize = 0;
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
		wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

		if(waveOutOpen(&m_hwo, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL)
			!=MMSYSERR_NOERROR) return 0;

		// start background thread
		m_hThread = CreateThread(0, 0, s_ThreadProc,
			(void*)this, 0, &m_dwThreadID);
		if(m_hThread==NULL) return 0;

		return 1;
	}

	static DWORD CALLBACK s_ThreadProc(LPVOID lpParameter)
	{
		return ((CModulePlayerThread*)lpParameter)->ThreadProc();
	}

protected:

	float			m_fVolume;
	DWORD			m_dwThreadID;
	HANDLE			m_hThread;
	BOOL			m_bStopThread;
	WAVEHDR			m_whdr[NUM_BUFFERS];
	HWAVEOUT		m_hwo;
	char			*m_pcBuf;
	DUH				*m_pDUH;
	DUH_SIGRENDERER *m_pdr;

	// background thread function
	DWORD CModulePlayerThread::ThreadProc()
	{
		int iBuf;
		long l;

		// init WaveHeader
		for(iBuf=0; iBuf<NUM_BUFFERS; iBuf++)
		{
			ZeroMemory(&m_whdr[iBuf], sizeof(WAVEHDR));
			m_whdr[iBuf].dwBufferLength = NUM_SAMPLES*SAMPLE_SIZE;
			m_whdr[iBuf].lpData = m_pcBuf + iBuf*NUM_SAMPLES*SAMPLE_SIZE;
			waveOutPrepareHeader(m_hwo, &m_whdr[iBuf], sizeof(WAVEHDR));
			m_whdr[iBuf].dwFlags |= WHDR_DONE;
		}

		// main loop
		iBuf = 0;
		while(m_bStopThread==FALSE)
		{
			// wait until buffer is finished
			while((m_whdr[iBuf].dwFlags&WHDR_DONE)==0);

			// clear buffer
			ZeroMemory(m_pcBuf + iBuf*NUM_SAMPLES*SAMPLE_SIZE,
				NUM_SAMPLES*SAMPLE_SIZE);

			// fill buffer with music data
			l = duh_render(m_pdr, 16, 0, m_fVolume, 65536.0f / 44100.0f,
				NUM_SAMPLES, m_pcBuf+iBuf*NUM_SAMPLES*SAMPLE_SIZE);

			// add buffer to queue
			if(l) waveOutWrite(m_hwo, &m_whdr[iBuf], sizeof(WAVEHDR));

			iBuf++; if(iBuf>=NUM_BUFFERS) iBuf = 0;
		}

		return 0;
	}
};
