// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASGameTypes.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASYSTEM_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	
	virtual void Init(TSubclassOf<UItemStaticData> ItemStaticDataClass);

	virtual bool IsSupportedForNetworking() const override {return true;}

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bEquipped = false;

	UFUNCTION()
	void OnRep_Equipped() {};

	virtual void OnEquipped() {};
	virtual void OnUnEquppied() {};
	
};
