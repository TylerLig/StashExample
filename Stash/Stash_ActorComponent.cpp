#include "Stash_ActorComponent.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Items/ItemInstance.h"
#include "Cursor/Cursor_ActorComponent.h"
#include "Inventory/Inventory_ActorComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Stash_ActorComponent)

UStash_ActorComponent::UStash_ActorComponent(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    SetIsReplicatedByDefault(true);
	Stash_ItemSerializer = FStash_FastArraySerializer(FIntPoint(25, 30), this);
}

void UStash_ActorComponent::Server_MoveItemOnPositionToCursor_Implementation(FIntPoint Position)
{
	UCursor_ActorComponent* CursorComponent = GetCursorComponent();
	if(!CursorComponent || CursorComponent->IsItemOnCursor())
	{
		return;
	}

	UItemInstance* RemovedInstance = Stash_ItemSerializer.RemoveEntireItemByPosition(Position);
	if (RemovedInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(RemovedInstance);
	}
	CursorComponent->Server_MoveItemToCursor(RemovedInstance);
}

void UStash_ActorComponent::Server_MoveItemOnCursorToStashToPosition_Implementation(FIntPoint Position)
{
	UCursor_ActorComponent* CursorComponent = GetCursorComponent();
	if(!CursorComponent || !CursorComponent->IsItemOnCursor())
	{
		return;
	}

	UItemInstance* CursorInstance = CursorComponent->GetItemOnCursor();
	FAddItemResult Result = Stash_ItemSerializer.AddItemToPosition(CursorInstance, Position);
	if(Result.Success)
	{
		CursorComponent->Server_RemoveItemFromCursor();
	}
	if(Result.ItemToPutOnCursor != nullptr)
	{
		CursorComponent->Server_MoveItemToCursor(Result.ItemToPutOnCursor);
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(Result.AddedItemInstance);
		}
	}
}

void UStash_ActorComponent::Server_TransferItemOnPosition_Implementation(FIntPoint Position)
{
	UInventory_ActorComponent* InventoryComponent = GetInventoryComponent();
	if(!InventoryComponent)
	{
		return;
	}

	UItemInstance* InstanceToTransfer = Stash_ItemSerializer.GetItemByPosition(Position);
	bool AbleToTransfer = InventoryComponent->Server_IsThereAFreeSpaceForThisItem(InstanceToTransfer);
	if(AbleToTransfer)
	{
		UItemInstance* RemovedInstance = Stash_ItemSerializer.RemoveEntireItemByPosition(Position);
		bool Success = InventoryComponent->Server_AddItemToStash(RemovedInstance);
		if(!Success)
		{
			Server_AddItemToStash(RemovedInstance);
		}
		else
		{
			if (RemovedInstance && IsUsingRegisteredSubObjectList())
			{
				RemoveReplicatedSubObject(RemovedInstance);
			}
		}
	}
}

void UStash_ActorComponent::Server_SplitItemOnPositionToCursor_Implementation(FIntPoint Position, int32 Count)
{
	UCursor_ActorComponent* CursorComponent = GetCursorComponent();
	if(!CursorComponent || CursorComponent->IsItemOnCursor())
	{
		return;
	}

	FRemoveItemResult Result = Stash_ItemSerializer.RemoveItemsByPosition(Position, Count);
	if(!Result.Success)
	{
		return;
	}

	if (Result.RemovedEntireItem && Result.RemovedItem && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(Result.RemovedItem);
	}
	CursorComponent->Server_MoveItemToCursor(Result.RemovedItem);
}

bool UStash_ActorComponent::Server_AddItemToStash(UItemInstance* Instance)
{
	if (GetOwnerRole() != ROLE_Authority || Instance == nullptr)
	{
		return false;
	}

	FAddItemResult Result = Stash_ItemSerializer.AddItem(Instance);
	if (Result.Success && Result.AddedItemInstance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Result.AddedItemInstance);
	}
	return Result.Success;
}

bool UStash_ActorComponent::Server_IsThereAFreeSpaceForThisItem(UItemInstance* Instance)
{
	if (GetOwnerRole() != ROLE_Authority || Instance == nullptr)
	{
		return false;
	}

	return Stash_ItemSerializer.IsThereAFreeSpaceForThisItem(Instance);
}

const TArray<UItemInstance*> UStash_ActorComponent::GetAllItems() const
{
	return Stash_ItemSerializer.GetAllItems();
}

const FIntPoint& UStash_ActorComponent::GetStashSize()
{
	return Stash_ItemSerializer.GetStashSize();
}

UCursor_ActorComponent* UStash_ActorComponent::GetCursorComponent()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
    {
        return PlayerController->GetComponentByClass<UCursor_ActorComponent>();
    }
    return nullptr;
}

UInventory_ActorComponent* UStash_ActorComponent::GetInventoryComponent()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
    {
        return PlayerController->GetComponentByClass<UInventory_ActorComponent>();
    }
    return nullptr;
}

UStash_ActorComponent* UStash_ActorComponent::GetStashComponent()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
    {
        return PlayerController->GetComponentByClass<UStash_ActorComponent>();
    }
    return nullptr;
}

void UStash_ActorComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	if (IsUsingRegisteredSubObjectList())
	{
		const TArray<UItemInstance*>& AllItemInstances = Stash_ItemSerializer.GetAllItems();
		for (UItemInstance* Instance : AllItemInstances)
		{
			if (Instance && IsValid(Instance))
			{
				TArray<UItemFragment*> InstanceFragments = Instance->GetAllFragments();
				for(UItemFragment* Fragment : InstanceFragments)
				{
					AddReplicatedSubObject(Fragment);
				}
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UStash_ActorComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	const TArray<UItemInstance*>& AllItemInstances = Stash_ItemSerializer.GetAllItems();
	for (UItemInstance* Instance : AllItemInstances)
	{
		if (Instance && IsValid(Instance))
		{
			TArray<UItemFragment*> InstanceFragments = Instance->GetAllFragments();
			for(UItemFragment* Fragment : InstanceFragments)
			{
				WroteSomething |= Channel->ReplicateSubobject(Fragment, *Bunch, *RepFlags);
			}
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}

void UStash_ActorComponent::OnRep_StashData()
{
	FStashUpdateData StashData;
	StashData.Items = GetAllItems();
    OnStashUpdated.Broadcast(StashData);
}

void UStash_ActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, Stash_ItemSerializer);
}