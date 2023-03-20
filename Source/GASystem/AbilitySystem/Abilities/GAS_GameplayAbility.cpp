// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAS_GameplayAbility.h"

#include "AbilitySystemComponent.h"

void UGAS_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();

	for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
	{
		if (!GameplayEffect.Get()) continue;
		if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (!ActiveGEHandle.WasSuccessfullyApplied())
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to apply jump effect! %s"), *GetNameSafe(GameplayEffect));
				}
			}
		}
	}

	if (IsInstantiated())
	{
		for (auto GameplayEffect : OngoingEffectsToRemoveOnEnd)
		{
			if (!GameplayEffect.Get()) continue;
			if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					if (!ActiveGEHandle.WasSuccessfullyApplied())
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to apply jump effect! %s"), *GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEndEffectHandles.Add(ActiveGEHandle);
					}
				}
			}
		}
	}
	
}

void UGAS_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	for (FActiveGameplayEffectHandle ActiveEffectHandle : RemoveOnEndEffectHandles)
	{
		if (ActiveEffectHandle.IsValid())
		{
			ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
		}
		RemoveOnEndEffectHandles.Empty();

	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

AGASystemCharacter* UGAS_GameplayAbility::GetActionGameCharacterFromActorInfo() const
{
	return Cast<AGASystemCharacter>(GetAvatarActorFromActorInfo());
}
