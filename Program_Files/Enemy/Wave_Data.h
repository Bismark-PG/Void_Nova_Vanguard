/*==============================================================================

    Manage Wave Data File I/O [Wave_Data.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef WAVE_DATA_H
#define WAVE_DATA_H
#include <vector>
#include <unordered_map>
#include <string>
#include "Pattern_Data.h"

// "Wave" Is Pattern Tag List For Spawn Pattern
using Wave = std::vector<int>;

// "Stage Wave" Is Wave List For Spawn 
using Stage_Waves = std::vector<Wave>;

class Wave_Data_Manager
{
public:
    static Wave_Data_Manager& GetInstance()
    {
        static Wave_Data_Manager instance;
        return instance;
    }

	// Load Wave Data From CSV File
    void Load_Wave_CSV(const std::string& filepath);

	// Get Stage Waves By Stage Number
    const Stage_Waves* Get_Stage_Waves(int stage) const;

private:
    Wave_Data_Manager() = default;
    ~Wave_Data_Manager() = default;

    Wave_Data_Manager(const Wave_Data_Manager&) = delete;
    Wave_Data_Manager& operator=(const Wave_Data_Manager&) = delete;

    std::unordered_map<int, Stage_Waves> m_Stage_Data_Map;
};

#endif // WAVE_DATA_H