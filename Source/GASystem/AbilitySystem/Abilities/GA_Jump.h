// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GAS_GameplayAbility.h"
#include "GA_Jump.generated.h"

class UGameplayEffect;


UCLASS()
class GASYSTEM_API UGA_Jump : public UGAS_GameplayAbility
{
	GENERATED_BODY()

	UGA_Jump();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WallRunStateTag;

	UPROPERTY(EditDefaultsOnly)
	float offWallJumpStregth = 100.f;
	
};
