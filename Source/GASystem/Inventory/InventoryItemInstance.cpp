// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "GASGameStatics.h"
#include "Actors/ItemActor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	ItemStaticDataClass = InItemStaticDataClass;
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
	return UGASGameStatics::GetItemStaticData(ItemStaticDataClass);
}

void UInventoryItemInstance::OnRep_Equipped()
{
	
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
	if (UWorld* World = GetWorld())
	{
		const UItemStaticData* StaticData = GetItemStaticData();
		
		FTransform Transform;
		ItemActor =	World->SpawnActorDeferred<AItemActor>(StaticData->ItemActorClass, Transform, InOwner);
		ItemActor->Init(this);

		ItemActor->OnEquipped();
		
		ItemActor->FinishSpawning(Transform);

		ACharacter* Character = Cast<ACharacter>(InOwner);
		if (USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr)
		{
			ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, StaticData->AttachmentSocket);
			UE_LOG(LogTemp, Warning, TEXT("Something happening"))
		}
	}
	bEquipped = true;

}

void UInventoryItemInstance::OnUnEquppied()
{
	if (ItemActor)
	{
		ItemActor->Destroy();
		ItemActor = nullptr;
	}

	bEquipped = false;
}

void UInventoryItemInstance::OnDropped()
{
	if (ItemActor)
	{
		ItemActor->OnDropped();
	}
	bEquipped = false;
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bEquipped);
	DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}
