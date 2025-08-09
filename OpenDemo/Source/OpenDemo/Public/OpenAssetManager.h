#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "OpenAssetManager.generated.h"

UCLASS()
class OPENDEMO_API UOpenAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	// from self
	static UOpenAssetManager& Get();
	
protected:

	// from UAssetManager
	virtual void StartInitialLoading() override;
	
};
