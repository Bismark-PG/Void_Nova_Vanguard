/*==============================================================================

	Game Title And Logo [Title.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef TITLE_H
#define TITLE_H

enum class LOGO_STATE
{
    NONE,
	PRINT_START,    // Logo Print Start
	PRINT_MADE,     // Logo Print Made By
    WAIT_MADE_BY,      
	PRINT_CORP,     // Logo Print Corporation
    WAIT_CORP,      
    DONE
};

enum class INTRO_STATE
{
    NONE,
	BRACKET_START,      
	BRACKET_PRINT,      // Bracket Print Start
	WAIT_FOR_LOGO,      // Wait For Logo Print Done
    WAIT_LOGO_DONE,   
	SYS_OS_PRINT,       // System OS Print Start
    WAIT_INIT,        
	DATA_PRINT,         // Mission Data Print Start
    WAIT_DATA,        
	READY_PRINT,        // Mission Ready Print Start
    WAIT_READY,       
    DONE
};

enum class GLITCH_STATE
{
	NONE,
	OP_PRINT,		// Operation Print
	OP_WAIT,
	TITLE_PRINT,	// Title Print
	TITLE_WAIT,
	GLITCH_START,	// Glitch Effect Start
	BLACK_OUT,		// Black Out Effect Start
	BLUE_FLARE,		// Blue Flare Effect Start
	WHITE_OUT,		// White Out Effect Start
	DONE
};

enum class FLARE_STATE
{
	NONE,
	SCALE_UP,		// Flare Scale Up	Scale : 0	 ~ 0.25
	SCALE_DOWN,		// Flare Scale Down Scale : 0.25 ~ 0.2
	POP_UP,			// Flare Pop Up		Scale : 0.2  ~ 0.5
	FLASH_BANG,		// WhitOut
	DONE
};

void Title_Initialize();
void Title_Finalize();

void Title_Reset();

void Title_Update(float elapsed_time);
void Title_Draw();
#endif // TITLE_H