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
// # win32Sound.h                                                             #
// #                                                                          #
// ############################################################################ 

#ifdef WIN32
#ifndef _WIN32_SOUND_H_
#define _WIN32_SOUND_H_

typedef struct _SAMPLE {
	char *cSampleName;
	int iSampleFreq;		// sampling frequency, e. g. 44100
	int iSampleChannels;	// number of channels (1 or 2)
	int iSampleWordsize;	// 1 (8 bits) or 2 (16 bits)
	int iSampleCount;		// number of samples (not bytes!)
	char *cSampleBuf;		// pointer to sampledata
} SAMPLE;

//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
int sndInit(int iHighQuality);
void sndDeInit(void);
int sndPlayMusic(char *cFilename);
void sndStopMusic(void);
SAMPLE* sndLoadSample();
void sndFreeSample(SAMPLE *s);
int sndPlaySample(SAMPLE *s);


#endif	// _WIN32_SOUND_H_
#endif	// WIN32
