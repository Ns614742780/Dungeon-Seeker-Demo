#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenUserWidget.generated.h"

UCLASS()
class OPENDEMO_API UOpenUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
};
