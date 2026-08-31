/*==============================================================================
    
    Define Enemy Spawn Waves [Wave_Data.h]
    
    Author : Choi HyungJoon

==============================================================================*/
#ifndef WAVE_DATA_H
#define WAVE_DATA_H
#include "Pattern_Data.h"

// "Wave" Is Pattern Tag List For Spawn Pattern
using Wave = std::vector<PatternTag>;

// "Stage Wave" Is Wave List For Spawn 
using Stage_Waves = std::vector<Wave>;

// For Prototype, Delete After
constexpr float Wave_End_Time = 240;


static const std::unordered_map<int, Stage_Waves> Stage_Data_Map =
{
// ---------------------------------------------------------//
//						   Stage 1							//
// ---------------------------------------------------------//
    {
        1,
        {
            { PatternTag::Pattern_1 },
            { PatternTag::Pattern_2 },
            { PatternTag::Pattern_3 },
            { PatternTag::Pattern_4 },
            { PatternTag::Pattern_2, PatternTag::Pattern_3 }
        }
    },

// ---------------------------------------------------------//
//						   Stage 2							//
// ---------------------------------------------------------//
    {
        2,
        { 
            {PatternTag::Pattern_4},
            {PatternTag::Pattern_1, PatternTag::Pattern_2}
        }
    },
};

#endif // WAVE_DATA_H