/*==============================================================================

	Manage Action Mapping & Input [Input_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

class Input_Manager
{
public:
	static Input_Manager* GetInstance();

	Input_Manager(const Input_Manager&) = delete;
	Input_Manager& operator=(const Input_Manager&) = delete;

	// ---------------------------------------------------------//
	//						   Movement							//
	// ---------------------------------------------------------//
	bool Is_Up_Trigger() const;
	bool Is_Down_Trigger() const;
	bool Is_Left_Trigger() const;
	bool Is_Right_Trigger() const;

	bool Is_Up_Pressed() const;
	bool Is_Down_Pressed() const;
	bool Is_Left_Pressed() const;
	bool Is_Right_Pressed() const;

	// ---------------------------------------------------------//
	//						   Action							//
	// ---------------------------------------------------------//
	bool Is_Confirm_Trigger() const;
	bool Is_Cancel_Trigger() const;
	bool Is_Escape_Trigger() const;

	// ---------------------------------------------------------//
	//						   Combat							//
	// ---------------------------------------------------------//
	bool Is_Weapon_Changed() const;

	// ---------------------------------------------------------//
	//						Mouse Input							//
	// ---------------------------------------------------------//
	DirectX::XMFLOAT2 Mouse_Position() const;
	bool Is_Mouse_Moved() const;
	bool Is_Mouse_In_Rect(float x, float y, float w, float h) const;

	bool Is_Mouse_Left_Trigger() const;
	bool Is_Mouse_Right_Trigger() const;

	bool Is_Mouse_Left_Pressed() const;
	bool Is_Mouse_Right_Pressed() const;

	bool Is_Mouse_Visible();
	void Set_Mouse_Visible_Mode(bool Visible);

	// ---------------------------------------------------------//
	//					   Controller Input						//
	// ---------------------------------------------------------//
	DirectX::XMFLOAT2 Controller_Input_L_Stick() const;
	DirectX::XMFLOAT2 Controller_Input_R_Stick() const;
	BYTE Controlle_Left_Trigger() const;
	BYTE Controlle_Right_Trigger() const;

	// ---------------------------------------------------------//
	//						   Debug							//
	// ---------------------------------------------------------//
	bool Debug_Cancel_Trigger() const;

private:
	Input_Manager() = default;
	~Input_Manager() = default;
};

#define M_INPUT Input_Manager::GetInstance()

#endif // INPUT_MANAGER_H