// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASGameTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GASGameStatics.generated.h"

/**
 * 
 */
UCLASS()
class GASYSTEM_API UGASGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);
	
};
