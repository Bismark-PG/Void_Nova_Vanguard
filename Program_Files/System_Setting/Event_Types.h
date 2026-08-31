/*==============================================================================

	Event Type For Event System [Event_Types.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H

#include "Palette.h"

// Event Type
enum class EventType
{
	// Scene Change Event
    Go_From_Title_To_Main_Menu,
    Go_From_Main_Menu_To_In_Game,
    Go_From_In_Game_To_Main_Menu,
    Start_Fade,

	// Pop-Up Event
    Open_Stage_Select,
    Close_Stage_Select,
    Open_Settings,
    Close_Settings,
    Open_Ranking,

    // Game Flow Event
    Stage_Start,
    Phase_Spawning_Done,
    Exit_Game,

	// Audio Event
    Play_Audio_BGM,
	Stop_Audio_BGM,
	Play_Audio_SFX,
	Stop_Audio_SFX_All,
    Change_Audio_Volume,

	// Combet State Event
    Player_Weapon_Changed,
    Player_Fired,
    Visual_Hit_Effect,

    // Combet Action Request Event (Player >> Weapon Manager)
    Player_Weapon_Change_Requested,
    Player_Fire_Requested,

    // Enemy Combat Event
    Enemy_Fired,

    // Need More
};

// Event Data
struct EventData
{
    virtual ~EventData() = default;
};

// Event Data For Fade Event
struct Fade_Event_Data : public EventData
{
    double Duration;
    bool IsFadeOut;
    PALETTE::Palette FadeColor;

    Fade_Event_Data(double duration, bool isOut, PALETTE::Palette color = PALETTE::Black)
        : Duration(duration), IsFadeOut(isOut), FadeColor(color) {
    }
};

#endif // EVENT_TYPE_H