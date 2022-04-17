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
// # messagetext.h                                                            #
// #                                                                          #
// ############################################################################ 

#ifndef _MESSAGETEXT_H_
#define _MESSAGETEXT_H_

#include <string.h>
#include <malloc.h>
#include <stdio.h>

#define MAX_MSG_LEN 4096

//-----------------------------------------------------------------------------
// structures
//-----------------------------------------------------------------------------
typedef struct _MSG_TXT {
	char* pcMessage;
	char* pcName;
	void *pNext;
} MSG_TXT;

//-----------------------------------------------------------------------------
// variables
//-----------------------------------------------------------------------------
extern char g_cErrorString[MAX_MSG_LEN];

//-----------------------------------------------------------------------------
// prototypes
//-----------------------------------------------------------------------------

int Msg_SetLanguage(int iLanguage);
char* Msg_GetMessage(char* pcMessageID, char* pcSourceFile, int iSourceLine);
int Msg_Init(char* pcFilename);
int Msg_DeInit(void);
#define MSGTXT(ID) Msg_GetMessage(ID, __FILE__, __LINE__)

#endif	// _MESSAGETEXT_H_

