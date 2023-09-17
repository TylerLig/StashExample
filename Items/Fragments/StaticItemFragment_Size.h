#pragma once

#include "Items/ItemDefinition.h"
#include "StaticItemFragment_Size.generated.h"

UCLASS()
class UStaticItemFragment_Size : public UStaticItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint Size;
};
