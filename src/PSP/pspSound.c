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
// # This file contains routines to play music and sound fx on a PSP through  #
// # MikMod.                                                                  #
// #                                                                          #
// ############################################################################ 
#ifdef PSP

#include <pspkernel.h>

#include "pspSound.h"
#include "../common/messagetext.h"

//-----------------------------------------------------------------------------
// external globals
//-----------------------------------------------------------------------------
extern int _mm_errno;
extern BOOL _mm_critical;
extern char *_mm_errmsg[];
extern UWORD md_mode;
extern UBYTE md_reverb;
extern UBYTE md_pansep;

//-----------------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------------
MODULE *m_ModuleFile = 0;
int m_mikModThreadID = 0;
int m_ExitAudioChannelThread = 0;

//-----------------------------------------------------------------------------
// AudioChannelThread
//
// This is a separate thread which is doing the sound updates. It calls
// MikModUpdate() periodically to render and output the current sound.
//
// -> args, argp: not used here
// <- return code (always 0)
//-----------------------------------------------------------------------------
static int AudioChannelThread(SceSize args, void *argp)
{
	while(!m_ExitAudioChannelThread)
	{
		MikMod_Update();
		sceKernelDelayThread(1);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// MyMikmodErrorHandler
//
// This is a callback function which is called by Mikmod in case of any error.
// It copies the error, its code and its textual description to the global
// error string.
//
// -> --
// <- --
//-----------------------------------------------------------------------------
void MyMikmodErrorHandler(void)
{
	sprintf(g_cErrorString, "MikMod: _mm_critical %d\n_mm_errno %d\n%s\n",
		MikMod_critical, MikMod_errno, MikMod_strerror(MikMod_errno));
	return;
}

//-----------------------------------------------------------------------------
// sndInit
//
// Sound (MikMod) gets initialized here. Don't forget to call DeInitSound()
// when sound is no longer needed.
//
// -> iHighQuality: if !=0, MikMod is initialized with high quality mixing,
//                  that is, with flags DMODE_INTERP and DMODE_HQMIXER
//                  WARNING: This will eat almost all CPU power and is unlikely
//                  to work
// <- 0: OK
//    1: there was an error, check g_cErrorString for more info
//-----------------------------------------------------------------------------
int sndInit(int iHighQuality)
{
	m_ExitAudioChannelThread = 0;

	// setup Mikmod
	MikMod_RegisterErrorHandler(MyMikmodErrorHandler);
	MikMod_RegisterAllDrivers();
	MikMod_RegisterAllLoaders();

	// set some global MikMod variables and initialize MikMod
	md_mode = DMODE_16BITS|DMODE_STEREO|DMODE_SOFT_SNDFX|DMODE_SOFT_MUSIC;
	if(iHighQuality) md_mode |= DMODE_INTERP|DMODE_HQMIXER; 
	md_reverb = 0;
	md_pansep = 128;
	if(MikMod_Init(""))
	{
		sprintf(g_cErrorString, MSGTXT("SoundInitFailed"),
			MikMod_strerror(MikMod_errno));
		return 1;
	}
	
	MikMod_EnableOutput();

	// create the background music thread
	if((m_mikModThreadID = sceKernelCreateThread("MikModThread",
		AudioChannelThread, 0x12, 0x10000, 0, NULL)) > 0)
	{
		sceKernelStartThread(m_mikModThreadID, 0 , NULL);
	}
	else
	{
		printf(MSGTXT("SoundThreadFailed"));
		return 1;
	}

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
	MikMod_DisableOutput();

	// signal the audio channel thread to end	
	m_ExitAudioChannelThread = 1;
	
	// terminate the audio channel thread, wait for timeout if necessary
	if(m_mikModThreadID > 0)
	{
		SceUInt timeout = 100000;
		sceKernelWaitThreadEnd(m_mikModThreadID, &timeout);
		sceKernelDeleteThread(m_mikModThreadID);
	}
	
	// stop MikMod
	Player_Stop();
	if(m_ModuleFile) Player_Free(m_ModuleFile);
	MikMod_Exit();
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
	// load file                          MaxChannels
	m_ModuleFile = Player_Load(cFilename, 128, 0);

	// play file
	if (NULL != m_ModuleFile)
	{
		m_ModuleFile->wrap = 1; // loop module after it finished
		Player_Start(m_ModuleFile);
		return 0;
	}
	else
	{
		return 1;
	}
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
	if(NULL != m_ModuleFile)
	{
		Player_Stop();
		Player_Free(m_ModuleFile); 
		m_ModuleFile = NULL;
	}
}

#endif	// PSP
