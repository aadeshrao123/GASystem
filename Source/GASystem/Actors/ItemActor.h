// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GASGameTypes.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UInventoryItemInstance;

UCLASS()
class GASYSTEM_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();
	virtual void OnEquipped();
	virtual void OnUnEquipped();
	virtual void OnDropped();


	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void Init(UInventoryItemInstance* InInstance);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	UInventoryItemInstance* ItemInstance = nullptr;

	UPROPERTY(ReplicatedUSing = OnRep_ItemState)
	TEnumAsByte<EItemState> ItemState = EItemState::None;

	UFUNCTION()
	void OnRep_ItemState();

	UPROPERTY()
	USphereComponent* SphereComponent = nullptr;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
