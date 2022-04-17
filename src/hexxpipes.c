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
// # This is the main file.                                                   #
// #                                                                          #
// # Developers                                                               #
// #  nullp01nter (Thoralt Franz)                                             #
// #  opa_knack (Erik Hecht)                                                  #
// #                                                                          #
// ############################################################################ 

#include "hexxpipes.h"
#include "common/messagetext.h"

//-----------------------------------------------------------------------------
// main
//
// Entry point for the game
//
// -> --
// <- return code (normally 0)
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	Msg_Init("messages.txt");
	Msg_SetLanguage(1);

	InitApplication(argc, argv);		// defined in PSP/pspInit.h and win32/win32Init.h
	//StartGame();
	
	Msg_DeInit();
	return 0;
}
