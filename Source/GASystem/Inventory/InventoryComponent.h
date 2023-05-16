// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryList.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void InitializeComponent() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	
	UFUNCTION(BlueprintCallable)
	void AddItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
	void EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	
	UFUNCTION(BlueprintCallable)
	void EquipItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
	void UnEquipItem();

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable)
	void EquipNext();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventoryItemInstance* GetEquippedItem() const;

	virtual void GameplayEventCallback(const FGameplayEventData* Payload);

	static FGameplayTag EquipItemActorTag;
	static FGameplayTag DropItemTag;
	static FGameplayTag EquipNextTag;
	static FGameplayTag UnEquipTag;
	
protected:

	UPROPERTY(Replicated)
	FInventoryList InventoryList;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UItemStaticData>> DefaultsItems;

	UPROPERTY(Replicated)
	UInventoryItemInstance* CurrentItem = nullptr;

	UFUNCTION()
	void AddInventoryTags();

	FDelegateHandle TagDelegateHandle;

	void HandleGameplayEventInternal(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerHandleGameplayEvent(FGameplayEventData Payload);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
