#include "Map/ModuleState.h"

#include "OpenGameplayTags.h"
#include "OpenDemo/OpenLogChannels.h"

FGameplayTag UModuleState::GetSelfConnectTagByDirection(const FOpenModuleState& ModuleState, const int32 Direction)
{
	FGameplayTag ConnectTag;

	switch (Direction)
	{
	case 0:
		ConnectTag = ModuleState.Connect_Front_ID;
		break;
	case 1:
		ConnectTag = ModuleState.Connect_Right_ID;
		break;
	case 2:
		ConnectTag = ModuleState.Connect_Up_ID;
		break;
	case 3:
		ConnectTag = ModuleState.Connect_Back_ID;
		break;
	case 4:
		ConnectTag = ModuleState.Connect_Left_ID;
		break;
	case 5:
		ConnectTag = ModuleState.Connect_Down_ID;
		break;
	default:
		ConnectTag = FGameplayTag();
	}
	return ConnectTag;
}

FOpenModuleState UModuleState::SetRotation(const FOpenModuleState& ModuleState, const int32 InRotation)
{
	FOpenModuleState NewModuleState = ModuleState;
	NewModuleState.Mesh = ModuleState.Mesh;
	NewModuleState.Weight = ModuleState.Weight;
	NewModuleState.Rotation = InRotation;

	// 按照旋转设置水平方向的 Tag
	if (InRotation == 0)
	{
		NewModuleState.Connect_Front_ID = ModuleState.Connect_Front_ID;
		NewModuleState.Connect_Right_ID = ModuleState.Connect_Right_ID;
		NewModuleState.Connect_Back_ID = ModuleState.Connect_Back_ID;
		NewModuleState.Connect_Left_ID = ModuleState.Connect_Left_ID;
	}
	else if (InRotation == 1)
	{
		NewModuleState.Connect_Front_ID = ModuleState.Connect_Right_ID;
		NewModuleState.Connect_Right_ID = ModuleState.Connect_Back_ID;
		NewModuleState.Connect_Back_ID = ModuleState.Connect_Left_ID;
		NewModuleState.Connect_Left_ID = ModuleState.Connect_Front_ID;
	}
	else if (InRotation == 2)
	{
		NewModuleState.Connect_Front_ID = ModuleState.Connect_Back_ID;
		NewModuleState.Connect_Back_ID = ModuleState.Connect_Front_ID;
		NewModuleState.Connect_Left_ID = ModuleState.Connect_Right_ID;
		NewModuleState.Connect_Right_ID = ModuleState.Connect_Left_ID;
	}
	else if (InRotation == 3)
	{
		NewModuleState.Connect_Front_ID = ModuleState.Connect_Left_ID;
		NewModuleState.Connect_Left_ID = ModuleState.Connect_Back_ID;
		NewModuleState.Connect_Back_ID = ModuleState.Connect_Right_ID;
		NewModuleState.Connect_Right_ID = ModuleState.Connect_Front_ID;
	}
	return NewModuleState;
}
