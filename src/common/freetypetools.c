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
// # This is a set of functions to make the use of FreeType2 easier.          #
// #                                                                          #
// #                                                                          #
// ############################################################################ 

#include <stdio.h>
#include "freetypetools.h"

//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------
FT_Library m_ftLibrary = 0;

//------------------------------------------------------------------------------
// ftInit
//
// Initializes the FreeType2 engine
//
// -> --
// <- 0: OK
//    >0: error code from FreeType
//------------------------------------------------------------------------------
int ftInit(void)
{
	return (int)FT_Init_FreeType(&m_ftLibrary);
}

//------------------------------------------------------------------------------
// ftLoadFont
//
// Loads a given font into memory.
//
// -> cFontName = name and path to font file
// <- >0: handle to a font face
//    =0: error during load
//------------------------------------------------------------------------------
FONT* ftLoadFont(char *cFontName)
{
	FT_Error err;
	FILE *f;
	FONT *font;
	int iSize;
	
	// is library initialized?
	if(m_ftLibrary==0)
	{
		printf("ftLoadFont: Error: FreeType library not initialized.\n");
		return NULL;
	}

	// open the file for reading
	f = fopen(cFontName, "rb");
	if(!f)
	{
		printf("ftLoadFont: Error: Unable to open file.\n");
		return NULL;
	}
	
	// get the filesize	
	fseek(f, 0, SEEK_END);
	iSize = ftell(f);
	fseek(f, 0, SEEK_SET);
	if(!iSize)
	{
		fclose(f);
		printf("ftLoadFont: Error: File size = 0.\n");
		return NULL;
	}
	
	// allocate some memory
	font = (FONT*)malloc(sizeof(FONT));
	if(!font)
	{
		fclose(f);
		printf("ftLoadFont: Error: malloc(%i) for FONT* structure failed.\n",
			sizeof(FONT));
		return NULL;
	}
	
	font->iSize = iSize; font->face = 0; font->previous = 0;
	font->pBuf = (unsigned char*)malloc(iSize);
	if(!font->pBuf)
	{
		fclose(f);
		printf("ftLoadFont: Error: malloc(%i) for font buffer failed.\n", iSize);
		return NULL;
	}

	// load file into memory
	fread(font->pBuf, iSize, 1, f);
	fclose(f);
	
	// load font
	err = FT_New_Memory_Face(m_ftLibrary, font->pBuf, font->iSize, 0, &font->face);
	if(err)
	{
		printf("ftLoadFont: Error: FT_New_Memory_Face(0x%08X, 0x%08X, %i, 0, 0x%08X) failed.\n",
			(unsigned int)m_ftLibrary, (unsigned int)font->pBuf, font->iSize, (unsigned int)&font->face);
		return NULL;
	}

	printf("ftLoadFont(\"%s\")=%0x08X OK.\n", cFontName, (unsigned int)font);
	return font;
}

//------------------------------------------------------------------------------
// ftFreeFont
//
// Frees a given font.
//
// -> f = a loaded font face
// <- >0: error
//    =0: OK
//------------------------------------------------------------------------------
int ftFreeFont(FONT *f)
{
	FT_Error err;
	
	// check all the pointers
	if(!f)
	{
		printf("ftFreeFont: f is NULL.\n");
		return 1;
	}
	if(!f->pBuf)
	{
		printf("ftFreeFont: f->pBuf is NULL.\n");
		return 1;
	}
	if(!f->face)
	{
		printf("ftFreeFont: f->face is NULL.\n");
		return 1;
	}
	
	// is library initialized?
	if(m_ftLibrary==0)
	{
		printf("ftFreeFont: Error: FreeType library not initialized.\n");
		return 1;
	}

	// free font
	err = FT_Done_Face(f->face);
	if(err)
	{
		printf("ftFreeFont: Error: FT_Done_Face(0x%08X) returned %i.\n",
			(int)f->face, err);
		return 1;
	}
	
	// free buffers
	free(f->pBuf);
	free(f);
	
	return 0;
}

//------------------------------------------------------------------------------
// ftRenderText
//
// Renders a string to a buffer. Buffer has to be RGBA 32 bits format.
//
// -> cText = pointer to text
//    font = a pointer to a loaded font (use ftLoadFont)
//    iSize = desired height in pixels
//    uDestBuf = buffer to receive the rendered text
//    xpos, ypos = position of the text; the character will be drawn from
//                 ypos into the direction of decreasing y-values!
//    iLineLength = length (in DWORDs) of one line in your destination buffer
//    iNumLines = number of lines in your destination buffer
//    uColor = color (32 bits R8G8B8A8, use A = 0xFF for opaque text or
//             e. g. A = 0x20 for very light text)
// <- --
//------------------------------------------------------------------------------
void ftRenderText(char *cText, FONT *font, int iSize, unsigned int *uDestBuf,
	int x, int y, int iLineLength, int iNumLines, unsigned int uColor)
{
	char c;
	
	font->previous = 0;
	if(!cText) return;
	
	// loop through all characters
	while((c=*cText++))
	{
		x += ftRenderChar(c, font, iSize, uDestBuf, x, y, iLineLength, 
						  iNumLines, uColor);
	}
}

//------------------------------------------------------------------------------
// ftRenderChar
//
// Renders a character to a buffer. Buffer has to be RGBA 32 bits format.
// This drawing routine is not optimized for speed.
//
// -> c = character to be rendered
//    font = a pointer to a loaded font (use ftLoadFont)
//    iSize = desired height in pixels
//    uDestBuf = buffer to receive the rendered character
//    xpos, ypos = position of the character; the character will be drawn from
//                 ypos into the direction of decreasing y-values!
//    iLineLength = length (in DWORDs) of one line in your destination buffer
//    iNumLines = number of lines in your destination buffer
//    uColor = color (32 bits R8G8B8A8, use A = 0xFF for opaque characters or
//             e. g. A = 0x20 for very light characters)
// <- >0: advance in x direction (something like the width of one character)
//    =0: error
//------------------------------------------------------------------------------
int ftRenderChar(char c, FONT *font, int iSize, unsigned int *uDestBuf,
	int xpos, int ypos, int iLineLength, int iNumLines, 
	unsigned int uColor)
{
	int r1, g1, b1, a1, ColorR, ColorG, ColorB, ColorA, alpha, r, g, b, addr;
	FT_GlyphSlot slot = font->face->glyph;
	unsigned char *ucLine, *ucFontBuf;
	FT_UInt glyph_index;
	unsigned int u;
	FT_Error err;
	int x, y;

	// check pointers
	if(!font)
	{
		printf("ftRenderChar: Error: font is NULL.\n");
		return 0;
	}
	
	if(!(font->face))
	{
		printf("ftRenderChar: Error: font->face is NULL.\n");
		return 0;
	}
	
	// set size of the character to be rendered
	err = FT_Set_Pixel_Sizes(font->face, 0, iSize);
	if(err)
	{
		printf("ftRenderChar: FT_SetPixel_Sizes(0x%08X, 0, %i) returned %i\n",
			(int)font->face, iSize, err);
		return 0;
	}

	// process kerning information if necessary
	glyph_index = FT_Get_Char_Index(font->face, c);
	
	if(font->previous && glyph_index)
	{
		FT_Vector delta;
		FT_Get_Kerning(font->face, font->previous, glyph_index,
			FT_KERNING_DEFAULT, &delta);
		xpos += delta.x >> 6;
	}
	font->previous = glyph_index;	

	// load and render the desired character
	err = FT_Load_Glyph(font->face, glyph_index, FT_LOAD_RENDER);
	if(err)
	{
		printf("ftRenderChar: FT_Load_Glyph(0x%08X, %i, FT_LOAD_RENDER) returned %i\n",
			(int)font->face, glyph_index, err);
		return 0;
	}

	// separate the color channels
	ColorR = (uColor>>24) & 0xFF;
	ColorG = (uColor>>16) & 0xFF;
	ColorB = (uColor>> 8) & 0xFF;
	ColorA = (uColor>> 0) & 0xFF;

	ucLine = slot->bitmap.buffer;

	// loop through all lines of the rendered bitmap
	for(y=0; y<slot->bitmap.rows; y++)
	{
		// get the start of a line
		ucFontBuf = ucLine;
		
		// calculate destination address
		#ifdef PSP
		addr = (y-slot->bitmap_top+ypos)*iLineLength + xpos + slot->bitmap_left;
		#else
		addr = ((iNumLines-1-y-ypos)+slot->bitmap_top)*iLineLength + xpos
			 + slot->bitmap_left;
		#endif
		
		// check if we are out of bounds
		if(addr<0) continue;
		if(addr>iNumLines*iLineLength) continue;
		
		// loop through all x values
		for(x=0; x<slot->bitmap.width; x++)
		{
			// check if end of line is reached
			if((xpos + x + slot->bitmap_left)>=iLineLength) continue;
			
			// fetch one pixel (grayscale alpha value) from freetype buffer
			alpha = ((int)(*ucFontBuf++) * ColorA)>>8;

			// fetch the background pixel of the destination buffer
			u = uDestBuf[addr];
			r1 = (u>> 0) & 0xFF;
			g1 = (u>> 8) & 0xFF;
			b1 = (u>>16) & 0xFF;
			a1 = (u>>24) & 0xFF;
	
			// combine the background pixel and the character pixel color
			r = ((ColorR * alpha)>>8) + ((r1 * (255-alpha))>>8);
			g = ((ColorG * alpha)>>8) + ((g1 * (255-alpha))>>8);
			b = ((ColorB * alpha)>>8) + ((b1 * (255-alpha))>>8);
			
			// write back the result
			uDestBuf[addr++] = (a1 << 24) |
							   (b  << 16) |
						       (g  <<  8) |
							   (r  <<  0);

		}
		
		// advance to next line in source buffer
		ucLine += slot->bitmap.pitch;
	}

	return slot->advance.x >> 6;
}
