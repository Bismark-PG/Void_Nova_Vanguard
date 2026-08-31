/*==============================================================================

	Game Flow Event Register For Event System [Game_Flow_Register.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef GAME_FLOW_REGISTER_H
#define GAME_FLOW_REGISTER_H


#include "Event_Types.h"

struct Stage_Event_Data : public EventData
{
	int Stage_Number;

	Stage_Event_Data(int stage) : Stage_Number(stage) {}
};

#endif // GAME_FLOW_REGISTER_H