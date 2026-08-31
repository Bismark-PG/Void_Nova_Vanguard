/*==============================================================================

	Sound Event Register For Event System [Sound_Register.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef SOUND_REGISTER_H
#define SOUND_REGISTER_H

#include <string>
#include "Event_Types.h"

// Sound Tag Enum
enum class Sound_BGM_Tag
{
    Main,
};


enum class Sound_SFX_Tag
{
	// Controller Alert SFX
    Controller_Alert,

	// Intro SFX
    Intro_Loading,
    Intro_Glitch,
    Intro_Flare,

	// Buffer SFX
    Buffer_Move,
    Buffer_Select,
    Buffer_Back,
    Buffer_Denied
};

// Sound Event Data
struct Sound_BGM_Event_Data : public EventData
{
    Sound_BGM_Tag BGM_Tag;

    Sound_BGM_Event_Data(Sound_BGM_Tag t) : BGM_Tag(t) {}
};

struct Sound_SFX_Event_Data : public EventData
{
    Sound_SFX_Tag SFX_Tag;

    Sound_SFX_Event_Data(Sound_SFX_Tag t) : SFX_Tag(t) {}
};

struct Sound_Volume_Event_Data : public EventData
{
    bool Is_BGM;
    float Volume;

    Sound_Volume_Event_Data(bool is_bgm, float vol) : Is_BGM(is_bgm), Volume(vol) {}
};

// Enum -> String
inline std::string Get_BGM_String(Sound_BGM_Tag tag)
{
    switch (tag)
    {
    case Sound_BGM_Tag::Main:                   return "Intro_Loading";
    default:                                    return "";
    }
}

inline std::string Get_SFX_String(Sound_SFX_Tag tag)
{
    switch (tag)
    {
        // Controller Alert SFX
    case Sound_SFX_Tag::Controller_Alert:       return "Controller_Alert";

		// Intro SFX
    case Sound_SFX_Tag::Intro_Loading:          return "Intro_Loading";
    case Sound_SFX_Tag::Intro_Glitch:           return "Intro_Glitch_Sound";
    case Sound_SFX_Tag::Intro_Flare:            return "Intro_Flare";

		// Buffer SFX
    case Sound_SFX_Tag::Buffer_Move:            return "Buffer_Move";
    case Sound_SFX_Tag::Buffer_Select:          return "Buffer_Select";
    case Sound_SFX_Tag::Buffer_Back:            return "Buffer_Back";
    case Sound_SFX_Tag::Buffer_Denied:          return "Buffer_Denied";
    default:                                    return "";
    }
}

#endif // SOUND_REGISTER_H