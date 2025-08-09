#include "OpenAssetManager.h"

#include "OpenGameplayTags.h"

UOpenAssetManager& UOpenAssetManager::Get()
{
	check(GEngine);

	UOpenAssetManager* OpenAssetManager = Cast<UOpenAssetManager>(GEngine->AssetManager);
	check(OpenAssetManager);
	return *OpenAssetManager;
}

void UOpenAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	FOpenGameplayTags::InitializeNativeGameplayTags();
}
