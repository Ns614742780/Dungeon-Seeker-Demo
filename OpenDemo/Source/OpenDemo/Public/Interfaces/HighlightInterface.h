#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

class OPENDEMO_API IHighlightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void HighlightActor() = 0;
	virtual void UnhighlightActor() = 0;
};
