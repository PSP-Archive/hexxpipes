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
// # freetypetools.h                                                          #
// #                                                                          #
// ############################################################################ 

#ifndef _FREETYPETOOLS_H_
#define _FREETYPETOOLS_H_

#include <ft2build.h>
#include FT_FREETYPE_H

//-----------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// structs
//-----------------------------------------------------------------------------
typedef struct {
	FT_Face face;
	unsigned char *pBuf;
	int iSize;
	FT_UInt previous;
} FONT;

//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
int ftInit(void);
FONT* ftLoadFont(char *cFontName);
int ftRenderChar(char c, FONT *font, int iSize, unsigned int *uDestBuf, int xpos, int ypos, int iLineLength, int iNumLines, unsigned int uColor);
void ftRenderText(char *cText, FONT *font, int iSize, unsigned int *uDestBuf, int x, int y, int iLineLength, int iNumLines, unsigned int uColor);
int ftFreeFont(FONT *f);

#endif	// _FREETYPETOOLS_H_

