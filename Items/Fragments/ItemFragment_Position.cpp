#include "ItemFragment_Position.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemFragment_Position)

void UItemFragment_Position::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Position, Params);
}

FIntPoint UItemFragment_Position::GetPosition()
{
	return Position;
}

void UItemFragment_Position::SetPosition(const FIntPoint& InPosition)
{
	Position = InPosition;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Position, this);
}
