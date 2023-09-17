#include "ItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemDefinition)

UItemDefinition::UItemDefinition(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

UStaticItemFragment* UItemDefinition::FindStaticFragmentByClass(TSubclassOf<UStaticItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UStaticItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}