## Unreal Engine Replication System for Grid-Based Inventory

This project provides a robust replication system and modular item classes for managing a grid-based inventory in Unreal Engine. Items can have rectangular sizes, and both static and dynamic data are supported. The system ensures efficient network communication by only replicating changed data to clients.

### Core Components

- **UItemDefinition** - Defines a singular item type, containing static, immutable data used to instantiate the item.
  
- **UStaticItemFragment** - A modular class for defining static data, designed in the editor using a blueprint instance of `UItemDefinition`.
  
- **UItemFragment** - A modular class for defining replicated dynamic data.
  
- **FFragment_FastArraySerializer** - Utilizes Unreal's Fast TArray replication system to replicate `UItemFragment` instances efficiently.
  
- **UItemInstance** - Represents the actual item, grouping static and dynamic data into a single `UObject`.
  
- **FStash_FastArraySerializer** - Utilizes Unreal's Fast TArray replication to manage `UItemInstance` objects. It maintains grid-based inventory rules, such as item size and stackability.
  
- **Stash_ActorComponent** - Provides the interface for interacting with the grid-based inventory through server-authoritative RPCs (Remote Procedure Calls).

### UI Implementation Example

An example UI implementation demonstrates the efficiency of the replication system. The network size remains consistently small regardless of inventory size or fullness, as only the changed data is replicated to the client.


https://github.com/TylerLig/StashExample/assets/29814578/b0d64df2-17c4-47ec-b20e-a3abf1817b32


https://github.com/TylerLig/StashExample/assets/29814578/70b7a7e2-fd95-49da-87aa-903d5ae64931
