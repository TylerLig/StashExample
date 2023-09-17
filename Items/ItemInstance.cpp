#include "ItemInstance.h"
#include "Items/ItemDefinition.h"
#include "Items/Fragments/ItemFragment_Position.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemInstance)

UItemInstance::UItemInstance(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemDefinition);
	DOREPLIFETIME(ThisClass, Fragment_FastArraySerializer);
}

void UItemInstance::SetItemDefinition(TSubclassOf<UItemDefinition> Definition)
{
	ItemDefinition = Definition;
}

FText UItemInstance::GetDisplayName()
{
	if (ItemDefinition != nullptr)
	{
		return GetDefault<UItemDefinition>(ItemDefinition)->DisplayName;
	}
	return FText();
}

UStaticItemFragment* UItemInstance::FindStaticFragmentByClass(TSubclassOf<UStaticItemFragment> FragmentClass) const
{
	if ((ItemDefinition != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UItemDefinition>(ItemDefinition)->FindStaticFragmentByClass(FragmentClass);
	}
	return nullptr;
}

UItemFragment* UItemInstance::FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const
{
	return Fragment_FastArraySerializer.FindFragmentByClass(FragmentClass);
}

void UItemInstance::AddFragment(UItemFragment* NewFragment)
{
	Fragment_FastArraySerializer.AddFragment(NewFragment);
}

TArray<UItemFragment*> UItemInstance::GetAllFragments() const
{
	return Fragment_FastArraySerializer.GetAllFragments();
}