/*==============================================================================

    Management Texture Resource [Game_Texture.h]

    Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Game_Texture.h"
#include "Texture_Manager.h"

Texture_Manager* Texture_M;

void Game_Texture_Initialize()
{
    Texture_M = Texture_Manager::GetInstance();
    Game_Texture_Load();
}

void Game_Texture_Finalize()
{
}

void Game_Texture_Load()
{
    //------------------------------------------------Debug Texture------------------------------------------------//
    Texture_M->Load("Debug_Text", L"Resource/Texture/Other/Console_ASCII_512.png");
    Texture_M->Load("Debug_Circle", L"Resource/Texture/Other/OBJ.png");
    
    Texture_M->Load("W", L"Resource/Texture/BG/RGBCMYK/White.png");
    Texture_M->Load("R", L"Resource/Texture/BG/RGBCMYK/Red.png");
    Texture_M->Load("G", L"Resource/Texture/BG/RGBCMYK/Green.png");
    Texture_M->Load("B", L"Resource/Texture/BG/RGBCMYK/Blue.png");
    Texture_M->Load("C", L"Resource/Texture/BG/RGBCMYK/Cyan.png");
    Texture_M->Load("M", L"Resource/Texture/BG/RGBCMYK/Magenta.png");
    Texture_M->Load("Y", L"Resource/Texture/BG/RGBCMYK/Yellow.png");
    Texture_M->Load("K", L"Resource/Texture/BG/RGBCMYK/Black.png");

    //-------------------------------------------<< In Game Texture >>-------------------------------------------//
    //---------------------In Game : Player Texture---------------------//
	// Bottom
    Texture_M->Load("Player_Bottom_Left",   L"Resource/Texture/Player/Player_Bottom_L.png");
    Texture_M->Load("Player_Bottom_Center", L"Resource/Texture/Player/Player_Bottom.png");
    Texture_M->Load("Player_Bottom_Right",  L"Resource/Texture/Player/Player_Bottom_R.png");

    // Middle
    Texture_M->Load("Player_Middle_Left",   L"Resource/Texture/Player/Player_Middle_L.png");
    Texture_M->Load("Player_Middle_Center", L"Resource/Texture/Player/Player_Middle.png");
    Texture_M->Load("Player_Middle_Right",  L"Resource/Texture/Player/Player_Middle_R.png");

    // Top
    Texture_M->Load("Player_Top_Left",      L"Resource/Texture/Player/Player_Top_L.png");
    Texture_M->Load("Player_Top_Center",    L"Resource/Texture/Player/Player_Top.png");
    Texture_M->Load("Player_Top_Right",     L"Resource/Texture/Player/Player_Top_R.png");
   
    //---------------------In Game : Enemy Texture---------------------//
    Texture_M->Load("Enemy", L"Resource/Texture/Other/HP.png");

    //---------------------In Game : Effect Texture---------------------//
    Texture_M->Load("Effect_Explosion", L"Resource/Texture/Animation/Explosion.png");
    
    //---------------------------------------------<< Card Texture >>---------------------------------------------//

    
    //----------------------------------------------<< BG Texture >>----------------------------------------------//
    Texture_M->Load("BG_F", L"Resource/Texture/BG/BG_Floor.png");
    Texture_M->Load("BG_S", L"Resource/Texture/BG/BG_Sky.png");

    //----------------------------------------------<< UI Texture >>----------------------------------------------//
    //---------------------UI : Title Texture---------------------//
    Texture_M->Load("Title", L"Resource/Texture/UI/Title_Line.png");

    //---------------------UI : Title Intro BG Texture---------------------//
    Texture_M->Load("Intro_Bracket", L"Resource/Texture/UI/Intro/Title_Bracket.png");
    Texture_M->Load("Intro_Bracket_RGB", L"Resource/Texture/UI/Intro/Title_Bracket_Alpha.png");

    //---------------------UI : Title Intro Logo Texture---------------------//
    Texture_M->Load("Intro_Logo", L"Resource/Texture/UI/Intro/Title_Logo.png");
    Texture_M->Load("Intro_Made_By", L"Resource/Texture/UI/Intro/Title_MadeBy.png");
    Texture_M->Load("Intro_Corporation", L"Resource/Texture/UI/Intro/Title_Corporation.png");
    Texture_M->Load("Intro_Name", L"Resource/Texture/UI/Intro/Title_Name.png");

    //---------------------UI : Title Intro Anim Texture---------------------//
    Texture_M->Load("Intro_SystemOS", L"Resource/Texture/UI/Intro/Title_System.png");
    Texture_M->Load("Intro_DtatInit", L"Resource/Texture/UI/Intro/Title_Data.png");
    Texture_M->Load("Intro_Ready", L"Resource/Texture/UI/Intro/Title_Ready.png");
    Texture_M->Load("Intro_Oper", L"Resource/Texture/UI/Intro/Title_Operation_Alpha.png");
    Texture_M->Load("Intro_Nova", L"Resource/Texture/UI/Intro/Title_Nova_Alpha.png");
    Texture_M->Load("Intro_Flare", L"Resource/Texture/UI/Intro/Title_Flare_Anim.png");
    
    //---------------------UI : Menu Texture---------------------//
    Texture_M->Load("Panel_BG", L"Resource/Texture/UI/BG_Panel.png");

    //---------------------UI : Main Menu Texture---------------------//
    Texture_M->Load("Start_N", L"Resource/Texture/UI/Menu/Main_Start_Wait.png");
    Texture_M->Load("Start_G", L"Resource/Texture/UI/Menu/Main_Start_Glow.png");
    Texture_M->Load("Settings_N", L"Resource/Texture/UI/Menu/Main_Setting_Wait.png");
    Texture_M->Load("Settings_G", L"Resource/Texture/UI/Menu/Main_Setting_Glow.png");
    Texture_M->Load("Ranking_N", L"Resource/Texture/UI/Menu/Main_Ranking_Wait.png");
    Texture_M->Load("Ranking_G", L"Resource/Texture/UI/Menu/Main_Ranking_Glow.png");
    Texture_M->Load("Exit_N", L"Resource/Texture/UI/Menu/Main_Done_Wait.png");
    Texture_M->Load("Exit_G", L"Resource/Texture/UI/Menu/Main_Done_Glow.png");
    
    //---------------------UI : Select Menu Texture---------------------//
    Texture_M->Load("Select_Tutorial", L"Resource/Texture/UI/Menu/Select_Training.png");
    Texture_M->Load("Select_Start", L"Resource/Texture/UI/Menu/Select_Start.png");
    Texture_M->Load("Select_Stage", L"Resource/Texture/UI/Menu/Select_Navi.png");
    Texture_M->Load("Select_Secter_Title", L"Resource/Texture/UI/Menu/Select_Sector.png");
    Texture_M->Load("Select_Back", L"Resource/Texture/UI/Menu/Select_Back.png");
    
    //---------------------UI : Setting Menu Texture---------------------//
    Texture_M->Load("Setting_BGM", L"Resource/Texture/UI/Menu/Setting_BGM.png");
    Texture_M->Load("Setting_SFX", L"Resource/Texture/UI/Menu/Setting_SFX.png");
    Texture_M->Load("Setting_Win", L"Resource/Texture/UI/Menu/Setting_Win_Mode.png");
    Texture_M->Load("Setting_Full", L"Resource/Texture/UI/Menu/Setting_Full_Mode.png");
    Texture_M->Load("Setting_Done", L"Resource/Texture/UI/Menu/Setting_Done.png");

    //---------------------UI : In Game UI Texture---------------------//
    Texture_M->Load("In_Game_Score", L"Resource/Texture/UI/In_Game/In_Game_Score.png");
    Texture_M->Load("In_Game_High_Score", L"Resource/Texture/UI/In_Game/In_Game_High_Score.png");
    Texture_M->Load("In_Game_Stage", L"Resource/Texture/UI/In_Game/In_Game_Stage.png");
    Texture_M->Load("In_Game_Destroyed", L"Resource/Texture/UI/In_Game/In_Game_Hit.png");
    
    //---------------------UI : Aim UI Texture---------------------//
    Texture_M->Load("Aim_MachineGun", L"Resource/Texture/UI/Aim_Normal.png");
    Texture_M->Load("Aim_Missile", L"Resource/Texture/UI/Aim_Missile.png");
    Texture_M->Load("Aim_Lock_On", L"Resource/Texture/UI/Aim_Lock_On.png");
    
    //---------------------UI : Mouse UI Texture---------------------//
    Texture_M->Load("UI_Mouse_Cursor", L"Resource/Texture/UI/Mouse_UI.png");

    //-----------------------UI : Number Texture-----------------------//
    Texture_M->Load("UI_Num_MIN", L"Resource/Texture/UI/Number/Num_00.png");
    Texture_M->Load("UI_Num_1",   L"Resource/Texture/UI/Number/Num_01.png");
    Texture_M->Load("UI_Num_2",   L"Resource/Texture/UI/Number/Num_02.png");
    Texture_M->Load("UI_Num_3",   L"Resource/Texture/UI/Number/Num_03.png");
    Texture_M->Load("UI_Num_4",   L"Resource/Texture/UI/Number/Num_04.png");
    Texture_M->Load("UI_Num_5",   L"Resource/Texture/UI/Number/Num_05.png");
    Texture_M->Load("UI_Num_6",   L"Resource/Texture/UI/Number/Num_06.png");
    Texture_M->Load("UI_Num_7",   L"Resource/Texture/UI/Number/Num_07.png");
    Texture_M->Load("UI_Num_8",   L"Resource/Texture/UI/Number/Num_08.png");
    Texture_M->Load("UI_Num_9",   L"Resource/Texture/UI/Number/Num_09.png");
    Texture_M->Load("UI_Num_MAX", L"Resource/Texture/UI/Number/Num_10.png");

    Texture_M->Load("UI_Num_Button_L", L"Resource/Texture/UI/Number/Num_Arr_L.png");
    Texture_M->Load("UI_Num_Button_R", L"Resource/Texture/UI/Number/Num_Arr_R.png");

//    Texture_M->Load("UI_Score_Number", L"Resource/Texture/UI/Num/UI_Num_Animation.png");

    //-------------------------------------------------Others Texture-------------------------------------------------//
    Texture_M->Load("Bismark_LOGO", L"Resource/Texture/Other/Bismark.jpg");
    Texture_M->Load("Bismark_LOGO_B", L"Resource/Texture/Other/Bismark_Black.jpg");
    Texture_M->Load("Bismark_Name", L"Resource/Texture/Other/Name_W.png");
    Texture_M->Load("Pixel_Withe", L"Resource/Texture/Other/W_Pixel.png");
    Texture_M->Load("Pixel_Black", L"Resource/Texture/Other/B_Pixel.png");

    // Sample
    Texture_M->Load("TextSample", L"Resource/Texture/BG/RGBCMYK/White.png");
}