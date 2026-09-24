/*==============================================================================

    Manage Wave Data File I/O [Wave_Data.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Wave_Data.h"
#include "debug_ostream.h"

void Wave_Data_Manager::Load_Wave_CSV(const std::string& filepath)
{
    m_Stage_Data_Map.clear();
    std::ifstream File(filepath);

    if (!File.is_open())
    {
        Debug::D_Out << "[Wave Data] Failed To Load CSV : " << filepath << std::endl;
        return;
    }

    std::string Line;
	// Skip The First Line (Header)
    std::getline(File, Line);

	// Read Second Line (Stage Numbers)
    while (std::getline(File, Line))
    {
		// Parsing CSV Line Into Columns, Handling Quoted Commas
        std::vector<std::string> Columns;
        bool Info = false;
        std::string Current_Columns;
        for (char Index : Line)
        {
            if (Index == '\"') Info = !Info;
            else if (Index == ',' && !Info)
            {
                Columns.push_back(Current_Columns);
                Current_Columns.clear();
            }
            else Current_Columns += Index;
        }
        Columns.push_back(Current_Columns);

		// Load Data for Each Stage (Starting from Column B, Column A Is Wave ID)
        for (size_t i = 1; i < Columns.size(); ++i)
        {
            int stage_num = static_cast<int>(i);
            std::string data = Columns[i];

			// Delete Spaces and Tabs from the Beginning and End of the String
            data.erase(0, data.find_first_not_of(" \t\r\n"));
            data.erase(data.find_last_not_of(" \t\r\n") + 1);

            if (!data.empty())
            {
                Wave current_wave;

				// If Have Comma, Split Into Multiple Pattern Tags
                // e.g. : 1, 2, 3 = Pattern 1, Pattern 2, Pattern 3
                std::stringstream val_ss(data);
                std::string val;
                while (std::getline(val_ss, val, ','))
                {
                    val.erase(0, val.find_first_not_of(" \t\r\n"));
                    val.erase(val.find_last_not_of(" \t\r\n") + 1);
                    if (!val.empty())
                    {
                        current_wave.push_back(std::stoi(val));
                    }
                }

                if (!current_wave.empty())
                {
                    m_Stage_Data_Map[stage_num].push_back(current_wave);
                }
            }
        }
    }
    Debug::D_Out << "[Wave Data] Wave CSV Loaded Successfully!" << std::endl;
}

const Stage_Waves* Wave_Data_Manager::Get_Stage_Waves(int stage) const
{
    auto it = m_Stage_Data_Map.find(stage);
    if (it != m_Stage_Data_Map.end()) return &(it->second);
    return nullptr;
}