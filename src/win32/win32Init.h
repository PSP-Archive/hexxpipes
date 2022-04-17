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
// # win32Init.h                                                              #
// #                                                                          #
// ############################################################################ 

#ifdef WIN32
#ifndef _WIN32_INIT_H_
#define _WIN32_INIT_H_

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "../common/gfxcore.h" 
//#include <d3d9.h>
//#include <d3d9caps.h>
//#include <d3d9types.h>

//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void prepareData();
void tileTexture(GFXIMAGE img_in,int tile_x, int tile_y,GFXTILEDTEXTURE* g_gfxBGtex,float rot_x,float rot_y,float rot_z);
void initopengl(void);
void test_rotate(int millisecs);
void displayopengl(void);
void cleanup();
int InitApplication(int argc, char** argv);


#endif	// _WIN32_INIT_H_
#endif	// WIN32
