/*==============================================================================
 
    Game Title And Logo [Title.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Title.h"

// Assential Logic
#include "Fade.h"
#include "Input_Manager.h"
#include "Event_Manager.h"

// Other
#include "Shader_Manager.h"
#include "Sound_Register.h"

using namespace DirectX;

//-----------------Texture-----------------//
static int Title_BG = -1, Title_Bracket = -1, Title_Bracket_RGB = -1;
static int Intro_Logo = -1, Intro_MadeBy = -1, Intro_Corparation = -1, Intro_Name = -1;
static int Intro_System = -1, Intro_Dtat = -1, Intro_Ready = -1, Intro_Nova = -1, Intro_Operation = -1;
static int Intro_Flare = -1, Flare_Play_ID = -1;

//-----------------State-----------------//
static float ScreenW = 0.0f, ScreenH = 0.0f;

static float Logo_Size = 0.0f, Logo_X = 0.0f, Logo_Y = 0.0f;
static float MadeBy_W = 0.0f, MadeBy_H = 0.0f, MadeBy_X = 0.0f, MadeBy_Y = 0.0f;
static float Corp_W = 0.0f, Corp_H = 0.0f, Corp_X = 0.0f, Corp_Y = 0.0f;
static float Name_W = 0.0f, Name_H = 0.0f, Name_X = 0.0f, Name_Y = 0.0f;
static float Sys_W = 0.0f, Sys_H = 0.0f, Sys_X = 0.0f, Sys_Y = 0.0f;
static float Data_W = 0.0f, Data_H = 0.0f, Data_X = 0.0f, Data_Y = 0.0f;
static float Ready_W = 0.0f, Ready_H = 0.0f, Ready_X = 0.0f, Ready_Y = 0.0f;
static float Op_W = 0.0f, Op_H = 0.0f, Op_X = 0.0f, Op_Y = 0.0f;
static float Nova_W = 0.0f, Nova_H = 0.0f, Nova_X = 0.0f, Nova_Y = 0.0f;

//-----------------Ratio-----------------//
static float Bracket_Ratio = 0.0f;
static float Line1_Ratio = 0.0f; // Logo, Created By
static float Line2_Ratio = 0.0f; // Corp, Name
static float SysOS_Ratio = 0.0f;
static float Data_Ratio = 0.0f;
static float Ready_Ratio = 0.0f;
static float Op_Ratio = 0.0f;
static float Nova_Ratio = 0.0f;

static float Print_Speed = 2.0f;
static float Title_Print_Speed = 1.0f;
static float Intro_Wait_Timer = 0.0f;
static float Title_Wait_Timer = 0.0f;

static float Glitch_Timer = 0.0f;
static float Flare_Timer = 0.0f;
static float Flare_Scale = 0.0f;

static LOGO_STATE   Current_Logo_State   = LOGO_STATE::NONE;
static INTRO_STATE  Current_Intro_State  = INTRO_STATE::NONE;
static GLITCH_STATE Current_Glitch_State = GLITCH_STATE::NONE;
static FLARE_STATE  Current_Flare_State  = FLARE_STATE::NONE;

static bool Is_Intro_SFX_Playing = false;
static bool Is_Glitch_SFX_Playing = false;
static bool Is_Flare_SFX_Playing = false;
static bool Is_Title_Skip = false;

static const float FADE_DURATION = 1.0f;
static const float FADE_TIME = 1.5f;

//---------------Private Logic---------------//
void Title_Texture();

// --- Phase 1 ---
void Logo_Update(float dt);
void Logo_Draw();

// --- Phase 2 ---
void Intro_Update(float dt);
void Intro_Draw();

// --- Phase 3 ---
void Glitch_Update(float dt);
void Glitch_Draw();

void Flare_Update(float dt);

// --- Background ---
void Title_BG_Draw();

// --- Skip ---
void Title_Intro_Skip();

//-----------------Main Logic-----------------//
void Title_Initialize()
{
	Title_Texture();

    ScreenW = static_cast<float>(Direct3D_GetBackBufferWidth());
    ScreenH = static_cast<float>(Direct3D_GetBackBufferHeight());

    // 1. Logo
    Logo_Size = ScreenH * 0.2f;
    Logo_X = ScreenW * 0.05f;
    Logo_Y = ScreenH * 0.075f;
    float Text_Base_X = Logo_X + Logo_Size + (Logo_Size * 0.1f);

    // 2. Created By
    MadeBy_H = Logo_Size * 0.3f;
    MadeBy_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Intro_MadeBy, MadeBy_H);
    MadeBy_X = Text_Base_X;
    MadeBy_Y = Logo_Y;

    // 3. Corporation
    Corp_H = Logo_Size * 0.3f;
    Corp_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Intro_Corparation, Corp_H);
    Corp_X = Text_Base_X;
    Corp_Y = Logo_Y + (Logo_Size * 0.5f) - (Corp_H * 0.5f);

    // 4. Bismark Name
    Name_H = Corp_H;
    Name_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Intro_Name, Name_H);
    Name_X = Corp_X + Corp_W + (Logo_Size * 0.1f);
    Name_Y = Corp_Y;

    // 5. System OS
    Sys_H = Logo_Size * 0.5f;
    Sys_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Intro_System, Sys_H);
    Sys_X = Logo_X;
    Sys_Y = Logo_Y + Logo_Size + (Logo_Size * 0.25f);

    // 6. Mission Data
    Data_H = Sys_H;
    Data_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Intro_Dtat, Data_H);
    Data_X = Logo_X;
    Data_Y = Sys_Y + Sys_H + (Sys_H * 0.5f);

    // 7. Mission Ready
    Ready_H = Data_H;
    Ready_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Intro_Ready, Ready_H);
    Ready_X = Logo_X;
    Ready_Y = Data_Y + Data_H + (Data_H * 0.5f);

    // 8. Operation, Title
    Op_H = Sys_H; 
    Op_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Intro_Operation, Op_H);
    Op_X = Logo_X; 
    Op_Y = Logo_Y;

    Nova_H = ScreenH * 0.25f;
    Nova_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Intro_Nova, Nova_H);
    Nova_X = ScreenW * 0.1f; 
    Nova_Y = Op_Y + Op_H;

    Title_Reset();
}

void Title_Finalize()
{
    Title_Reset();
}

void Title_Reset()
{
    Bracket_Ratio = Line1_Ratio = Line2_Ratio = 0.0f;
    SysOS_Ratio = Data_Ratio = Ready_Ratio = 0.0f;
    Intro_Wait_Timer = 0.0f;

    Glitch_Timer, Flare_Timer, Flare_Scale = 0.0f;

    Current_Logo_State = LOGO_STATE::NONE;
    Current_Intro_State = INTRO_STATE::BRACKET_START;
    Current_Glitch_State = GLITCH_STATE::NONE;
    Current_Flare_State = FLARE_STATE::NONE;

    Is_Intro_SFX_Playing = false;
    Is_Glitch_SFX_Playing = false;
    Is_Flare_SFX_Playing = false;
	Is_Title_Skip = false;

    int Flare_Pattern_ID = SpriteAni_Get_Pattern_Info(Intro_Flare, 20, 5, 0.075, { 200, 200 }, { 0, 0 }, true);

    if (Flare_Pattern_ID >= 0)
    {
        Flare_Play_ID = SpriteAni_CreatePlayer(Flare_Pattern_ID);
    }
}

void Title_Update(float elapsed_time)
{
    if (M_INPUT->Is_Escape_Trigger() && !Is_Title_Skip)
    {
		Title_Intro_Skip();
    }

    if (!Is_Title_Skip)
    {
        Logo_Update(elapsed_time);
        Intro_Update(elapsed_time);
        Glitch_Update(elapsed_time);
    }
    else
    {
        Glitch_Update(elapsed_time);
    }
}

void Title_Draw()
{
    Direct3D_SetDepthEnable(false);
    Shader_Manager::GetInstance()->Begin2D();

	// BG And Bracket
    if (Current_Glitch_State < GLITCH_STATE::BLACK_OUT)
    {
        Title_BG_Draw();

        if (Current_Logo_State != LOGO_STATE::DONE || Current_Intro_State != INTRO_STATE::DONE)
        {
            // Draw Intro Animation
            Logo_Draw();
            Intro_Draw();
        }
    }

    // Draw Glitch Animation
    Glitch_Draw();
}

//---------------Private Logic---------------//
void Logo_Update(float dt)
{
    switch (Current_Logo_State)
    {
    case LOGO_STATE::NONE:
        break;
    case LOGO_STATE::PRINT_START:
        Intro_Wait_Timer += dt;

        if (Intro_Wait_Timer >= 2.5f)
        {
            Current_Logo_State = LOGO_STATE::PRINT_MADE;
        }
        break;

    case LOGO_STATE::PRINT_MADE:
        Line1_Ratio += Print_Speed * dt;

        if (Line1_Ratio >= 1.0f)
        {
            Line1_Ratio = 1.0f;
            Intro_Wait_Timer = 0.0f;
            Current_Logo_State = LOGO_STATE::WAIT_MADE_BY;
        }
        break;

    case LOGO_STATE::WAIT_MADE_BY:
        Intro_Wait_Timer += dt;

        if (Intro_Wait_Timer >= 0.5f)
        {
            Current_Logo_State = LOGO_STATE::PRINT_CORP;
        }
        break;

    case LOGO_STATE::PRINT_CORP:
        Line2_Ratio += Print_Speed * dt;

        if (Line2_Ratio >= 1.0f)
        {
            Line2_Ratio = 1.0f;
            Intro_Wait_Timer = 0.0f;
            Current_Logo_State = LOGO_STATE::WAIT_CORP;
        }
        break;

    case LOGO_STATE::WAIT_CORP:
        Intro_Wait_Timer += dt;

        if (Intro_Wait_Timer >= 0.5f)
        {
            Debug::D_Out << "[Title] Logo System DONE" << std::endl;
            Current_Logo_State = LOGO_STATE::DONE;
        }
        break;

    case LOGO_STATE::DONE:
        break;
    }
}

void Logo_Draw()
{
    if (Intro_Logo == -1 || Intro_MadeBy == -1 || Intro_Corparation == -1 || Intro_Name == -1) return;

    // Logo Printing
    if (Line1_Ratio > 0.0f)
    {
		// Logo
        float Tex_W_Logo = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(Intro_Logo));
        float Tex_H_Logo = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(Intro_Logo));

        Sprite_UV_Draw(Intro_Logo, Logo_X, Logo_Y, Logo_Size, Logo_Size * Line1_Ratio,
            0.0f, 0.0f, Tex_W_Logo, Tex_H_Logo * Line1_Ratio, 0.0f);

		// Created By
        float Tex_W_MadeBy = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(Intro_MadeBy));
        float Tex_H_MadeBy = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(Intro_MadeBy));

        Sprite_UV_Draw(Intro_MadeBy, MadeBy_X, MadeBy_Y, MadeBy_W, MadeBy_H * Line1_Ratio,
            0.0f, 0.0f, Tex_W_MadeBy, Tex_H_MadeBy * Line1_Ratio, 0.0f);
    }

	// Name Printing
    if (Line2_Ratio > 0.0f)
    {
        // Corporation
        float Tex_W_Corp = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(Intro_Corparation));
        float Tex_H_Corp = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(Intro_Corparation));

        Sprite_UV_Draw(Intro_Corparation, Corp_X, Corp_Y, Corp_W, Corp_H * Line2_Ratio,
            0.0f, 0.0f, Tex_W_Corp, Tex_H_Corp * Line2_Ratio, 0.0f);

        // Name
        float Tex_W_Name = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(Intro_Name));
        float Tex_H_Name = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(Intro_Name));

        Sprite_UV_Draw(Intro_Name, Name_X, Name_Y, Name_W, Name_H * Line2_Ratio,
            0.0f, 0.0f, Tex_W_Name, Tex_H_Name * Line2_Ratio, 0.0f);
    }
}

void Intro_Update(float dt)
{
    switch (Current_Intro_State)
    {
    case INTRO_STATE::NONE:
        break;

    case INTRO_STATE::BRACKET_START:
        if (!Is_Intro_SFX_Playing)
        {
            Debug::D_Out << "[Title] Intro Bracket Start" << std::endl;

            Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Intro_Loading);
            EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

            Is_Intro_SFX_Playing = true;
        }

        Current_Intro_State = INTRO_STATE::BRACKET_PRINT;
        break;

    case INTRO_STATE::BRACKET_PRINT:
        Bracket_Ratio += Print_Speed * dt;

        if (Bracket_Ratio >= 1.0f)
        {
            Bracket_Ratio = 1.0f;
            Intro_Wait_Timer = 0.0f;
            Current_Intro_State = INTRO_STATE::WAIT_FOR_LOGO;
        }
        break;

    case INTRO_STATE::WAIT_FOR_LOGO:
        Intro_Wait_Timer += dt;

        if (Intro_Wait_Timer >= 1.0f)
        {
            Debug::D_Out << "[Title] Braket Done" << std::endl;
            Current_Intro_State = INTRO_STATE::WAIT_LOGO_DONE;

            Debug::D_Out << "[Title] Logo Draw Start" << std::endl;
			Current_Logo_State = LOGO_STATE::PRINT_START; // Logo Draw Start
        }
        break;

    case INTRO_STATE::WAIT_LOGO_DONE:
		if (Current_Logo_State == LOGO_STATE::DONE) // Wait For Logo Done
        {
            Current_Intro_State = INTRO_STATE::SYS_OS_PRINT;
        }
        break;

    case INTRO_STATE::SYS_OS_PRINT:
        SysOS_Ratio += Print_Speed * dt;

        if (SysOS_Ratio >= 1.0f) 
        { 
            SysOS_Ratio = 1.0f; 
            Intro_Wait_Timer = 0.0f; 
            Current_Intro_State = INTRO_STATE::WAIT_INIT;
        }
        break;

    case INTRO_STATE::WAIT_INIT:
        Intro_Wait_Timer += dt;

        if (Intro_Wait_Timer >= 1.0f)
        {
            Current_Intro_State = INTRO_STATE::DATA_PRINT;
        }
        break;

    case INTRO_STATE::DATA_PRINT:
        Data_Ratio += Print_Speed * dt;

        if (Data_Ratio >= 1.0f)
        { 
            Data_Ratio = 1.0f; 
            Intro_Wait_Timer = 0.0f; 
            Current_Intro_State = INTRO_STATE::WAIT_DATA; 
        }
        break;

    case INTRO_STATE::WAIT_DATA:
        Intro_Wait_Timer += dt;

        if (Intro_Wait_Timer >= 1.5f) 
        {
            Current_Intro_State = INTRO_STATE::READY_PRINT;
        }
        break;

    case INTRO_STATE::READY_PRINT:
        Ready_Ratio += Print_Speed * dt;

        if (Ready_Ratio >= 1.0f)
        {
            Ready_Ratio = 1.0f;
            Intro_Wait_Timer = 0.0f; 
            Current_Intro_State = INTRO_STATE::WAIT_READY; 
        }
        break;

    case INTRO_STATE::WAIT_READY:
        Intro_Wait_Timer += dt;

        if (Intro_Wait_Timer >= 1.0f)
        { 
            Intro_Wait_Timer = 0.0f; 
            Current_Intro_State = INTRO_STATE::DONE;

            Debug::D_Out << "[Title] Phase 2 Draw Start" << std::endl;
            Current_Glitch_State = GLITCH_STATE::OP_PRINT;
        }
        break;

    case INTRO_STATE::DONE:
        break;
    }
}

void Intro_Draw()
{
    if (Intro_System == -1 || Intro_Dtat == -1 || Intro_Ready == -1) return;

    // System Text Drawing
    if (SysOS_Ratio > 0.0f)
    {
        float TexW = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(Intro_System));
        float TexH = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(Intro_System));

        Sprite_UV_Draw(Intro_System, Sys_X, Sys_Y, Sys_W, Sys_H * SysOS_Ratio,
            0.0f, 0.0f, TexW, TexH * SysOS_Ratio, 0.0f);
    }

    if (Data_Ratio > 0.0f)
    {
        float TexW = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(Intro_Dtat));
        float TexH = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(Intro_Dtat));

        Sprite_UV_Draw(Intro_Dtat, Data_X, Data_Y, Data_W, Data_H * Data_Ratio,
            0.0f, 0.0f, TexW, TexH * Data_Ratio, 0.0f);
    }

    if (Ready_Ratio > 0.0f)
    {
        float TexW = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(Intro_Ready));
        float TexH = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(Intro_Ready));

        Sprite_UV_Draw(Intro_Ready, Ready_X, Ready_Y, Ready_W, Ready_H * Ready_Ratio,
            0.0f, 0.0f, TexW, TexH * Ready_Ratio, 0.0f);
    }
}

void Glitch_Update(float dt)
{
    switch (Current_Glitch_State)
    {
    case GLITCH_STATE::OP_PRINT:
        Op_Ratio += Print_Speed * dt;

        if (Op_Ratio >= 1.0f)
        {
            Op_Ratio = 1.0f;
            Current_Glitch_State = GLITCH_STATE::TITLE_PRINT;
        }
        break;

    case GLITCH_STATE::TITLE_PRINT:
        Nova_Ratio += Title_Print_Speed * dt;

        if (Nova_Ratio >= 0.8f && !Is_Glitch_SFX_Playing)
        {
            Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Intro_Glitch);
            EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

            Is_Glitch_SFX_Playing = true;

            Debug::D_Out << "[Title] Glitch Start" << std::endl;
            Current_Glitch_State = GLITCH_STATE::GLITCH_START;
        }
        break;

    case GLITCH_STATE::GLITCH_START:
        if (Nova_Ratio >= 1.0f)
        {
            Nova_Ratio = 0.8f;
        }

        Glitch_Timer += dt;

        if (Glitch_Timer >= 2.0f)
        {
            Debug::D_Out << "[Title] Black Out After Glitch" << std::endl;
            Current_Glitch_State = GLITCH_STATE::BLACK_OUT;

            EventManager::GetInstance().Fire(EventType::Stop_Audio_SFX_All);

            Glitch_Timer = 0.0f;
        }
        break;

    case GLITCH_STATE::BLACK_OUT:
        Glitch_Timer += dt;

        if (Glitch_Timer >= 1.0f)
        {
            Debug::D_Out << "[Title] Flare Effect Start" << std::endl;
            Current_Glitch_State = GLITCH_STATE::BLUE_FLARE;
			Current_Flare_State = FLARE_STATE::SCALE_UP;
            Flare_Timer = 0.0f;
        }
        break;

    case GLITCH_STATE::BLUE_FLARE:
        Flare_Update(dt);
        break;

    case GLITCH_STATE::WHITE_OUT:
        Flare_Timer += dt;

        if (Flare_Timer <= 2.0f)
        {
            // 1.5 ~ 2.0 Sec : 0.5 -> 1.0 Scale
            float ratio = (Flare_Timer - 1.5f) / 0.5f; // 0.0 ~ 1.0
            Flare_Scale += Flare_Scale * 0.25f;
        }

        if (Flare_Timer >= 2.0f || Fade_GetState() == FADE_STATE::FINISHED_OUT)
        {
            Debug::D_Out << "[Title] Title Animation Done" << std::endl;
            Current_Glitch_State = GLITCH_STATE::DONE;

            Debug::D_Out << "[Title] Change State Title To Main_Menu" << std::endl;
            EventManager::GetInstance().Fire(EventType::Go_From_Title_To_Main_Menu);
        }
        break;

    case GLITCH_STATE::DONE:
        break;
    }
}

void Glitch_Draw()
{
    if (Title_Bracket_RGB == -1 || Intro_Operation == -1 || Intro_Nova == -1
        || Intro_Flare  == -1 || Flare_Play_ID == -1) return;

	// Draw Flare Effect
    if (Current_Glitch_State >= GLITCH_STATE::BLACK_OUT)
    {
        Sprite_Draw(Title_BG, 0.0f, 0.0f, ScreenW, ScreenH, 0.0f);

        if (Current_Glitch_State == GLITCH_STATE::BLUE_FLARE || Current_Glitch_State == GLITCH_STATE::WHITE_OUT)
        {
            float Base_Size = 200.0f;
            float fw = Base_Size * Flare_Scale;
            float fh = Base_Size * Flare_Scale;
            float fx = (ScreenW - fw) * 0.5f;
            float fy = (ScreenH - fh) * 0.5f;

            SpriteAni_Draw(Flare_Play_ID, fx, fy, fw, fh, 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
        }
        return;
    }

	// Glitch Effect Draw
    float rX = 0, bX = 0, gY = 0;
    if (Current_Glitch_State == GLITCH_STATE::GLITCH_START)
    {
        // Glitch Effect Random Offset
		rX = (rand() % 31) - 15.0f; // Red   : Move Left/Right Strong
		bX = (rand() % 21) - 10.0f; // Blue  : Move Left/Right Weak
		gY = (rand() % 11) - 5.0f;  // Green : Move Up/Down
    }

	// Make A Lambda Function To Draw The Texture With RGB Glitch Effect
    auto Draw_RGB_Glitch = [&](int TexID, float x, float y, float w, float h, float ratio)
        {
            float TexW = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(TexID));
            float TexH = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(TexID));

			// RBG Glitch Effect Will Be Draw Before The Main Texture
            if (Current_Glitch_State == GLITCH_STATE::GLITCH_START)
            {
                Sprite_UV_Draw(TexID, x + rX, y, w, h * ratio, 0.0f, 0.0f, TexW, TexH * ratio, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f }); // Red
                Sprite_UV_Draw(TexID, x, y + gY, w, h * ratio, 0.0f, 0.0f, TexW, TexH * ratio, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f }); // Green
                Sprite_UV_Draw(TexID, x + bX, y, w, h * ratio, 0.0f, 0.0f, TexW, TexH * ratio, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f }); // Blue
            }

			// Draw Main Texture
            Sprite_UV_Draw(TexID, x, y, w, h * ratio, 0.0f, 0.0f, TexW, TexH * ratio, 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
        };

    if (Current_Glitch_State == GLITCH_STATE::GLITCH_START && Title_Bracket_RGB != -1)
    {
        float dw = ScreenW * Bracket_Ratio;
        float dh = ScreenH * Bracket_Ratio;
        float dx = (ScreenW - dw) * 0.5f;
        float dy = (ScreenH - dh) * 0.5f;

        Draw_RGB_Glitch(Title_Bracket_RGB, dx, dy, dw, dh, 1.0f);
    }

    // Operation Print
    if (Op_Ratio > 0.0f) 
    {
        Draw_RGB_Glitch(Intro_Operation, Op_X, Op_Y, Op_W, Op_H, Op_Ratio);
    }

	// Logo Print
    if (Nova_Ratio > 0.0f) 
    {
        Draw_RGB_Glitch(Intro_Nova, Nova_X, Nova_Y, Nova_W, Nova_H, Nova_Ratio);
    }
}

void Flare_Update(float dt)
{
    Flare_Timer += dt;

    switch (Current_Flare_State)
    {
    case FLARE_STATE::NONE:
        break;
    case FLARE_STATE::SCALE_UP:
        if (Flare_Timer >= 0.2f)
        {
			Flare_Timer = 0.2f;
			Current_Flare_State = FLARE_STATE::SCALE_DOWN;
        }

        // 0.0 ~ 0.2 Sec : 0.0 -> 0.25 Scale
        Flare_Scale = (Flare_Timer / 0.2f) * 0.25f;
        break;

    case FLARE_STATE::SCALE_DOWN:
    {
        if (Flare_Timer >= 0.5f)
        {
            Flare_Timer = 0.5f;
            Current_Flare_State = FLARE_STATE::POP_UP;
        }

        // 0.2 ~ 0.5 Sec : 0.25 -> 0.2 Scale
        float ratio = (Flare_Timer - 0.2f) / 0.3f;
        Flare_Scale = 0.25f - (ratio * 0.05f);
    }
        break;

    case FLARE_STATE::POP_UP:
    {
        if (Flare_Timer >= 1.5f)
        {
			Flare_Timer = 1.5f;
            Current_Flare_State = FLARE_STATE::FLASH_BANG;
            Fade_Start(0.5f, true, White);
        }

        if (Flare_Timer >= 1.0f && !Is_Flare_SFX_Playing)
        {
            Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Intro_Flare);
            EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

            Is_Flare_SFX_Playing = true;
        }

        // 0.5 ~ 1.5 Sec : 0.2 -> 0.5 Scale
        float ratio = (Flare_Timer - 0.5f) / 1.0f;
        Flare_Scale = 0.2f + (ratio * 0.3f);
    }
        break;

    case FLARE_STATE::FLASH_BANG:
        if (Flare_Timer >= 1.5f)
        {
            Flare_Timer = 1.5f;
        }

        Debug::D_Out << "[Title] Flare Effect Done" << std::endl;
		Current_Flare_State = FLARE_STATE::DONE;
        Debug::D_Out << "[Title] White Out Start" << std::endl;
        Current_Glitch_State = GLITCH_STATE::WHITE_OUT;
        break;

    case FLARE_STATE::DONE:
        break;
    }
}

void Title_BG_Draw()
{
    if (Title_BG == -1 || Title_Bracket == -1) return;

    // Default BG
    Sprite_Draw(Title_BG, 0.0f, 0.0f, ScreenW, ScreenH, 0.0f);

    if (Bracket_Ratio > 0.0f)
    {
        // Drawing Bracket at Center of Screen
        float dw = ScreenW * Bracket_Ratio;
        float dh = ScreenH * Bracket_Ratio;
        float dx = (ScreenW - dw) * 0.5f;
        float dy = (ScreenH - dh) * 0.5f;

        Sprite_Draw(Title_Bracket, dx, dy, dw, dh, 0.0f);
    }
}

void Title_Intro_Skip()
{
    Fade_Start(0.2f, true, White);

    EventManager::GetInstance().Fire(EventType::Stop_Audio_SFX_All);

    Current_Logo_State   = LOGO_STATE::DONE;
    Current_Intro_State  = INTRO_STATE::DONE;
    Current_Flare_State  = FLARE_STATE::DONE;
    Current_Glitch_State = GLITCH_STATE::WHITE_OUT;

    Flare_Timer = 1.8f;

    Is_Intro_SFX_Playing  = true;
    Is_Glitch_SFX_Playing = true;
    Is_Flare_SFX_Playing  = true;

    Is_Title_Skip = true;
}

//----------------Texture List----------------//
void Title_Texture()
{
	//----------------BG Texture----------------//
    Title_BG            = Texture_Manager::GetInstance()->GetID("K");
    Title_Bracket       = Texture_Manager::GetInstance()->GetID("Intro_Bracket");
    Title_Bracket_RGB   = Texture_Manager::GetInstance()->GetID("Intro_Bracket_RGB");
        
	//---------------Logo Texture---------------//
    Intro_Logo        = Texture_Manager::GetInstance()->GetID("Intro_Logo");
    Intro_MadeBy      = Texture_Manager::GetInstance()->GetID("Intro_Made_By");
    Intro_Corparation = Texture_Manager::GetInstance()->GetID("Intro_Corporation");
    Intro_Name        = Texture_Manager::GetInstance()->GetID("Intro_Name");

    //---------------Intro Texture---------------//
    Intro_System    = Texture_Manager::GetInstance()->GetID("Intro_SystemOS");
    Intro_Dtat      = Texture_Manager::GetInstance()->GetID("Intro_DtatInit");
    Intro_Ready     = Texture_Manager::GetInstance()->GetID("Intro_Ready");
    Intro_Operation = Texture_Manager::GetInstance()->GetID("Intro_Oper");
    Intro_Nova      = Texture_Manager::GetInstance()->GetID("Intro_Nova");
    Intro_Flare     = Texture_Manager::GetInstance()->GetID("Intro_Flare");

    if (Title_BG == -1 || Title_Bracket == -1 || Title_Bracket_RGB == -1
        || Intro_Logo == -1 || Intro_MadeBy == -1 || Intro_Corparation == -1 || Intro_Name == -1
        || Intro_System == -1 || Intro_Dtat == -1 || Intro_Ready == -1
        || Intro_Nova == -1 || Intro_Operation == -1 || Intro_Flare == -1)
    {
        Debug::D_Out << "[Intro Title] Texture Init Error" << std::endl;
        Debug::D_Out << "Title_BG : "    << Title_BG          << "\tTitle_Bracket : "     << Title_Bracket
            << "\tIntro_Logo : "         << Intro_Logo        << "\tIntro_MadeBy : "     << Intro_MadeBy
            << "\tIntro_Corparation : "  << Intro_Corparation << "\tIntro_Name : "       << Intro_Name
            << "\tIntro_System : "       << Intro_System      << "\tIntro_Dtat : "       << Intro_Dtat
            << "\tIntro_Ready : "        << Intro_Ready       << "\tIntro_Nova : "       << Intro_Nova
            << "\tIntro_Flare : "         << Intro_Flare      << "\tTitle_Bracket_RGB :" << Title_Bracket_RGB
            << std::endl;
    }
}