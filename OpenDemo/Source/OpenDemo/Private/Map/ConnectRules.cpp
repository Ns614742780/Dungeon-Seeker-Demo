#include "Map/ConnectRules.h"

FGameplayTagsArray UConnectRules::GetConnectTag(FGameplayTag InTag)
{
	return ConnectRule[InTag];
}
