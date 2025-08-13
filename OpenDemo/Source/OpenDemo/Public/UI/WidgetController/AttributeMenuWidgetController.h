#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/OpenWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayTag;
struct FGameplayAttribute;
struct FOpenAttributeInfo;

class UAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FOpenAttributeInfo&, Info);


UCLASS(BlueprintType, Blueprintable)
class OPENDEMO_API UAttributeMenuWidgetController : public UOpenWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnPlayerStatChangedSignature AttributePointChangedDelegate;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
	
};
