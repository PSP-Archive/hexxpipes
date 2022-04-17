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
// # This is a set of functions for loading images.                           #
// #                                                                          #
// #                                                                          #
// ############################################################################ 
 
#include "gfxcore.h"
#define GFXNUMVERTICESBGFX 2*3


struct Vertex __attribute__((aligned(16))) gfxVerticesBGfx[GFXNUMVERTICESBGFX] =
{
		
	{0, 0, 0x807f0000,-1,-1, 1}, // a
	{1, 0, 0x807f0000, 1,-1, 1}, // b
	{0, 1, 0x807f0000,-1, 1, 1}, // c

	{1, 0, 0x807f0000, 1,-1, 1}, // b
	{1, 1, 0x807f0000, 1, 1, 1}, // d
	{0, 1, 0x807f0000,-1, 1, 1}, // c
};


//-----------------------------------------------------------------------------
// gfxLoadPNG
//
// Loads an image in PNG file format to a buffer (RGBA, 32 bit per pixel)
//
// example of usage:
//		unsigned long ulW = 0, ulH = 0;
//		unsigned char *ucBuf;
//		if(GFXLOAD_SUCCESS!=gfxLoadPNG("image.png", &ulW, &ulHt, &ucBuf))
//		{
//			// there was an error
//		}
//
// -> *file = filename including path
//    *pulWidth, *pulHeight = pointer to variables receiving the width and
//                            the height of the image being loaded (if NULL,
//							  no width or height is returned)
//    **image_data_ptr = pointer to your pointer which will hold the data
//                       the buffer is allocated inside of this function
//                       and has to be freed by yourself
// <- return code
//	GFXLOAD_SUCCESS
//	GFXLOAD_READ_ERROR
//	GFXLOAD_NO_PNG_ERROR
//	GFXLOAD_MEMORY_ERROR
//	GFXLOAD_BACKGROUNDCHUNK_ERROR
//	GFXLOAD_ILLEGAL_POINTER
//-----------------------------------------------------------------------------
int gfxLoadPNG(char *file, unsigned long *pulWidth, 
	unsigned long *pulHeight, unsigned char **image_data_ptr) {

	FILE          *infile;         
	png_structp   png_ptr;        
	png_infop     info_ptr;       
	unsigned char *image_data=NULL;  
	char          sig[8];   
	int           bit_depth,color_type,i;
	unsigned long width, height;
	unsigned int  rowbytes; 
	png_bytepp row_pointers = NULL;

	printf("gfxLoadPNG: Loading \"%s\"... ", file);
	
	if(!image_data_ptr)
	{
		printf("Error: image_data_ptr is NULL.\n");
		return GFXLOAD_ILLEGAL_POINTER;
	}
	
	// open the file
	infile = fopen(file, "rb");
	if (!infile) {
		printf("Error: Unable to open file.\n");
		return GFXLOAD_READ_ERROR;
	}
	
	// check signature of file
	fread(sig, 1, 8, infile);
	if (!png_check_sig((unsigned char *) sig, 8)) {
		fclose(infile);
		printf("Error: File has no PNG signature.\n");
		return GFXLOAD_NO_PNG_ERROR;
	}
 
	// setting up the PNG structs 
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(infile);
		printf("Error: png_create_read_struct() failed.\n");
		return GFXLOAD_MEMORY_ERROR;    
	}
 
   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		printf("Error: png_create_info_struct() failed.\n");
		fclose(infile);
		return GFXLOAD_MEMORY_ERROR;
	}
   
	// block to handle libpng errors, then check whether the PNG file had
	// a bKGD chunk
    if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		printf("Error: setjmp(png_jmpbuf()) failed.\n");
		fclose(infile);
		return GFXLOAD_BACKGROUNDCHUNK_ERROR;
	}

	// store file stream pointer (infile) for later use.
	png_init_io(png_ptr, infile);
   
	// anounce first 8 bytes read
	png_set_sig_bytes(png_ptr, 8);
   
	// Read the image info
  
	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	// setting up some transforms
	if (color_type == PNG_COLOR_TYPE_RGB)
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);	//output R8G8B8A8
   
	if (bit_depth > 8) {
		png_set_strip_16(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	}
   
	// Update the png info struct
	png_read_update_info(png_ptr, info_ptr);

	// Rowsize in bytes.
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data buffer
	if ((image_data = (unsigned char *) memalign(16, rowbytes * height))==NULL)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		printf("Error: malloc(%i) for image_data failed.\n",
			(int)(rowbytes * height));
		return GFXLOAD_MEMORY_ERROR;
    }

	if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        image_data = NULL;
		printf("Error: malloc(%i) for row_pointer failed.\n",
			(int)height*sizeof(png_bytep));
        return GFXLOAD_MEMORY_ERROR;
    }
   
    // set the individual row_pointers to point at the correct offsets
    for (i = 0;  i < (int)height;  ++i)
      #ifdef PSP
	    row_pointers[i] = image_data + i*rowbytes;
	  #else
	  	row_pointers[height-1-i] = image_data + i*rowbytes;
	  #endif
    // read the image
    png_read_image(png_ptr, row_pointers);

	// cleaning up
	free(row_pointers);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(infile);

	// set up return values
	if(pulWidth) *pulWidth = width;
	if(pulHeight) *pulHeight = height;
	*image_data_ptr = image_data;
	
	printf("OK (image is %ix%i, %i bytes in RAM).\n",
		(int)width, (int)height, (int)(width*height*4));
	return GFXLOAD_SUCCESS;
}

//-----------------------------------------------------------------------------
// gfxLoadJPEG
//
// Loads an image in JPEG file format to a buffer (RGBA, 32 bit per pixel)
//
// example of usage:
//		unsigned long ulW = 0, ulH = 0;
//		unsigned char *ucBuf;
//		if(GFXLOAD_SUCCESS!=gfxLoadJPEG("image.jpg", &ulW, &ulHt, &ucBuf))
//		{
//			// there was an error
//		}
//
// -> *file = filename including path
//    *pulWidth, *pulHeight = pointer to variables receiving the width and
//                            the height of the image being loaded (if NULL,
//							  no width or height is returned)
//    **image_data_ptr = pointer to your pointer which will hold the data
//                       the buffer is allocated inside of this function
//                       and has to be freed by yourself
// <- return code
//	GFXLOAD_SUCCESS
//	GFXLOAD_READ_ERROR
//	GFXLOAD_MEMORY_ERROR
//	GFXLOAD_ILLEGAL_POINTER
//-----------------------------------------------------------------------------
int gfxLoadJPEG(char *file, unsigned long *pulWidth, 
	unsigned long *pulHeight, unsigned char **image_data_ptr) {

	unsigned char *image_data=NULL;  
	unsigned long width = 0, height = 0;
	int x, y;
	
	printf("gfxLoadJPEG: Loading \"%s\"... ", file);

	if(!image_data_ptr)
	{
		printf("Error: image_data_ptr is NULL.\n");
		return GFXLOAD_ILLEGAL_POINTER;
	}
	
	// this part of source code is borrowed from LUA player, graphics.cpp
	struct jpeg_decompress_struct dinfo;
	struct jpeg_error_mgr jerr;
	dinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&dinfo);
	FILE* inFile = fopen(file, "rb");
	if (!inFile) {
		printf("Error: Unable to open file.\n");
		jpeg_destroy_decompress(&dinfo);
		return GFXLOAD_READ_ERROR;
	}
	jpeg_stdio_src(&dinfo, inFile);
	jpeg_read_header(&dinfo, TRUE);
	width = dinfo.image_width;
	height = dinfo.image_height;
	jpeg_start_decompress(&dinfo);
	
	image_data = (unsigned char*) memalign(16, width * height * 4);
	if (!image_data) {
		printf("Error: malloc(%i) for image_data failed.\n",
			(int)(width * height * 4));
		jpeg_destroy_decompress(&dinfo);
		return GFXLOAD_MEMORY_ERROR;
	}
	
	unsigned char* line = (unsigned char*) malloc(width * 3);
	if (!line) {
		printf("Error: malloc(%i) for line failed.\n", (int)(width * 3));
		jpeg_destroy_decompress(&dinfo);
		return GFXLOAD_MEMORY_ERROR;
	}
	if (dinfo.jpeg_color_space == JCS_GRAYSCALE) {
		while (dinfo.output_scanline < dinfo.output_height) {
			#ifdef PSP
			y = dinfo.output_scanline;
			#else
			y = dinfo.output_height-dinfo.output_scanline-1;
			#endif
			jpeg_read_scanlines(&dinfo, &line, 1);
			for (x = 0; x < (int)width; x++) {
				image_data[(x + width * y)*4+0] = line[x];		// R
				image_data[(x + width * y)*4+1] = line[x];		// G
				image_data[(x + width * y)*4+2] = line[x];		// B
				image_data[(x + width * y)*4+3] = 0xFF;			// A
			}
		}
	} else {
		while (dinfo.output_scanline < dinfo.output_height) {
            #ifdef PSP		
			y = dinfo.output_scanline;
			#else
			y = dinfo.output_height-dinfo.output_scanline-1;
			#endif
			jpeg_read_scanlines(&dinfo, &line, 1);
			unsigned char* linePointer = line;
			for (x = 0; x < (int)width; x++) {
				
				image_data[(x + width * y)*4+0] = *(linePointer++);		// R
				image_data[(x + width * y)*4+1] = *(linePointer++);		// G
				image_data[(x + width * y)*4+2] = *(linePointer++);		// B
				image_data[(x + width * y)*4+3] = 0xFF;					// A
			}
		}
	}
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
	free(line);
	fclose(inFile);
	
	// set up return values
	if(pulWidth) *pulWidth = width;
	if(pulHeight) *pulHeight = height;
	*image_data_ptr = image_data;

	printf("OK (image is %ix%i, %i bytes in RAM).\n",
		(int)width, (int)height, (int)(width*height*4));
		
	return GFXLOAD_SUCCESS;
}


//------------------------------------------------------------------------------
// gfxDrawTiledTexture()
//
// function for drawing a tiled texture
//
// TODO: should be in gfx core later with function parameters
//
// -> 
// <- --
//------------------------------------------------------------------------------
void gfxDrawTiledTexture(GFXTILEDTEXTURE* g_gfxBGtex_a,GFXTILEDTEXTURE* g_gfxBGtex_b){
	 int k=0,v=0;

	glEnable( GL_TEXTURE_2D );
	for(k=0;k<30*17;k++) {
        glPushMatrix();
		glTranslated(g_gfxBGtex_a[k].tx,g_gfxBGtex_a[k].ty,g_gfxBGtex_a[k].tz);
		glRotated(g_gfxBGtex_a[k].ang_x , 1.0f, 0.0f, 0.0f );
		glRotated(g_gfxBGtex_a[k].ang_y , 0.0f, 1.0f, 0.0f );
		glRotated(g_gfxBGtex_a[k].ang_z , 0.0f, 0.0f, 1.0f );
		glBindTexture(GL_TEXTURE_2D, g_gfxBGtex_a[k].texture);
		glFrontFace( GL_CCW );
    	glBegin(GL_TRIANGLES);
		for(v=0;v<GFXNUMVERTICESBGFX;v++) {
            glTexCoord2d(gfxVerticesBGfx[v].u,gfxVerticesBGfx[v].v);
			glVertex3d(gfxVerticesBGfx[v].x,gfxVerticesBGfx[v].y,gfxVerticesBGfx[v].z); 
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, g_gfxBGtex_b[k].texture);
		glFrontFace( GL_CW );
		glBegin(GL_TRIANGLES);
		for(v=0;v<GFXNUMVERTICESBGFX;v++) {
            glTexCoord2d(gfxVerticesBGfx[v].u*-1,gfxVerticesBGfx[v].v);
			glVertex3d(gfxVerticesBGfx[v].x,gfxVerticesBGfx[v].y,gfxVerticesBGfx[v].z); 
		}
		glEnd();
  		glPopMatrix();
		glCallList(1);
	}
}

