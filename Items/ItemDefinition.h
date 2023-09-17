#pragma once

#include "ItemDefinition.generated.h"

class UItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UStaticItemFragment : public UObject
{
	GENERATED_BODY()
};

UCLASS(Blueprintable, Const, Abstract)
class UItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UStaticItemFragment>> Fragments;

	UStaticItemFragment* FindStaticFragmentByClass(TSubclassOf<UStaticItemFragment> FragmentClass) const;

	template <typename ResultClass>
	static ResultClass* FindStaticFragmentByClass(TSubclassOf<UItemDefinition> ItemDefinition)
	{
		UStaticItemFragment* FoundFragment = GetDefault<UItemDefinition>(ItemDefinition)->FindStaticFragmentByClass(ResultClass::StaticClass());
		if (FoundFragment)
		{
			return Cast<ResultClass>(FoundFragment);
		}
		return nullptr;
	}
};
