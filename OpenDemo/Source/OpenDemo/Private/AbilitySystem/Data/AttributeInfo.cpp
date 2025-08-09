#include "AbilitySystem/Data/AttributeInfo.h"

FOpenAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
    for(const FOpenAttributeInfo& Info : AttributeInformation)
    {
        if (Info.AttributeTag.MatchesTagExact(AttributeTag))
        {
            return Info;
        }
	}

    if(bLogNotFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("AttributeInfo not found for tag: %s"), *AttributeTag.ToString());
    }
    return FOpenAttributeInfo();
}
