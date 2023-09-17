#pragma once

#include "Items/ItemDefinition.h"
#include "StaticItemFragment_Stackable.generated.h"

UCLASS()
class UStaticItemFragment_Stackable : public UStaticItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 MaxStackSize;
};
