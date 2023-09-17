#pragma once
#include "Items/Fragments/ItemFragment.h"
#include "ItemInstance.generated.h"

class UItemDefinition;
class UStaticItemFragment;

UCLASS(BlueprintType)
class UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool IsSupportedForNetworking() const override { return true; }

	TSubclassOf<UItemDefinition> GetItemDefinition() const
	{
		return ItemDefinition;
	}

	FText GetDisplayName();	

	UFUNCTION(meta=(DeterminesOutputType=FragmentClass))
	UStaticItemFragment* FindStaticFragmentByClass(TSubclassOf<UStaticItemFragment> FragmentClass) const;

	template <typename ResultClass>
	ResultClass* FindStaticFragmentByClass() const
	{
		return (ResultClass*)FindStaticFragmentByClass(ResultClass::StaticClass());
	}

	UFUNCTION(meta=(DeterminesOutputType=FragmentClass))
	UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;

	template <typename ResultClass>
	ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	TArray<UItemFragment*> GetAllFragments() const;
	void AddFragment(UItemFragment* NewFragment);

private:
	void SetItemDefinition(TSubclassOf<UItemDefinition> Definition);

	friend struct FStash_FastArraySerializer;

	UPROPERTY(Replicated)
	TSubclassOf<UItemDefinition> ItemDefinition;

	UPROPERTY(Replicated)
	FFragment_FastArraySerializer Fragment_FastArraySerializer;
};
