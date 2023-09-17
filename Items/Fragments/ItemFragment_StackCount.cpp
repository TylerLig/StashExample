#include "ItemFragment_StackCount.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemFragment_StackCount)

void UItemFragment_StackCount::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Count, Params);
}

int32 UItemFragment_StackCount::GetCount()
{
	return Count;
}

void UItemFragment_StackCount::SetCount(int32 InCount)
{
	Count = InCount;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Count, this);
}