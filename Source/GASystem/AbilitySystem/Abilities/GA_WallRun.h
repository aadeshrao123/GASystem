// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GAS_GameplayAbility.h"
#include "GA_WallRun.generated.h"

class UAbilityTask_TickWallRun;

UCLASS()
class GASYSTEM_API UGA_WallRun : public UGAS_GameplayAbility
{
	GENERATED_BODY()
	UGA_WallRun();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION()
	void OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnWallSideDetermined(bool bLeftSide);

	UPROPERTY()
	UAbilityTask_TickWallRun* WallRunTickTask = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> Wall_Run_TraceObjectsTypes;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> WallRunLeftSideEffect;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> WallRunRightSideEffect;
	
	
};
