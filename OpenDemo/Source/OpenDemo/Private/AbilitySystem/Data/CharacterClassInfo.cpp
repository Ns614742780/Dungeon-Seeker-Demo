#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	const FCharacterClassDefaultInfo* FindInfo = CharacterClassInformation.Find(CharacterClass);
	if (FindInfo)
	{
		return *FindInfo;
	}
	return FCharacterClassDefaultInfo();
}
