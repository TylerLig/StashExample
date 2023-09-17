#pragma once

#include "Items/Fragments/ItemFragment.h"
#include "ItemFragment_StackCount.generated.h"

UCLASS()
class UItemFragment_StackCount : public UItemFragment
{
	GENERATED_BODY()
public:
	int32 GetCount();
	void SetCount(int32 InCount);

private:
	UPROPERTY(Replicated)
	int32 Count;
};
