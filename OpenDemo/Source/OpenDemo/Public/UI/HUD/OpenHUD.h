#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OpenHUD.generated.h"

class UOpenUserWidget;
class UOverlayWidgetController;
class UAbilitySystemComponent;
class UAttributeSet;
class UAttributeMenuWidgetController;

struct FWidgetControllerParams;

UCLASS()
class OPENDEMO_API AOpenHUD : public AHUD
{
	GENERATED_BODY()

public:

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);;
protected:


private:

	UPROPERTY()
	TObjectPtr<UOpenUserWidget> OverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOpenUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

public:
	FORCEINLINE UOpenUserWidget* GetOpenOverlay() const { return OverlayWidget; }
};
