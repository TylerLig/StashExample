#pragma once

#include "Items/Fragments/ItemFragment.h"
#include "ItemFragment_Position.generated.h"

UCLASS()
class UItemFragment_Position : public UItemFragment
{
	GENERATED_BODY()
public:
	FIntPoint GetPosition();
	void SetPosition(const FIntPoint& InPosition);

private:
	UPROPERTY(Replicated)
	FIntPoint Position;
};
