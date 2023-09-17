#include "ItemFragment.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemFragment)

void FFragment_FastArraySerializer::AddFragment(UItemFragment* InFragment)
{
    TSubclassOf<UItemFragment> InClass = InFragment->GetClass();
    if (!ClassToIndexMap.Contains(InClass))
    {
        FFragment_FastArraySerializerItem& NewItem = Fragments.AddDefaulted_GetRef();
        NewItem.Fragment = InFragment;
        MarkItemDirty(NewItem);
        int32 NewIndex = Fragments.Num() - 1;
        ClassToIndexMap.Add(InClass, NewIndex);
    }
}

void FFragment_FastArraySerializer::RemoveFragment(TSubclassOf<UItemFragment> FragmentClass)
{
    if (ClassToIndexMap.Contains(FragmentClass))
    {
        int32 Index = ClassToIndexMap[FragmentClass];
        Fragments.RemoveAt(Index);
        ClassToIndexMap.Remove(FragmentClass);
        MarkArrayDirty();
    }
}

UItemFragment* FFragment_FastArraySerializer::FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const
{
    if (ClassToIndexMap.Contains(FragmentClass))
    {
        return Fragments[ClassToIndexMap[FragmentClass]].Fragment;
    }
    return nullptr;
}

TArray<UItemFragment*> FFragment_FastArraySerializer::GetAllFragments() const
{
    TArray<UItemFragment*> Output;
    for(const FFragment_FastArraySerializerItem& Fragment : Fragments)
    {
        Output.Add(Fragment.Fragment);
    }
    return Output;
}

bool FFragment_FastArraySerializer::Contains(TSubclassOf<UItemFragment> FragmentClass) const
{
    return ClassToIndexMap.Contains(FragmentClass);
}

void FFragment_FastArraySerializer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
        FFragment_FastArraySerializerItem& RemovedItem = Fragments[Index];
        ClassToIndexMap.Remove(RemovedItem.Fragment->GetClass());
	}
}

void FFragment_FastArraySerializer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
        FFragment_FastArraySerializerItem& AddedItem = Fragments[Index];
        ClassToIndexMap.Add(AddedItem.Fragment->GetClass(), Index);
	}
}

void FFragment_FastArraySerializer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
	}
}