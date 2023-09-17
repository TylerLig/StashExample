Unreal Engine replication system and modular item classes to support a grid based inventory of items with rectangular sizes.

*   UItemDefinition composes a singular item type, contains the inherit unchanging data about the item type used to instantiate the item 

*   UStaticItemFragment is a modular way of defining static data designed in editor in a blueprint instance of ItemDefinition

*   UItemFragment is a modular way of defining replicated dynamic data

*   FFragment_FastArraySerializer replicates UItemFragments using fast TArray replication

*   UItemInstance is the grouping of static and dynamic data in the actual UObject representation of an item

*   FStash_FastArraySerializer replicates UItemInstances using fast TArray replication as well as maintaining the grid based inventory rules with item properties like size and stackable

*   Stash_ActorComponent is the interface for interacting with the grid based inventory through server authoritative RPCs


Example of a UI implementation of the system that demonstrates the replication network size is consistently small regardless of inventory size or fullness as only changed data is replicated to the client:


https://github.com/TylerLig/StashExample/assets/29814578/b0d64df2-17c4-47ec-b20e-a3abf1817b32


https://github.com/TylerLig/StashExample/assets/29814578/70b7a7e2-fd95-49da-87aa-903d5ae64931

