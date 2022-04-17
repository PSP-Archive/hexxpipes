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
// # This file contains PSP specific initialization routines.                 #
// #                                                                          #
// #                                                                          #
// ############################################################################ 
#ifdef PSP

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <pspctrl.h>
#include <pspgu.h> 
#include <pspgum.h> 

#include "../common/freetypetools.h"
#include "pspSound.h"
#include "../common/gfxcore.h"

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272) 

struct Vertex
{
	float u, v;
	unsigned int color;
	float x,y,z;
};

extern unsigned char logo_start[]; 

#define NUM_VERTICES 12*3
struct Vertex __attribute__((aligned(16))) vertices[NUM_VERTICES] =
{
	{0, 0, 0x807f0000,-1,-1, 1}, // 0
	{1, 0, 0x807f0000,-1, 1, 1}, // 4
	{1, 1, 0x807f0000, 1, 1, 1}, // 5

	{0, 0, 0x807f0000,-1,-1, 1}, // 0
	{1, 1, 0x807f0000, 1, 1, 1}, // 5
	{0, 1, 0x807f0000, 1,-1, 1}, // 1

	{0, 0, 0x807f0000,-1,-1,-1}, // 3
	{1, 0, 0x807f0000, 1,-1,-1}, // 2
	{1, 1, 0x807f0000, 1, 1,-1}, // 6

	{0, 0, 0x807f0000,-1,-1,-1}, // 3
	{1, 1, 0x807f0000, 1, 1,-1}, // 6
	{0, 1, 0x807f0000,-1, 1,-1}, // 7

	{0, 0, 0x80007f00, 1,-1,-1}, // 0
	{1, 0, 0x80007f00, 1,-1, 1}, // 3
	{1, 1, 0x80007f00, 1, 1, 1}, // 7

	{0, 0, 0x80007f00, 1,-1,-1}, // 0
	{1, 1, 0x80007f00, 1, 1, 1}, // 7
	{0, 1, 0x80007f00, 1, 1,-1}, // 4

	{0, 0, 0x80007f00,-1,-1,-1}, // 0
	{1, 0, 0x80007f00,-1, 1,-1}, // 3
	{1, 1, 0x80007f00,-1, 1, 1}, // 7

	{0, 0, 0x80007f00,-1,-1,-1}, // 0
	{1, 1, 0x80007f00,-1, 1, 1}, // 7
	{0, 1, 0x80007f00,-1,-1, 1}, // 4

	{0, 0, 0x8000007f,-1, 1,-1}, // 0
	{1, 0, 0x8000007f, 1, 1,-1}, // 1
	{1, 1, 0x8000007f, 1, 1, 1}, // 2

	{0, 0, 0x8000007f,-1, 1,-1}, // 0
	{1, 1, 0x8000007f, 1, 1, 1}, // 2
	{0, 1, 0x8000007f,-1, 1, 1}, // 3

	{0, 0, 0x8000007f,-1,-1,-1}, // 4
	{1, 0, 0x8000007f,-1,-1, 1}, // 7
	{1, 1, 0x8000007f, 1,-1, 1}, // 6

	{0, 0, 0x8000007f,-1,-1,-1}, // 4
	{1, 1, 0x8000007f, 1,-1, 1}, // 6
	{0, 1, 0x8000007f, 1,-1,-1}, // 5
};







//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
PSP_MODULE_INFO("hexxpipes", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
int g_ExitApplication = 0;	// global exit flag, app runs until this is != 0

void* framebuffer = 0;
static unsigned int __attribute__((aligned(16))) list[262144];
static unsigned int __attribute__((aligned(16))) pixels[512*272];

#define printf	pspDebugScreenPrintf 

//------------------------------------------------------------------------------
// Exit callback
//
// Gets called when the system wants to terminate our program
//
// -> arg1, arg2, common: unused
// <- always 0
//------------------------------------------------------------------------------
int exit_callback(int arg1, int arg2, void *common)
{
	// set our global exit flag
	g_ExitApplication = 1;
	return 0;
}

//------------------------------------------------------------------------------
// Callback thread
//
// -> SceSize, argp: unused
// <- always 0
//------------------------------------------------------------------------------
int CallbackThread(SceSize args, void *argp)
{
	int cbid;
	
	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	
	sceKernelSleepThreadCB();
	
	return 0;
}

//------------------------------------------------------------------------------
// SetupCallbacks
//
// Sets up the callback thread and returns its thread id
//
// -> --
// <- id of callback thread 
//------------------------------------------------------------------------------
int SetupCallbacks(void)
{
	int thid = 0;
	
	// create thread
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11,
		0xFA0, 0, 0);

	// start thread
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}
	
	return thid;
}

void DisplaySomething(char *cFilename, FT_Face f)
{
	unsigned int x = 0, y = 0;
	long unsigned int width, height;
	unsigned char *backgroundpic;
	
	gfxLoadJPEG(cFilename, &width, &height, &backgroundpic);
	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pixels[x + y*512] = ((unsigned int*)backgroundpic)[x + y*width];
		}
	}
	free(backgroundpic);

	ftRenderText("AlphaText 25%", f, 75, pixels, 5, 120, 512, 272, 0x00000040);

	ftRenderText("Sample text: r1 = (u>>0) & 0xFF; g1 = (u>> 8) & 0xFF; b1 = (u>>16) & "
		"0xFF; a1 = (u>>24) & 0xFF;", f, 12, pixels, 5, 250, 512,272, 0x000000FF);
	ftRenderText(cFilename, f, 12, pixels, 5, 17, 512, 272, 0x000080FF);

	sceKernelDcacheWritebackAll();

//	sceGuStart(GU_DIRECT,list);

	// copy image from ram to vram
//	sceGuCopyImage(GU_PSM_8888,0,0,480,272,512,pixels,0,0,512,(void*)(0x04000000+(u32)framebuffer));

//	sceGuFinish();
//	sceGuSync(0,0);

//	sceDisplayWaitVblankStart();
//	framebuffer = sceGuSwapBuffers();

}

//-----------------------------------------------------------------------------
// InitApplication
//
// Initialize the app, setup threads etc.
//
// -> --
// <- 0: OK
//    1: Error
//-----------------------------------------------------------------------------
int InitApplication(int argc, char** argv)
{
	FT_Face f;
	ftInit();
	f = ftLoadFont("gara.ttf");
	char cText[128];
	int val = 0;
	int iPicture = 1;
	int iFPS = 0;
	time_t tLastTime = 0, tTime = 0;
	SceCtrlData pad, lastpad;

	pspDebugScreenInit();
	SetupCallbacks();

	sndInit(0);
	sndPlayMusic("ms0:/AA_ION.IT");

	sceGuInit();

	// setup
	sceGuStart(GU_DIRECT,list);

	sceGuDrawBuffer(GU_PSM_8888,(void*)0,512);
	sceGuDispBuffer(480,272,(void*)0x088000,512);
	sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
//	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH); 
//	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH); 
//	sceGuDepthBuffer(zbp,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT); 
	sceGuDepthRange(65535,0); 
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT); 
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST); 
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0,0);
 
//	sceGuDrawBuffer(GU_PSM_8888,(void*)0,512);
//	sceGuDispBuffer(480,272,(void*)0x88000,512);
//	sceGuDepthBuffer((void*)0x110000,512);
//	sceGuOffset(2048 - (480/2),2048 - (272/2));
//	sceGuViewport(2048,2048,480,272);
//	sceGuDepthRange(0xc350,0x2710);
//	sceGuScissor(0,0,480,272);
//	sceGuEnable(GU_SCISSOR_TEST);
//	sceGuFrontFace(GU_CW);
//	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
//	sceGuFinish();
//	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(1);
	
	sceCtrlReadBufferPositive(&lastpad, 1);

	DisplaySomething("hexxbg00.jpg", f);
	
	do
	{
		iFPS++;
		sceKernelLibcTime(&tTime);
		if(tTime!=tLastTime)
		{
			sprintf(cText, "%i fps", iFPS);
			tLastTime = tTime;
			iFPS = 0;
		}
		
		sceGuStart(GU_DIRECT,list);

		// clear screen
		// copy image from ram to vram
		sceGuCopyImage(GU_PSM_8888,0,0,480,272,512,pixels,0,0,512,(void*)(0x04000000+(u32)framebuffer));
		
//		sceGuClearColor(0xff554433);
		sceGuClearDepth(0);
//		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
		sceGuClear(GU_DEPTH_BUFFER_BIT);

		ftRenderText(cText, f, 12, (void*)(0x04000000+(u32)framebuffer), 5, 32, 512, 272, 0x800000FF);
		

		// setup matrices for cube

		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		sceGumPerspective(75.0f,16.0f/9.0f,0.5f,1000.0f);

		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();

		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		{
			ScePspFVector3 pos = { 0, 0, -5.0f };
			ScePspFVector3 rot = { val * 0.79f * (GU_PI/180.0f), val * 0.98f * (GU_PI/180.0f), val * 1.32f * (GU_PI/180.0f) };
			sceGumTranslate(&pos);
			sceGumRotateXYZ(&rot);
		}

		// setup texture

		sceGuTexMode(GU_PSM_4444,0,0,0);
		sceGuTexImage(0,64,64,64,logo_start);
		sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
		sceGuTexEnvColor(0xffff00);
		sceGuTexFilter(GU_LINEAR,GU_LINEAR);
		sceGuTexScale(1.0f,1.0f);
		sceGuTexOffset(0.0f,0.0f);
		sceGuAmbientColor(0xffffffff);

		// draw cube

		sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,NUM_VERTICES,0,vertices);

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		framebuffer = sceGuSwapBuffers();

		val++; 
		
		sceCtrlReadBufferPositive(&pad, 1);
		
		if(pad.Buttons != lastpad.Buttons)
    	{
			if(pad.Buttons & PSP_CTRL_CROSS)
      		{
				switch(iPicture)
				{
					case 0:
						DisplaySomething("hexxbg00.jpg", f);
						iPicture++;
						break;
					case 1:
						DisplaySomething("hexxbg01.jpg", f);
						iPicture++;
						break;
					case 2:
						DisplaySomething("hexxbg02.jpg", f);
						iPicture++;
						break;
					case 3:
						DisplaySomething("hexxbg03.jpg", f);
						iPicture++;
						break;
					case 4:
						DisplaySomething("hexxbg04.jpg", f);
						iPicture++;
						break;
					case 5:
						DisplaySomething("hexxbg05.jpg", f);
						iPicture++;
						break;
					case 6:
						DisplaySomething("hexxbg06.jpg", f);
						iPicture++;
						break;
					case 7:
						DisplaySomething("hexxbg07.jpg", f);
						iPicture = 0;
						break;
					default:
						iPicture = 0;
						break;
				}
			}

			if(pad.Buttons & PSP_CTRL_SQUARE)
      		{
			}
			
			if(pad.Buttons & PSP_CTRL_CIRCLE)
			{
			}

			if(pad.Buttons & PSP_CTRL_TRIANGLE)
			{
			}
				
			if(pad.Buttons & PSP_CTRL_SELECT)
			{
			}
			lastpad = pad;
		}
				
		if(pad.Buttons & PSP_CTRL_LTRIGGER)
	    {
		}
		
	    if(pad.Buttons & PSP_CTRL_RTRIGGER)
	    {
		}
			
		if(pad.Buttons & PSP_CTRL_UP)
	    {
		}
		
	    if(pad.Buttons & PSP_CTRL_DOWN)
	    {
		}
			
		if(pad.Buttons & PSP_CTRL_LEFT)
	    {
		}
		
	    if(pad.Buttons & PSP_CTRL_RIGHT)
	    {
		}

		sceDisplayWaitVblankStart();
		
	} while(!((pad.Buttons & PSP_CTRL_START) || g_ExitApplication));


	sndStopMusic();
	sndDeInit();
	
	sceKernelExitGame();
	return 0; 
}


#endif	// PSP
