// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASGameTypes.h"
#include "Components/ActorComponent.h"
#include "GASFootstepsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASYSTEM_API UGASFootstepsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGASFootstepsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	FName LeftFootSocketName = TEXT("foot_l");

	UPROPERTY(EditDefaultsOnly)
	FName RightFootSocketName = TEXT("foot_r");

	
public:
	void HandleFootStep(EFoot Foot);
};
