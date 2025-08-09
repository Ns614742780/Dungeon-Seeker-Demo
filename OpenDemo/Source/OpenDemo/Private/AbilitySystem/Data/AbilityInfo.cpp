#include "AbilitySystem/Data/AbilityInfo.h"

#include "OpenDemo/OpenLogChannels.h"

FOpenAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FOpenAbilityInfo& Info : AbilityInfos)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogOpen, Error, TEXT("Can't find the info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this))
	}
	return FOpenAbilityInfo();
}
