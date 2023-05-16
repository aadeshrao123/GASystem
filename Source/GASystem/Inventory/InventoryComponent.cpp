// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/InventoryItemInstance.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "Actors/ItemActor.h"

FGameplayTag UInventoryComponent::EquipItemActorTag;
FGameplayTag UInventoryComponent::DropItemTag;
FGameplayTag UInventoryComponent::EquipNextTag;
FGameplayTag UInventoryComponent::UnEquipTag;


static TAutoConsoleVariable<int32> CVarShowInventory(
	TEXT("ShowDebugInventory"),
	0,
	TEXT("Draws debug info about inventory")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat
);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

	UGameplayTagsManager::Get().OnLastChanceToAddNativeTags().AddUObject(this, &UInventoryComponent::AddInventoryTags);
	// ...
}

void UInventoryComponent::AddInventoryTags()
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	UInventoryComponent::EquipItemActorTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"), TEXT("Equip Item from item actor event"));
	UInventoryComponent::DropItemTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.DropItem"), TEXT("Drop Equipped Item"));
	UInventoryComponent::EquipNextTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipNext"), TEXT("Try Equipe Next item"));
	UInventoryComponent::UnEquipTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.UnEquipTag"), TEXT("UnEquip Current Item"));

	TagsManager.OnLastChanceToAddNativeTags().RemoveAll(this);
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (GetOwner()->HasAuthority())
	{
		for (auto ItemClass : DefaultsItems)
		{
			InventoryList.AddItem(ItemClass);
		}
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::EquipItemActorTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::DropItemTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::EquipNextTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::UnEquipTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething =  Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (FInventoryListItem& Item : InventoryList.GetItemsRef())
	{
		UInventoryItemInstance* ItemInstance = Item.ItemInstance;
		if (IsValid(ItemInstance))
		{
			WroteSomething = Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}



// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() != 0;
	if (bShowDebug)
	{
		for (FInventoryListItem& Item : InventoryList.GetItemsRef())
		{
			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			const UItemStaticData* ItemStaticData = ItemInstance->GetItemStaticData();

			if (IsValid(ItemInstance) && IsValid(ItemStaticData))
			{
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("Ttem: %s"), *ItemStaticData->Name.ToString()));
			}
		}
	}
}

void UInventoryComponent::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if (GetOwner()->HasAuthority())
	{
		InventoryList.AddItem(InItemStaticDataClass);
	}
}

void UInventoryComponent::AddItemInstance(UInventoryItemInstance* InItemInstance)
{
	if (GetOwner()->HasAuthority())
	{
		InventoryList.AddItem(InItemInstance);
	}
}

void UInventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if (GetOwner()->HasAuthority())
	{
		InventoryList.RemoveItem(InItemStaticDataClass);
	}
}

void UInventoryComponent::EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if (GetOwner()->HasAuthority())
	{
		for (auto Item : InventoryList.GetItemsRef())
		{
			if(Item.ItemInstance->ItemStaticDataClass == InItemStaticDataClass)
			{
				Item.ItemInstance->OnEquipped(GetOwner());
				CurrentItem = Item.ItemInstance;
				break;
			}
		}
	}
}

void UInventoryComponent::EquipItemInstance(UInventoryItemInstance* InItemInstance)
{
	if (GetOwner()->HasAuthority())
	{
		for (auto Item : InventoryList.GetItemsRef())
		{
			if (Item.ItemInstance == InItemInstance)
			{
				Item.ItemInstance->OnEquipped(GetOwner());
				CurrentItem = Item.ItemInstance;
				break;
			}
		}
	}
}

void UInventoryComponent::EquipNext()
{
	TArray<FInventoryListItem>& Items = InventoryList.GetItemsRef();

	const bool bNoItems = Items.Num() == 0;
	const bool bOneAndEquipped = Items.Num() == 1 && CurrentItem;

	if (bNoItems || bOneAndEquipped) return;

	UInventoryItemInstance* TargetItem = CurrentItem;

	for (auto Item : Items)
	{
		if (Item.ItemInstance->GetItemStaticData()->bCanBeEquipped)
		{
			if (Item.ItemInstance != CurrentItem)
			{
				TargetItem = Item.ItemInstance;
				break;
			}
		}
	}
	if (CurrentItem)
	{
		if (TargetItem == CurrentItem)
		{
			return;;
		}
		UnEquipItem();
	}
	EquipItemInstance(TargetItem);
}


void UInventoryComponent::UnEquipItem()
{
	if (GetOwner()->HasAuthority())
	{
		if (IsValid(CurrentItem))
		{
			CurrentItem->OnUnEquppied();
			CurrentItem = nullptr;
		}
	}
}

void UInventoryComponent::DropItem()
{
	if (GetOwner()->HasAuthority())
	{
		if (IsValid(CurrentItem))
		{
			CurrentItem->OnDropped();
			RemoveItem(CurrentItem->ItemStaticDataClass);
			CurrentItem = nullptr;
		}
	}
}


UInventoryItemInstance* UInventoryComponent::GetEquippedItem() const
{
	return CurrentItem;
}

void UInventoryComponent::GameplayEventCallback(const FGameplayEventData* Payload)
{
	ENetRole NetRole =  GetOwnerRole();

	if (NetRole == ROLE_Authority)
	{
		HandleGameplayEventInternal(*Payload);
	}
	else if (NetRole == ROLE_AutonomousProxy)
	{
		ServerHandleGameplayEvent(*Payload);
	}
}

void UInventoryComponent::HandleGameplayEventInternal(FGameplayEventData Payload)
{
	ENetRole NetRole =  GetOwnerRole();
	if (NetRole == ROLE_Authority)
	{
		FGameplayTag EventTag = Payload.EventTag;
		if (EventTag == UInventoryComponent::EquipItemActorTag)
		{
			if (const UInventoryItemInstance* ItemInstance = Cast<UInventoryItemInstance>(Payload.OptionalObject))
			{
				AddItemInstance(const_cast<UInventoryItemInstance*>(ItemInstance));
				if (Payload.Instigator)
				{
					Payload.Instigator->Destroy();
				}
			}
		}
		else if (EventTag == UInventoryComponent::EquipNextTag)
		{
			EquipNext();
		}
		else if(EventTag == UInventoryComponent::DropItemTag)
		{
			DropItem();
		}
		else if (EventTag == UInventoryComponent::UnEquipTag)
		{
			UnEquipItem();
		}
	}

}

void UInventoryComponent::ServerHandleGameplayEvent_Implementation(FGameplayEventData Payload)
{
	HandleGameplayEventInternal(Payload);
}


void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
	DOREPLIFETIME(UInventoryComponent, CurrentItem);
	
}
