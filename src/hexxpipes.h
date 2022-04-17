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
// # hexxpipes.h                                                              #
// #                                                                          #
// ############################################################################ 

#ifndef _HEXXPIPES_H_
#define _HEXXPIPES_H_

// #define PSP		// PSP is automatically defined during psp-gcc run
// #define WIN32	// WIN32 is automatically defined during mingw run

#ifndef PSP
	#ifndef WIN32
		#error The symbol PSP or the symbol WIN32 must be defined.
	#endif
#endif

#ifdef PSP
	#ifdef WIN32
		#error The symbols PSP and WIN32 must not be defined at the same time.
	#endif
	#include "PSP/pspInit.h"
	#include "PSP/pspHelper.h"
#endif

#ifdef WIN32
	#include "win32/win32Init.h"
#endif

#endif	//  _HEXXPIPES_H_
