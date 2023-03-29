// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_WallRun.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TickWallRun.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_WallRun::UGA_WallRun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_WallRun::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	ACharacter* CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor);
	UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();

	CapsuleComponent->OnComponentHit.AddDynamic(this, &UGA_WallRun::OnCapsuleComponentHit);
}

void UGA_WallRun::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (ActorInfo)
	{
		ACharacter* CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor);
		UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();

		CapsuleComponent->OnComponentHit.RemoveDynamic(this, &UGA_WallRun::OnCapsuleComponentHit);
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGA_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	AGASystemCharacter* CharacterAvatar = GetActionGameCharacterFromActorInfo();
	return CharacterAvatar && !CharacterAvatar->GetCharacterMovement()->IsMovingOnGround();
}

void UGA_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WallRunTickTask = UAbilityTask_TickWallRun::CreateWallRunTask(this, Cast<ACharacter>(GetAvatarActorFromActorInfo()), Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent), Wall_Run_TraceObjectsTypes);

	WallRunTickTask->OnFinished.AddDynamic(this, &UGA_WallRun::K2_EndAbility);
	WallRunTickTask->OnWallSideDetermined.AddDynamic(this, &UGA_WallRun::OnWallSideDetermined);
	
	WallRunTickTask->ReadyForActivation();
}

void UGA_WallRun::OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbility(GetCurrentAbilitySpec()->Handle);
	}
}

void UGA_WallRun::OnWallSideDetermined(bool bLeftSide)
{
	AGASystemCharacter* CharacterAvatar = GetActionGameCharacterFromActorInfo();
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	if (bLeftSide)
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunLeftSideEffect, EffectContextHandle);
	}
	else
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunRightSideEffect, EffectContextHandle);
	}
}


void UGA_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(WallRunTickTask))
	{
		WallRunTickTask->EndTask();
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunLeftSideEffect, AbilitySystemComponent);
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunRightSideEffect, AbilitySystemComponent);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

