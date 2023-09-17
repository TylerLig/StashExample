#pragma once

#include "Items/ItemDefinition.h"
#include "StaticItemFragment_Icon.generated.h"

struct FButtonStyle;

UCLASS()
class UStaticItemFragment_Icon : public UStaticItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FButtonStyle Icon;
};
