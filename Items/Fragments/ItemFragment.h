#pragma once
#include "Net/Serialization/FastArraySerializer.h"
#include "ItemFragment.generated.h"

UCLASS(Abstract)
class UItemFragment : public UObject
{
	GENERATED_BODY()
	
	public:
	virtual bool IsSupportedForNetworking() const override { return true; }
};

USTRUCT()
struct FFragment_FastArraySerializerItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FFragment_FastArraySerializerItem(){}

private:
	friend FFragment_FastArraySerializer;

	UPROPERTY()
	TObjectPtr<UItemFragment> Fragment = nullptr;
};

USTRUCT()
struct FFragment_FastArraySerializer : public FFastArraySerializer
{
	GENERATED_BODY()

	FFragment_FastArraySerializer()
	{}

public:
    void AddFragment(UItemFragment* InFragment);
    void RemoveFragment(TSubclassOf<UItemFragment> FragmentClass);
    UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;
	TArray<UItemFragment*> GetAllFragments() const;
    bool Contains(TSubclassOf<UItemFragment> FragmentClass) const;

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FFragment_FastArraySerializerItem, FFragment_FastArraySerializer>(Fragments, DeltaParms, *this);
	}

private:
	UPROPERTY()
	TArray<FFragment_FastArraySerializerItem> Fragments;

    TMap<TSubclassOf<UItemFragment>, int32> ClassToIndexMap;
};

template<>
struct TStructOpsTypeTraits<FFragment_FastArraySerializer> : public TStructOpsTypeTraitsBase2<FFragment_FastArraySerializer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};