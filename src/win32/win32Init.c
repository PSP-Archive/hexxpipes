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
// # This file contains initialization routines for Win32.                    #
// #                                                                          #
// #                                                                          #
// ############################################################################ 

#include <windows.h>
//#define WIN32_PSPFRAME
#include "win32Init.h"
#include "win32Sound.h"
#include "win32Helper.h"
#include <math.h>
#include "../common/freetypetools.h"

#include "../common/gfxcore.h"

//---------------------------------------------------------------------------
// globale Variablen
#define WINDOW_NAME "HexxPipes"

HWND ghwnd;                             // globales Fenster-Handle
HWND mainhwnd;
//for OpenGL
HDC m_hDC;
HGLRC m_hRC;
char g_szClassName[] = "HexxPipesClass";
int tickcount=0;
int lasttickcount=0;

int cxScreen = 0,
    cyScreen = 0,
    WindowPosX = 0,
    WindowPosY = 0,
    WindowSizeX = 480,
    WindowSizeY = 272,
    xPosMouse,
    yPosMouse;


GFXIMAGE g_gfxBGimage;
GFXIMAGE g_gfxBGfximg_a;
GFXIMAGE g_gfxBGfximg_b;
GFXIMAGE g_gfxBGpsp;
HINSTANCE g_hInst=NULL;
FONT* g_ftFont=NULL;

GFXTILEDTEXTURE g_gfxBGtex_a[510];
GFXTILEDTEXTURE g_gfxBGtex_b[510];
#ifdef WIN32_PSPFRAME
int kWindowWidth = 848; /* window width */
int kWindowHeight = 400; /* window height */
HRGN g_CopyBGRgn=NULL;  
#else
int kWindowWidth = 480; /* window width */
int kWindowHeight = 272; /* window height */
#endif
bool dorotate=false;
double g_debug_x=0;

//------------------------------------------------------------------------------
// prepareData
//
// main data loading procedure
//
// -> 
// <- --
//------------------------------------------------------------------------------
void prepareData()
{
	//load font
	

	ftInit();
	g_ftFont = ftLoadFont("..\\gfx\\fonts\\gara.ttf");
	if(!g_ftFont) printf("Loading of font failed.\n");

	
	int result;
	long unsigned int width, height;
    char filename[]="..\\gfx\\backgrounds\\hexxbg02.jpg";
	
  	result = gfxLoadJPEG(filename, &g_gfxBGfximg_a.width, &g_gfxBGfximg_a.height, &g_gfxBGfximg_a.data);
//  	result = gfxLoadPNG(filename, &g_gfxBGimage.width, &g_gfxBGimage.height, &g_gfxBGimage.data);
  	if (result != 0) {
      printf("gfxLoadPNG %s FAILED.\n", filename);
    }

    char filename1[]="..\\gfx\\backgrounds\\hexxbg03.jpg";
  	result = gfxLoadJPEG(filename1, &g_gfxBGfximg_b.width, &g_gfxBGfximg_b.height, &g_gfxBGfximg_b.data);
//  	result = gfxLoadPNG(filename, &g_gfxBGimage.width, &g_gfxBGimage.height, &g_gfxBGimage.data);
  	if (result != 0) {
      printf("gfxLoadPNG %s FAILED.\n", filename1);
    }
	
	
#ifdef WIN32_PSPFRAME
	char filenamepspframe[]="..\\gfx\\backgrounds\\pspframe.png";
	result = gfxLoadPNG(filenamepspframe, &g_gfxBGpsp.width, &g_gfxBGpsp.height, &g_gfxBGpsp.data);
  	if (result != 0) {
      printf("gfxLoadPNG %s FAILED.\n", filename);
    }
	int iwcorx=GetSystemMetrics(SM_CXSIZEFRAME);
	int iwcory=GetSystemMetrics(SM_CXSIZEFRAME)+GetSystemMetrics(SM_CYCAPTION);
   	g_CopyBGRgn=CreateRectRgn(0, 0, 0, 0);
   	for (int y=0;y<g_gfxBGpsp.height;y++){
   		for (int x=0;x<g_gfxBGpsp.width;x++){
		      if ( ( g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+0]==0xff)
              && (g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+1]==0xff)
              && (g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+2]==0x00)) {
            // 
           }
           else {
		   		  HRGN rtmp = CreateRectRgn(x+iwcorx, g_gfxBGpsp.height-y-1+iwcory, iwcorx+x+1, g_gfxBGpsp.height-y-1+1+iwcory);
			  CombineRgn(g_CopyBGRgn,g_CopyBGRgn, rtmp, RGN_OR);
			  DeleteObject(rtmp);
		  }
            if ( ( g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+0]==0x00)
              && (g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+1]==0xff)
              && (g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+2]==0xff)) {
			   g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+0]=0x00;
               g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+1]=0x00;
               g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+2]=0x00;
               g_gfxBGpsp.data[4*x+(4*y*g_gfxBGpsp.width)+3]=0x00;
           }      
       }
    }
    HWND hwnd=FindWindow(  "FREEGLUT","hexxpipes");
	SetWindowRgn(hwnd,g_CopyBGRgn,true);
 #endif
	
	
	
	ftRenderText("AlphaText 25%", g_ftFont, 75,(unsigned int*) g_gfxBGfximg_a.data, 5, 120, g_gfxBGfximg_a.width, g_gfxBGfximg_a.height, 0x00000040);

	tileTexture(g_gfxBGfximg_a,30,17,g_gfxBGtex_a,0,0,0);
	tileTexture(g_gfxBGfximg_b,30,17,g_gfxBGtex_b,0,0,0);
}

//------------------------------------------------------------------------------
// gfxDrawTiledTexture()
//
// function for drawing a tiled texture
//
// TODO: should be in gfx core later w/o using global variables
//
// -> 
// <- --
//------------------------------------------------------------------------------
void tileTexture(GFXIMAGE img_in,int tile_x, int tile_y,GFXTILEDTEXTURE* g_gfxBGtex,float rot_x,float rot_y,float rot_z) {
	int width = img_in.width/tile_x;
	int height = img_in.height/tile_y;
	int num_tiles=tile_x*tile_y;
	double d_tx=-29.0;
#ifdef WIN32_PSPFRAME
	double d_ty=-14.0;
#else
	double d_ty=-16.0;
#endif
	double d_tz=0.0;
	
	for(int ty=0;ty<tile_y;ty++) {

		for(int tx=0;tx<tile_x;tx++) {

			glGenTextures( 1, &g_gfxBGtex[ty*tile_x+tx].texture ); // allocate a texture name
			glBindTexture( GL_TEXTURE_2D, g_gfxBGtex[ty*tile_x+tx].texture ); // select our current texture

			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); // select modulate to mix texture with color for shading
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_NEAREST ); // when texture area is small, bilinear filter the closest MIP map
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // when texture area is large, bilinear filter the first MIP map
			// if wrap is true, the texture wraps over at the edges (repeat)
			//       ... false, the texture ends at the edges (clamp)
			/*glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP );
			*/

			BYTE* data =(BYTE *) malloc( width * height * 4 );
			int c=0;
			for(int dty=0;dty<width;dty++) {
				for(int dtx=0;dtx<width;dtx++) {
					data[4*c+0]=img_in.data[4*dtx+(16*tx*4)+(4*dty*img_in.width)+(16*ty*4*img_in.width)+0];
					data[4*c+1]=img_in.data[4*dtx+(16*tx*4)+(4*dty*img_in.width)+(16*ty*4*img_in.width)+1];
					data[4*c+2]=img_in.data[4*dtx+(16*tx*4)+(4*dty*img_in.width)+(16*ty*4*img_in.width)+2];
					data[4*c+3]=img_in.data[4*dtx+(16*tx*4)+(4*dty*img_in.width)+(16*ty*4*img_in.width)+3];
					c++;
				}
			}

			g_gfxBGtex[ty*tile_x+tx].tx=d_tx;
			g_gfxBGtex[ty*tile_x+tx].ty=d_ty;
			g_gfxBGtex[ty*tile_x+tx].tz=d_tz;
			g_gfxBGtex[ty*tile_x+tx].ang_x=rot_x;
			g_gfxBGtex[ty*tile_x+tx].ang_x=rot_y;
			g_gfxBGtex[ty*tile_x+tx].ang_z=rot_z;
			// build our texture MIP maps
			gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width,
				height, GL_RGBA, GL_UNSIGNED_BYTE, data );
			//remove buffer
			free(data);

			d_tx+=2.0;
		}
		d_tx=-29.0;
		d_ty+=2.0;
	}


}
void updateCalculations(int millisecs) {
	 if (dorotate) {
	 			    test_rotate(millisecs);
	 			  //  printf ("%f\n",0.005f*millisecs);
					}
}
//------------------------------------------------------------------------------
// init()
//
// additional OpenGL initializations
//
// -> 
// <- --
//------------------------------------------------------------------------------
void initOpenGL(void)
{
	glShadeModel (GL_FLAT);
	glDisable (GL_DEPTH_TEST);
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glClearDepth(1.0f);
	glCullFace( GL_BACK);
	glEnable(GL_CULL_FACE);
}


//------------------------------------------------------------------------------
// test_rotate
//
// function for testing rotating the tiled texture
//
// -> 
// <- --
//------------------------------------------------------------------------------

void test_rotate(int millisecs) {
	 
	 /*(1) es gibt einen globalen zeitzähler (zu erreichen mit GetTimer() in Win32Helper)
	 (2) jede kachel hat einen rotationswinkel abhängig von diesem zeitzähler (das führt erstmal dazu, daß sich alle gleichzeitig drehen)
	 (3) abhängig von sqrt(x*x + y*y) = position in der diagonalen wird von diesem zeitwert was abgezogen
*/

	for(int y=0;y<17;y++) {
		for(int x=0;x<30;x++) {
		    if ((g_gfxBGtex_a[y*30+x].ang_x<180)&&(g_gfxBGtex_a[y*30+x].ang_z<180)) {
		        if (g_gfxBGtex_a[y*30+x].ang_x>=360) g_gfxBGtex_a[y*30+x].ang_x=0;
    			if (g_gfxBGtex_a[y*30+x].ang_z>=360) g_gfxBGtex_a[y*30+x].ang_z=0;
	    		if (g_gfxBGtex_a[y*30+x].delay_x==0) g_gfxBGtex_a[y*30+x].delay_x=4*sqrt(x*x+ y*y);
//				printf("%i | ",g_gfxBGtex_b[y*30+x].delay_x);
	    		if (g_gfxBGtex_a[y*30+x].delay_x>1) {
					  g_gfxBGtex_a[y*30+x].delay_x-=1;
				}
		    	else {
			        g_gfxBGtex_a[y*30+x].ang_x+=0.11f*millisecs;//*(360/sqrt(x*x + y*y));
      			    g_gfxBGtex_a[y*30+x].ang_z+=0.11f*millisecs;//*(360/sqrt(x*x + y*y));

			     //   g_gfxBGtex_b[y*30+x].ang_x+=3;//*(360/sqrt(x*x + y*y));
      			 //   g_gfxBGtex_b[y*30+x].ang_z+=3;//*(360/sqrt(x*x + y*y));

		        }
		    }
		    else {
				 g_gfxBGtex_a[y*30+x].ang_x=180;
				 g_gfxBGtex_a[y*30+x].ang_z=180;
				 }
		}
	}
	if ((g_gfxBGtex_a[16*30+29].ang_x==180)&&(g_gfxBGtex_a[16*30+29].ang_z==180) ) {
 	   dorotate=!dorotate;
	}
}


//------------------------------------------------------------------------------
// display
//
// Callback function registered to glutDisplayFunc
// main rendering procedure
//
// -> 
// <- --
//------------------------------------------------------------------------------
void displayopengl(void) 
{

	if (!kWindowHeight)
		return;
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
    //glDrawPixels(g_gfxBGfximg_a.width,g_gfxBGfximg_a.height,  GL_RGBA,  GL_UNSIGNED_BYTE,g_gfxBGfximg_a.data);
 
#ifdef WIN32_PSPFRAME
	glDrawPixels(g_gfxBGpsp.width,g_gfxBGpsp.height,  GL_RGBA,  GL_UNSIGNED_BYTE,g_gfxBGpsp.data);
 	glCallList(1);
	glScissor(184,400-318,480,272);
#endif
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// setup texture mapping
	//glClearColor( 1.0f, 1.0f, 1.0f, 0.5f );
#ifdef WIN32_PSPFRAME
	glScaled(1.0/53,1.0/25,0.5);
#else
	glScaled(1.0/30,1.0/17,0.5);
#endif
	//glTranslatef(0.0f, 0.0f, g_debug_x);
	gfxDrawTiledTexture(&g_gfxBGtex_a[0],&g_gfxBGtex_b[0]);

}

//------------------------------------------------------------------------------
// cleanup
//
// function to cleanup all initialized variables
//
// -> 
// <- --
//------------------------------------------------------------------------------
void cleanup()
{
#ifdef WIN32_PSPFRAME
    DeleteObject(g_CopyBGRgn);
#endif
	for(int t=0;t<30*17;t++) {
		glDeleteTextures( 1, &g_gfxBGtex_b[t].texture );
	}

	free(g_gfxBGimage.data);
	ftFreeFont(g_ftFont);
}

//------------------------------------------------------------------------------
// WndProc
//
// main window procedure
//
// -> 
// <- --
//------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  static HDROP hDrop;
  HRESULT hr;
   PAINTSTRUCT    ps;

  static int omx, omy, mx, my;
  switch(Message)
  {
    case WM_CREATE:
    {
       //HDC hdcWindow;
      //MainHDC = GetDC(hwnd);
      break;
    }
	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
				return 0;
		case 'R':
		case 'r':
			 dorotate=!dorotate;
			 break;
		}
		break;
    case WM_PAINT:
    {
         BeginPaint(hwnd, &ps);							
		EndPaint(hwnd, &ps);							
		break;


    }
	
	case WM_CLOSE:
    {
      DestroyWindow(hwnd);
      break;
    }

    case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }

    default:
    {
      return DefWindowProc(hwnd, Message, wParam, lParam);
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
// EnableOpenGL
//
// Set Window to use OpenGL
//
// -> 
// <- --
//------------------------------------------------------------------------------
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    // get the device context (DC)
    *hDC = GetDC( hWnd );

    // set the pixel format for the DC
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat( *hDC, &pfd );
    SetPixelFormat( *hDC, iFormat, &pfd );

    // create and enable the render context (RC)
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );
}

//------------------------------------------------------------------------------
// DisableOpenGL
//
// removes OpenGL Support from window
//
// -> 
// <- --
//------------------------------------------------------------------------------
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );
}

//------------------------------------------------------------------------------
// InitApplication
//
// desc
//
// -> 
// <- --
//------------------------------------------------------------------------------
int InitApplication(int argc, char** argv)
{
  WNDCLASSEX WndClass;
  MSG msg;
  HRESULT hr;
  HICON chaka;
  g_hInst=GetModuleHandle(NULL);
  // Bildschirmgröße abfragen und Fensterposition festlegen
  cxScreen = GetSystemMetrics(SM_CXSCREEN);
  cyScreen = GetSystemMetrics(SM_CYSCREEN);
  WindowPosX = (cxScreen/2) - (WindowSizeX/2);
  WindowPosY = (cyScreen/2) - (WindowSizeY/2);

  //correction because of windows frames
  int iwcorx=2*GetSystemMetrics(SM_CXSIZEFRAME);
  int iwcory=2*GetSystemMetrics(SM_CXSIZEFRAME)+GetSystemMetrics(SM_CYCAPTION);
  
  WndClass.cbSize        = sizeof(WNDCLASSEX);
  WndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WndClass.lpfnWndProc   = WndProc;
  WndClass.cbClsExtra    = 0;
  WndClass.cbWndExtra    = 0;
  WndClass.hInstance     = g_hInst;
  WndClass.hIcon         = NULL; //LoadIcon(g_hInst,MAKEINTRESOURCE( IDC_ICON_LARGE ));
  WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  WndClass.hbrBackground = NULL; //(HBRUSH)(COLOR_BTNFACE+1);
  WndClass.lpszMenuName  = NULL;
  WndClass.lpszClassName = g_szClassName;
  WndClass.hIconSm       = NULL; //LoadIcon(NULL, MAKEINTRESOURCE( IDC_ICON_LARGE ));

  // Fenster registrieren
  if(!RegisterClassEx(&WndClass))
  {
     MessageBox(0, "Window registration failed!", "Fatal",
                MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
     return 0;
  }


  mainhwnd = CreateWindowEx( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, g_szClassName, WINDOW_NAME,
                             WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ,
                             WindowPosX, WindowPosY, WindowSizeX+iwcorx, WindowSizeY+iwcory,
                             NULL, NULL, g_hInst, NULL);

  // Fenster erzeugen, anzeigen und updaten
  if(!mainhwnd)
  {
     MessageBox(0, "Window creation failed!", "Fatal",
                MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
     return 0;
  }

  ShowWindow(mainhwnd, SW_SHOW);
  UpdateWindow(mainhwnd);

  // zentrale Message-Schleife
  EnableOpenGL(mainhwnd, &m_hDC, &m_hRC);
  prepareData();
  initOpenGL();
   bool g_mainloop=true;
   while (g_mainloop)											// Loop Until WM_QUIT Is Received
	{
    	// Success Creating Window.  Check For Window Messages
		if (PeekMessage (&msg, mainhwnd, 0, 0, PM_REMOVE) != 0) {
			// Check For WM_QUIT Message
			if ( msg.message == WM_QUIT ) 
			{
				g_mainloop=false;
			} 
			else 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else {
/*		    if (window.isVisible == FALSE) {
		        WaitMessage ();								// Application Is Minimized Wait For A Message
			}
			else {
	*/			tickcount = GetTimer ();
				updateCalculations(tickcount - lasttickcount);
				lasttickcount = tickcount;
				displayopengl();				
				SwapBuffers (m_hDC);
		//	}
		}
    }
  DisableOpenGL(mainhwnd, m_hDC, m_hRC);
  cleanup();
  UnregisterClass( g_szClassName, g_hInst );

  return msg.wParam;
}  
	
