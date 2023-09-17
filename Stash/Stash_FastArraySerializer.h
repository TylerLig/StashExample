#pragma once
#include "Net/Serialization/FastArraySerializer.h"
#include "Stash_FastArraySerializer.generated.h"

class UItemInstance;
class UItemDefinition;
class UStaticItemFragment_Stackable;

USTRUCT()
struct FStash_FastArraySerializerItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FStash_FastArraySerializerItem(){}

	UPROPERTY()
	TObjectPtr<UItemInstance> Instance = nullptr;
};

USTRUCT()
struct FAddItemResult
{
	GENERATED_BODY()

	bool Success = false;
	TObjectPtr<UItemInstance> ItemToPutOnCursor;
	TObjectPtr<UItemInstance> AddedItemInstance;
};

USTRUCT()
struct FRemoveItemResult
{
	GENERATED_BODY()

	bool Success = false;
	bool RemovedEntireItem = false;
	TObjectPtr<UItemInstance> RemovedItem;
};

USTRUCT()
struct FStash_FastArraySerializer : public FFastArraySerializer
{
	GENERATED_BODY()

	FStash_FastArraySerializer(): OwnerComponent(nullptr)
	{}

	FStash_FastArraySerializer(FIntPoint InSize, UActorComponent* InOwnerComponent): StashSize(InSize), OwnerComponent(InOwnerComponent)
	{}

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FStash_FastArraySerializerItem, FStash_FastArraySerializer>(Items, DeltaParms, *this);
	}

	FAddItemResult AddItem(TSubclassOf<UItemDefinition> Definition, int32 CountToAdd);
	FAddItemResult AddItem(UItemInstance* Instance);
	FAddItemResult AddItemToPosition(UItemInstance* Instance, const FIntPoint& Position);
	UItemInstance* RemoveEntireItemByInstance(UItemInstance* Instance);
	UItemInstance* RemoveEntireItemByPosition(const FIntPoint& Position);
	UItemInstance* GetItemByPosition(const FIntPoint& Position);
	bool IsThereAFreeSpaceForThisItem(TSubclassOf<UItemDefinition> Definition);
	bool IsThereAFreeSpaceForThisItem(UItemInstance* Instance);
	
	FRemoveItemResult RemoveItemsByPosition(const FIntPoint& Position, int32 CountToRemove);

	const TArray<UItemInstance*> GetAllItems() const;
	const FIntPoint& GetStashSize();

private:
	UPROPERTY()
	TArray<FStash_FastArraySerializerItem> Items;

	UPROPERTY()
    FIntPoint StashSize;
	
	UPROPERTY(NotReplicated)
	TMap<FIntPoint, int32> PositionMap;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	FAddItemResult AddItemInternal(TSubclassOf<UItemDefinition> Definition, int32 Count, const UStaticItemFragment_Stackable* StackableFragment);
	TArray<FStash_FastArraySerializerItem> ScanForItems(const FIntPoint& Position, const FIntPoint& Size);
	bool IsThisAFreeSpace(const FIntPoint& Position, const FIntPoint& Size);
	bool IsInBounds(const FIntPoint& Position, const FIntPoint& Size);
	FIntPoint FindFirstFreePositionForItem(TSubclassOf<UItemDefinition> Definition);
	FAddItemResult AddItemToFirstFreePosition(TSubclassOf<UItemDefinition> Definition, int32 Count);
	FAddItemResult AddItemToFirstFreePosition(UItemInstance* Instance);
	void AddToPositionMap(UItemInstance* Instance, int32 Index);
	void ReIndexPositionMap();
};

template<>
struct TStructOpsTypeTraits<FStash_FastArraySerializer> : public TStructOpsTypeTraitsBase2<FStash_FastArraySerializer>
{
	enum { WithNetDeltaSerializer = true };
};