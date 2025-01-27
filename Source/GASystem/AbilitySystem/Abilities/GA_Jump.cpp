// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Jump.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Jump::UGA_Jump()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	const bool bMovementAllowsJump = Character->GetCharacterMovement()->IsJumpAllowed();

	UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
	const bool bIsWallRunning = AbilityComponent->HasMatchingGameplayTag(WallRunStateTag);
	
	return Character->CanJump() || (bMovementAllowsJump && bIsWallRunning);
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

		ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		
		UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
		const bool bIsWallRunning = AbilityComponent->HasMatchingGameplayTag(WallRunStateTag);
		if (bIsWallRunning)
		{
			FGameplayTagContainer WallRunTags(WallRunStateTag);
			AbilityComponent->CancelAbilities(&WallRunTags);

			FVector JumpoffVector = Character->GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal() + FVector::UpVector;

			Character->LaunchCharacter(JumpoffVector * offWallJumpStregth, true, true);
		}
		else
		{
			Character->Jump();
		}
	}
}

