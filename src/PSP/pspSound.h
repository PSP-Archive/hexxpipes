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
// # pspSound.h                                                               #
// #                                                                          #
// ############################################################################ 

#ifdef PSP
#ifndef _PSP_SOUND_H_
#define _PSP_SOUND_H_

#include <mikmod.h>

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


#endif	// _PSP_SOUND_H_
#endif	// PSP

