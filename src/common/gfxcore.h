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
// # gfxcore.h                                                                #
// #                                                                          #
// ############################################################################ 

#ifndef _GFXCORE_H_
#define _GFXCORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define FREEGLUT_STATIC
#include <gl/freeglut.h>
#endif // WIN32

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#define XMD_H	// this resolves conflict between basestd.h and jmorecfg.h
#include <jpeglib.h>
#include <jerror.h>
#undef XMD_H

#include <png.h>

#ifdef WIN32	// mingw has no memalign
#define memalign(align, size) malloc(size)
#endif

//-----------------------------------------------------------------------------
// typedef
//-----------------------------------------------------------------------------
typedef struct GFXIMAGE {
	long unsigned int width;
    long unsigned int height;
	unsigned char* data;
} GFXIMAGE;

typedef struct Vertex
{
	float u, v;
	unsigned int color;
	float x,y,z;
} Vertex;
#ifdef WIN32
typedef struct GFXTILEDTEXTURE {
	GLuint texture;
	double tx;
	double ty;
	double tz;
	double ang_x;
	double ang_y;
	double ang_z;
	int delay_x;
	int delay_y;
	int delay_z;
} GFXTILEDTEXTURE;
#endif // WIN32

//-----------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------
#define GFXLOAD_SUCCESS 				0
#define GFXLOAD_READ_ERROR			 	1
#define GFXLOAD_NO_PNG_ERROR			2
#define GFXLOAD_MEMORY_ERROR 			3
#define GFXLOAD_BACKGROUNDCHUNK_ERROR 	4
#define GFXLOAD_ILLEGAL_POINTER		 	5
//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
int gfxLoadPNG(char *file, unsigned long *pulWidth, 
	unsigned long *pulHeight, unsigned char **image_data_ptr);
int gfxLoadJPEG(char *file, unsigned long *pulWidth, 
	unsigned long *pulHeight, unsigned char **image_data_ptr);
void gfxDrawTiledTexture(GFXTILEDTEXTURE* g_gfxBGtex_a,GFXTILEDTEXTURE* g_gfxBGtex_b);

#ifdef __cplusplus
}
#endif

#endif	// _GFXCORE_H_
