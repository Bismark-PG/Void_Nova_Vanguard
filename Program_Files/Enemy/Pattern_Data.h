/*==============================================================================
    
    Define Enemy Spawn Waves [Wave_Data.h]
    
    Author : Choi HyungJoon

==============================================================================*/
#ifndef PATTERN_DATA_H
#define PATTERN_DATA_H

#include <vector>
#include <unordered_map>
#include "Enemy_Type.h"

// Pattern Name
enum class PatternTag
{
    Pattern_1,
    Pattern_2,
    Pattern_3,
    Pattern_4
};

// Enemy Spawn Info
struct Pattern_Enemy_Node
{
    EnemyType Type;
    float X_Ratio;
    EvadeDirection E_Dir;
};

struct Pattern_Info
{
    float Cooldown;
    std::vector<Pattern_Enemy_Node> Enemies;
};

// Pattern List Map
static const std::unordered_map<PatternTag, Pattern_Info> Pattern_Map =
{
    // Pattern Tag, Cool-Time, Spawn Enemy Type, Spawn X Axis Ratio
    { PatternTag::Pattern_1, { 2.0f, { {EnemyType::ENEMY_NORMAL, 0.5f, EvadeDirection::Up} } }},

    { PatternTag::Pattern_2, { 2.0f, { {EnemyType::ENEMY_NORMAL, 0.8f, EvadeDirection::Right} } } },

    { PatternTag::Pattern_3, { 2.0f, { {EnemyType::ENEMY_NORMAL, 0.2f, EvadeDirection::Left} } } },

    { PatternTag::Pattern_4, { 5.0f, {
        {EnemyType::ENEMY_NORMAL, 0.25f, EvadeDirection::Left},
        {EnemyType::ENEMY_NORMAL, 0.5f,  EvadeDirection::Up},
        {EnemyType::ENEMY_NORMAL, 0.75f, EvadeDirection::Right}
    } } }
};

inline const Pattern_Info& Get_Pattern_Data(PatternTag tag)
{
    return Pattern_Map.at(tag);
}

#endif // PATTERN_DATA_H