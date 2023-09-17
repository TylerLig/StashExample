#include "Stash_FastArraySerializer.h"
#include "Items/ItemInstance.h"
#include "Items/ItemDefinition.h"
#include "Items/Fragments/StaticItemFragment_Stackable.h"
#include "Items/Fragments/StaticItemFragment_Size.h"
#include "Items/Fragments/ItemFragment_Position.h"
#include "Items/Fragments/ItemFragment_StackCount.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Stash_FastArraySerializer)

const FIntPoint& FStash_FastArraySerializer::GetStashSize()
{
	return StashSize;
}

FAddItemResult FStash_FastArraySerializer::AddItem(UItemInstance* Instance)
{
    UItemFragment_StackCount* StackCountFragment = Instance->FindFragmentByClass<UItemFragment_StackCount>();
	if(!StackCountFragment)
	{
		return FAddItemResult();
	}
	const UStaticItemFragment_Stackable* StackableFragment = Instance->FindStaticFragmentByClass<UStaticItemFragment_Stackable>();
    return AddItemInternal(Instance->GetItemDefinition(), StackCountFragment->GetCount(), StackableFragment);
}

FAddItemResult FStash_FastArraySerializer::AddItem(TSubclassOf<UItemDefinition> Definition, int32 CountToAdd)
{
    const UStaticItemFragment_Stackable* StackableFragment = UItemDefinition::FindStaticFragmentByClass<UStaticItemFragment_Stackable>(Definition);
    return AddItemInternal(Definition, CountToAdd, StackableFragment);
}

FAddItemResult FStash_FastArraySerializer::AddItemInternal(TSubclassOf<UItemDefinition> Definition, int32 Count, const UStaticItemFragment_Stackable* StackableFragment)
{
	if(Count <= 0)
	{
		return FAddItemResult();
	}

	int32 RemainingCount = Count;
    if (StackableFragment)
    {
        for (FStash_FastArraySerializerItem& ExistingItem : Items)
        {
            if (ExistingItem.Instance->GetItemDefinition() == Definition)
            {
                UItemFragment_StackCount* StackCountFragment = ExistingItem.Instance->FindFragmentByClass<UItemFragment_StackCount>();
                int32 CurrentCount = StackCountFragment->GetCount();
                int32 AmountToAdd = FMath::Min(StackableFragment->MaxStackSize - CurrentCount, RemainingCount);
                StackCountFragment->SetCount(CurrentCount + AmountToAdd);
                RemainingCount -= AmountToAdd;
                MarkItemDirty(ExistingItem);

                if (RemainingCount == 0)
                {
					FAddItemResult Output;
					Output.Success = true;
                    return Output;
                }
            }
        }
    }

    return AddItemToFirstFreePosition(Definition, RemainingCount);
}

FAddItemResult FStash_FastArraySerializer::AddItemToFirstFreePosition(TSubclassOf<UItemDefinition> Definition, int32 Count)
{
	UItemInstance* Instance = NewObject<UItemInstance>(OwnerComponent->GetOwner());
	Instance->SetItemDefinition(Definition);
	for (UStaticItemFragment* Fragment : GetDefault<UItemDefinition>(Definition)->Fragments)
	{
		Fragment->OnInstanceCreated(Instance);
	}

	UItemFragment_StackCount* NewStackCountFragment = NewObject<UItemFragment_StackCount>(OwnerComponent->GetOwner());
	NewStackCountFragment->SetCount(Count);
	Instance->AddFragment(NewStackCountFragment);

	UItemFragment_Position* NewPositionFragment = NewObject<UItemFragment_Position>(OwnerComponent->GetOwner());
	Instance->AddFragment(NewPositionFragment);

	return AddItemToFirstFreePosition(Instance);
}

FAddItemResult FStash_FastArraySerializer::AddItemToFirstFreePosition(UItemInstance* Instance)
{
	FIntPoint FirstFreePosition = FindFirstFreePositionForItem(Instance->GetItemDefinition());
	if(FirstFreePosition == FIntPoint(-1, -1))
	{
		return FAddItemResult();
	}

	return AddItemToPosition(Instance, FirstFreePosition);
}

FAddItemResult FStash_FastArraySerializer::AddItemToPosition(UItemInstance* Instance, const FIntPoint& Position)
{
    const FIntPoint Size = Instance->FindStaticFragmentByClass<UStaticItemFragment_Size>()->Size;

	if(!IsInBounds(Position, Size))
	{
		return FAddItemResult();
	}

	TArray<FStash_FastArraySerializerItem> ScanResult = ScanForItems(Position, Size);
    if (ScanResult.Num() > 1)
    {
		return FAddItemResult();
    }

	FAddItemResult Output;
	if(ScanResult.Num() == 1)
	{
		UItemInstance* FoundItem = ScanResult[0].Instance;
		if(FoundItem->GetItemDefinition() == Instance->GetItemDefinition())
		{
			const UStaticItemFragment_Stackable* FoundItemStackableFragment = FoundItem->FindStaticFragmentByClass<UStaticItemFragment_Stackable>();
			if(FoundItemStackableFragment)
			{
				UItemFragment_StackCount* FoundItemStackCountFragment = FoundItem->FindFragmentByClass<UItemFragment_StackCount>();
				UItemFragment_StackCount* StackCountFragment = Instance->FindFragmentByClass<UItemFragment_StackCount>();
				int32 FoundItemCurrentCount = FoundItemStackCountFragment->GetCount();
				if(FoundItemCurrentCount != FoundItemStackableFragment->MaxStackSize)
				{
					int32 CurrentCount = StackCountFragment->GetCount();
					int32 AmountToAdd = FMath::Min(FoundItemStackableFragment->MaxStackSize - FoundItemCurrentCount, CurrentCount);
					FoundItemStackCountFragment->SetCount(FoundItemCurrentCount + AmountToAdd);
					MarkItemDirty(ScanResult[0]);
					Output.Success = true;
					if(CurrentCount == AmountToAdd)
					{
						return Output;
					}

					StackCountFragment->SetCount(CurrentCount - AmountToAdd);
					Output.ItemToPutOnCursor = Instance;
					return Output;
				}
			}
		}
		Output.ItemToPutOnCursor = RemoveEntireItemByInstance(FoundItem);
	}

	UItemFragment_Position* PositionFragment = Instance->FindFragmentByClass<UItemFragment_Position>();
	PositionFragment->SetPosition(Position);

	FStash_FastArraySerializerItem& NewItem = Items.AddDefaulted_GetRef();
	NewItem.Instance = Instance;
	MarkItemDirty(NewItem);
	ReIndexPositionMap();
	Output.AddedItemInstance = NewItem.Instance;
	Output.Success = true;
	return Output;
}

UItemInstance* FStash_FastArraySerializer::RemoveEntireItemByPosition(const FIntPoint& Position)
{
	if(PositionMap.Contains(Position))
	{
		int32 ItemIndex = PositionMap[Position];
        UItemInstance* Item = Items[ItemIndex].Instance;
        Items.RemoveAt(ItemIndex);
		MarkArrayDirty();
		ReIndexPositionMap();
		return Item;
	}
	return nullptr;
}

UItemInstance* FStash_FastArraySerializer::GetItemByPosition(const FIntPoint& Position)
{
	if(PositionMap.Contains(Position))
	{
        return Items[PositionMap[Position]].Instance;
	}
	return nullptr;
}

UItemInstance* FStash_FastArraySerializer::RemoveEntireItemByInstance(UItemInstance* Instance)
{
	for (auto ItemIterator = Items.CreateIterator(); ItemIterator; ++ItemIterator)
	{
		UItemInstance* Item = (*ItemIterator).Instance;
		if (Item == Instance)
		{
			ItemIterator.RemoveCurrent();
			MarkArrayDirty();
			ReIndexPositionMap();
			return Item;
		}
	}
	return nullptr;
}

FRemoveItemResult FStash_FastArraySerializer::RemoveItemsByPosition(const FIntPoint& Position, int32 CountToRemove)
{
	if(!PositionMap.Contains(Position))
	{
       return FRemoveItemResult();
	}

	int32 ItemIndex = PositionMap[Position];
	FStash_FastArraySerializerItem& Item = Items[ItemIndex];

	const UStaticItemFragment_Stackable* StackableFragment = Item.Instance->FindStaticFragmentByClass<UStaticItemFragment_Stackable>();
	if(!StackableFragment)
	{
		return FRemoveItemResult();
	}

	UItemFragment_StackCount* StackCountFragment = Item.Instance->FindFragmentByClass<UItemFragment_StackCount>();
	int32 CurrentCount = StackCountFragment->GetCount();
	CountToRemove = FMath::Min(CurrentCount, CountToRemove);
	int32 NewCount = CurrentCount - CountToRemove;
	FRemoveItemResult Output;
	if (NewCount == 0)
	{
		Output.RemovedItem = Item.Instance;
		Items.RemoveAt(ItemIndex);
		MarkArrayDirty();
		ReIndexPositionMap();
	}
	else
	{
		StackCountFragment->SetCount(NewCount);
		MarkItemDirty(Item);

		UItemInstance* NewInstance = NewObject<UItemInstance>(OwnerComponent->GetOwner());
		NewInstance->SetItemDefinition(Item.Instance->GetItemDefinition());
		for (UStaticItemFragment* Fragment : GetDefault<UItemDefinition>(NewInstance->GetItemDefinition())->Fragments)
		{
			Fragment->OnInstanceCreated(NewInstance);
		}

		UItemFragment_StackCount* NewStackCountFragment = NewObject<UItemFragment_StackCount>(OwnerComponent->GetOwner());
		NewStackCountFragment->SetCount(CountToRemove);
		NewInstance->AddFragment(NewStackCountFragment);

		UItemFragment_Position* NewPositionFragment = NewObject<UItemFragment_Position>(OwnerComponent->GetOwner());
		NewInstance->AddFragment(NewPositionFragment);
		Output.RemovedItem = NewInstance;
	}
	Output.Success = true;
	return Output;
}


TArray<FStash_FastArraySerializerItem> FStash_FastArraySerializer::ScanForItems(const FIntPoint& Position, const FIntPoint& Size)
{
	TArray<FStash_FastArraySerializerItem> Output;
	TSet<int32> FoundIndices;
	for (int32 Y = 0; Y < Size.Y; Y++)
    {
        for (int32 X = 0; X < Size.X; X++)
        {
            const FIntPoint PositionToCheck(Position.X + X, Position.Y + Y);
            const int32* FoundIndexPtr = PositionMap.Find(PositionToCheck);

            if (FoundIndexPtr)
            {
				const int32 FoundIndex = *FoundIndexPtr;
				if(!FoundIndices.Contains(FoundIndex))
				{
					FoundIndices.Add(FoundIndex);
				}
            }
        }
    }

	for(int32 FoundIndex : FoundIndices)
	{
		Output.Add(Items[FoundIndex]);
	}
	return Output;
}

bool FStash_FastArraySerializer::IsThereAFreeSpaceForThisItem(UItemInstance* Instance)
{
	return IsThereAFreeSpaceForThisItem(Instance->GetItemDefinition());
}

bool FStash_FastArraySerializer::IsThereAFreeSpaceForThisItem(TSubclassOf<UItemDefinition> Definition)
{
	return FindFirstFreePositionForItem(Definition) != FIntPoint(-1, -1);
}

FIntPoint FStash_FastArraySerializer::FindFirstFreePositionForItem(TSubclassOf<UItemDefinition> Definition)
{
	const FIntPoint Size = UItemDefinition::FindStaticFragmentByClass<UStaticItemFragment_Size>(Definition)->Size;
	for (int32 Y = 0; Y <= StashSize.Y - Size.Y; Y++)
    {
        for (int32 X = 0; X <= StashSize.X - Size.X; X++)
        {
            if (IsThisAFreeSpace(FIntPoint(X, Y), Size))
            {
                return FIntPoint(X, Y);
            }
        }
    }
	return FIntPoint(-1, -1);
}

bool FStash_FastArraySerializer::IsInBounds(const FIntPoint& Position, const FIntPoint& Size)
{
	if (Position.X < 0 || Position.Y < 0 || Position.X + Size.X > StashSize.X || Position.Y + Size.Y > StashSize.Y)
    {
        return false;
    }
	return true;
}

bool FStash_FastArraySerializer::IsThisAFreeSpace(const FIntPoint& Position, const FIntPoint& Size)
{
	if (!IsInBounds(Position, Size))
    {
        return false;
    }

	for (int32 Y = 0; Y < Size.Y; Y++)
	{
		for (int32 X = 0; X < Size.X; X++)
		{
			if (PositionMap.Contains(FIntPoint(Position.X + X, Position.Y + Y)))
			{
				return false;
			}
		}
	}
	return true;
}

const TArray<UItemInstance*> FStash_FastArraySerializer::GetAllItems() const
{
	TArray<UItemInstance*> Results;
	Results.Reserve(Items.Num());
	for (const FStash_FastArraySerializerItem& Item : Items)
	{
		Results.Add(Item.Instance);	
	}
	return Results;
}

void FStash_FastArraySerializer::AddToPositionMap(UItemInstance* Instance, int32 Index)
{
	const FIntPoint Size = Instance->FindStaticFragmentByClass<UStaticItemFragment_Size>()->Size;
	const FIntPoint Position = Instance->FindFragmentByClass<UItemFragment_Position>()->GetPosition();
	for (int32 Y = Position.Y; Y < Position.Y + Size.Y; Y++)
	{
		for (int32 X = Position.X; X < Position.X + Size.X; X++)
		{
			PositionMap.Add(FIntPoint(X, Y), Index);
		}
	}
}
void FStash_FastArraySerializer::ReIndexPositionMap()
{
    PositionMap.Empty();
    for(int32 Index = 0; Index < Items.Num(); Index++)
    {
        AddToPositionMap(Items[Index].Instance, Index);
    }
}

void FStash_FastArraySerializer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		//FStash_FastArraySerializerItem& Item = Items[Index];
	}
}
void FStash_FastArraySerializer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		//FStash_FastArraySerializerItem& Item = Items[Index];
	}
}
void FStash_FastArraySerializer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		//FStash_FastArraySerializerItem& Item = Items[Index];
	}
}