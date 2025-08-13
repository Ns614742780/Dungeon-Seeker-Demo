#include "Map/Module.h"

AModule::AModule()
{
	
}

FGameplayTag AModule::GetSelfConnectTagByDirection(const int32 Direction)
{
	FGameplayTag RealConnectTag;
	int32 RealDirection = Direction;
	if (IsHorizontalDirection(Direction))
	{
		// 如果旋转0,则映射关系不变,如果90则循环后移一位
		RealDirection = GetHorizontalDirection()[(Direction + ModuleState.Rotation) % 4];
	}

	switch (RealDirection)
	{
	case 0:
		return ModuleState.Connect_Front_ID;
		break;
	case 1:
		return ModuleState.Connect_Right_ID;
		break;
	case 2:
		return ModuleState.Connect_Up_ID;
		break;
	case 3:
		return ModuleState.Connect_Back_ID;
		break;
	case 4:
		return ModuleState.Connect_Left_ID;
		break;
	case 5:
		return ModuleState.Connect_Down_ID;
		break;
	default:
		return FGameplayTag();
	}
	
}

const TArray<int32>& AModule::GetHorizontalDirection()
{
	static const TArray<int32> HorizontalDirection = { 0, 1, 3, 4 };
	return HorizontalDirection;
}

bool AModule::IsHorizontalDirection(int32 Direction)
{
	return Direction != 2 && Direction != 5;
}

