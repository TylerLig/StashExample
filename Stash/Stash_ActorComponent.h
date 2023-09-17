#pragma once

#include "Components/ActorComponent.h"
#include "Stash/Stash_FastArraySerializer.h"
#include "Stash_ActorComponent.generated.h"

class UItemInstance;
class UCursor_ActorComponent;
class UInventory_ActorComponent;

USTRUCT()
struct FStashUpdateData
{
	GENERATED_BODY()

	TArray<TObjectPtr<UItemInstance>> Items;
};

UINTERFACE(MinimalAPI)
class UStashInterface : public UInterface
{
	GENERATED_BODY()
};

class IStashInterface
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStashUpdated, FStashUpdateData&);
	FOnStashUpdated OnStashUpdated;

	UFUNCTION(Server, Reliable)
	virtual void Server_MoveItemOnPositionToCursor(FIntPoint Position);

	UFUNCTION(Server, Reliable)
	virtual void Server_MoveItemOnCursorToStashToPosition(FIntPoint Position);

	UFUNCTION(Server, Reliable)
	virtual void Server_TransferItemOnPosition(FIntPoint Position);

	UFUNCTION(Server, Reliable)
	virtual void Server_SplitItemOnPositionToCursor(FIntPoint Position, int32 Count);

	virtual const FIntPoint& GetStashSize() = 0;
};

UCLASS()
class UStash_ActorComponent : public UActorComponent, public IStashInterface
{
    GENERATED_BODY()

public:
    UStash_ActorComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(Server, Reliable)
	void Server_MoveItemOnPositionToCursor(FIntPoint Position);

	UFUNCTION(Server, Reliable)
	void Server_MoveItemOnCursorToStashToPosition(FIntPoint Position);

	UFUNCTION(Server, Reliable)
	void Server_TransferItemOnPosition(FIntPoint Position);

	UFUNCTION(Server, Reliable)
	void Server_SplitItemOnPositionToCursor(FIntPoint Position, int32 Count);

	bool Server_AddItemToStash(UItemInstance* Instance);
	bool Server_IsThereAFreeSpaceForThisItem(UItemInstance* Instance);

	const FIntPoint& GetStashSize();

    virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;

protected:
	const TArray<UItemInstance*> GetAllItems() const;
	bool IsItemOnCursor();
	UItemInstance* GetItemOnCursor();
	UCursor_ActorComponent* GetCursorComponent();
	UInventory_ActorComponent* GetInventoryComponent();
	UStash_ActorComponent* GetStashComponent();

    UPROPERTY(ReplicatedUsing = OnRep_StashData)
    FStash_FastArraySerializer Stash_ItemSerializer;

    UFUNCTION()
    void OnRep_StashData();
};