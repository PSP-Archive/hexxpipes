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
// # This is a set of functions for translation of messages into different    #
// # languages.                                                               #
// #                                                                          #
// ############################################################################ 

#include "messagetext.h"

// global variable for error translation
char g_cErrorString[MAX_MSG_LEN];


char m_cMsgText[MAX_MSG_LEN];
int m_iLanguage = 0;
int m_iInstalledLanguages = 2; 
MSG_TXT* m_pMsgRoot = 0;

//-----------------------------------------------------------------------------
// Msg_SetLanguage
//
// Set the current language [0...n]
//
// -> Language ID
// <- --
//-----------------------------------------------------------------------------
int Msg_SetLanguage(int iLanguage)
{
	if(iLanguage>=m_iInstalledLanguages) iLanguage = 0;
	m_iLanguage = iLanguage;
	return m_iLanguage;
}

//-----------------------------------------------------------------------------
// Msg_Init
//
// Load all messages
//
// -> pcFilename = name of the language file
// <- 0: OK
//    1: some error happened
//-----------------------------------------------------------------------------
int Msg_Init(char* pcFilename)
{
	FILE *f;
	char c, lastc, *cText = 0;
	long lFilePos, l;
	MSG_TXT *pMsg = 0, *pTemp = 0;
	
	printf("Msg_Init(\"%s\") executing...\n", pcFilename);
	f = fopen(pcFilename, "r");
	if(!f)
	{
		printf("Msg_Init: fopen(\"%s\") failed.\n", pcFilename);
		return 1;
	}
	
	while(!feof(f))
	{
		fread(&c, 1, 1, f);
		
		// skip comments
		if(c=='#')
		{
			do {
				fread(&c, 1, 1, f);
			} while((!feof(f))&&(c!='\n'));
		}
		
		else if(c=='[')
		{
			// calculate length of keyword
			lFilePos = ftell(f); l = 0;
			do {
				fread(&c, 1, 1, f);
				l++;
			} while((!feof(f))&&(c!=']'));
			
			// read keyword
			fseek(f, lFilePos, SEEK_SET);
			if(cText) free(cText);
			cText = (char*) malloc(l);
			printf("Msg_Init: malloc(%i) = 0x%08X\n", (int)l, (unsigned int)cText);
			fread(cText, l-1, 1, f);
			cText[l-1] = 0;
			
			// create new message list entry
			pTemp = (MSG_TXT*) malloc(sizeof(MSG_TXT));
			printf("Msg_Init: malloc(%i) = 0x%08X\n", sizeof(MSG_TXT), (unsigned int)pTemp);

			pTemp->pcMessage = 0;
			pTemp->pcName = (char*) malloc(l);
			printf("Msg_Init: malloc(%i) = 0x%08X\n", (int)l, (unsigned int)pTemp->pcName);
			strcpy(pTemp->pcName, cText);
			pTemp->pNext = 0;
			
			if(m_pMsgRoot==0)
			{
				// first entry
				m_pMsgRoot = pTemp;
			}
			else
			{
				// next entry
				pMsg->pNext = pTemp;
			}
			pMsg = pTemp;
		}
		else if(c=='{')
		{
			// calculate length of text
			lFilePos = ftell(f); l = 0; c = 0;
			do {
				lastc = c;
				fread(&c, 1, 1, f);
				l++;
			} while((!feof(f))&&(c!='}'));
			
			// read text
			fseek(f, lFilePos, SEEK_SET);
			if(cText) free(cText);
			cText = (char*) malloc(l+1);
			printf("Msg_Init: malloc(%i) = 0x%08X\n", (int)l+1, (unsigned int)cText);
			fread(cText, l-1, 1, f);
			cText[l-1] = '|'; cText[l] = 0;
			
			// copy text to message structure
			if(pMsg)
			{
				if(pMsg->pcMessage)
					free(pMsg->pcMessage);
					
				pMsg->pcMessage = (char*) malloc(l);
				printf("Msg_Init: malloc(%i) = 0x%08X\n", (int)l, (unsigned int)pMsg->pcMessage);
				strcpy(pMsg->pcMessage, cText);
			}
		}
	}
	
	fclose(f);
	
	if(cText) free(cText);
	
	printf("Msg_Init(\"%s\") finished successfully.\n\n", pcFilename);
	return 0;
}

//-----------------------------------------------------------------------------
// Msg_DeInit
//
// Clean up structures
//
// -> --
// <- 0: OK
//    1: some error happened
//-----------------------------------------------------------------------------
int Msg_DeInit(void)
{
	MSG_TXT *p, *ptemp;
	
	if(m_pMsgRoot)
	{
		// walk through list, free every entry
		p = m_pMsgRoot;
		while(p)
		{
			ptemp = (MSG_TXT*)p->pNext;
			if(p->pcMessage) free(p->pcMessage);
			if(p->pcName) free(p->pcName);
			free(p);
			p = ptemp;
		}
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// Msg_GetMessage
//
// INFO: Use the macro MSGTXT() for easy access to the messages
//
// Get the message n the currently selected language
//
// -> pcMessageID  = string identifier for the message
//    pcSourceFile = for debugging: file of the calling function
//    iSourceLine  = for debugging: line number of the calling function
//                   (use the macro MSGTXT() for easy access)
// <- pointer to Messagetext
//    Warning: The pointer points to a buffer inside of this module. It is
//             only valid until the next call of this function.
//-----------------------------------------------------------------------------
char* Msg_GetMessage(char* pcMessageID, char* pcSourceFile, int iSourceLine)
{
	MSG_TXT *p = m_pMsgRoot;
	int iLang, i, j;
	
	// initialized?
	if(m_pMsgRoot==0)
	{
		sprintf(m_cMsgText, "The message translation system was not correctly"
			" initialized (this function was called from %s, line %i).",
			pcSourceFile, iSourceLine);
		return m_cMsgText;
	}
	
	// loop through all messages
	while(p)
	{
		// message ID found?
		if(strcmp(p->pcName, pcMessageID)==0)
		{
			// Isolate the text in the selected language
			iLang = -1; i = 0; j = 0;
			do
			{
				m_cMsgText[j++] = p->pcMessage[i++];
				if(m_cMsgText[j-1]=='|')
				{
					iLang++;
					m_cMsgText[j-1] = 0;
					
					// is this the language we look for?
					if(iLang==m_iLanguage)
					{
						return m_cMsgText;
					}
					
					// if not, reset the current message buffer
					j = 0;
				}
				
			} while((i<(int)strlen(p->pcMessage))&&(j<MAX_MSG_LEN));
			
			// tell the user that the requested language could not be found
			sprintf(m_cMsgText, "In file %s, line %i, a message was to be "
				"translated, but the requested language could not be found.",
				pcSourceFile, iSourceLine);
			return m_cMsgText;
		}
		
		p = (MSG_TXT*)p->pNext;
	}

	// if the message could not be found, tell the user
	sprintf(m_cMsgText, "In file %s, line %i, the message \"%s\" was to be "
		"translated, but the requested message could not be found.",
		pcSourceFile, iSourceLine, pcMessageID);
	
	return m_cMsgText;
}

