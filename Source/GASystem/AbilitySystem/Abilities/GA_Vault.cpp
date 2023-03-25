// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Vault.h"

#include "GASystemCharacter.h"
#include "SAdvancedTransformInputBox.h"
#include "ActorComponent/GAS_MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"



UGA_Vault::UGA_Vault()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	if(!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
	{
		return false;
	}
	AGASystemCharacter* Character = GetActionGameCharacterFromActorInfo();

	if (!IsValid(Character))
	{
		return false;
	}
	
	const FVector StartLocation = Character->GetActorLocation();
	const FVector ForwardVector = Character->GetActorForwardVector();
	const FVector UpVector = Character->GetActorUpVector();

	TArray<AActor*> ActorsToIgnore = {Character};

	static const auto CVar =IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
	const bool bShowTraversal = CVar->GetInt() > 0;
	
	EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bJumpToLocationSet = false;

	int32 JumpTOLocationIdx = INDEX_NONE;

	int i = 0;

	FHitResult TraceHit;

	float MaxjumpDistance = HorizontalTraceLength;

	for (; i < HorizontalTraceCount; i++)
	{
		const FVector TraceStart = StartLocation + i * UpVector * HorizontalTraceStep;
		const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;

		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true))
		{
			if (JumpTOLocationIdx == INDEX_NONE && (i < HorizontalTraceCount - 1))
			{
				JumpTOLocationIdx = i;
				JumpToLocation = TraceHit.Location;
			}	
		
			else if(JumpTOLocationIdx == (i - 1))
			{
				MaxjumpDistance = FVector::Dist2D(TraceHit.Location, TraceStart);
				break;
			}
		}
		else
		{
			if (JumpTOLocationIdx != INDEX_NONE)
			{
				break;
			}
		}
	}
	if (JumpTOLocationIdx == INDEX_NONE)
	{
		return false;
	}
	const float DistanceToJump = FVector::Dist2D(StartLocation, JumpToLocation);

	const float MaxVerticalTraceDistance = MaxjumpDistance - DistanceToJump;
	if (MaxVerticalTraceDistance < 0)
	{
		return false;
	}
	if (i == HorizontalTraceCount)
	{
		i = HorizontalTraceCount - 1;
	}
	const float VerticalTraceLength = FMath::Abs(JumpToLocation.Z - (StartLocation + i * UpVector * HorizontalTraceStep).Z);
	
	FVector VerticalStartLocation = JumpToLocation + UpVector * VerticalTraceLength;

	i = 0;
	const float VerticalTraceCount = MaxVerticalTraceDistance / VerticalTraceStep;

	bool bJumpOverLocationSet = false;

	for (; i <= VerticalTraceCount; ++i)
	{
		const FVector TraceStart = VerticalStartLocation + i * VerticalTraceStep;
		const FVector TraceEnd = TraceStart + UpVector * VerticalTraceLength * -1;

		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true))
		{
			JumpOverLocation = TraceHit.ImpactPoint;
			if (i == 0)
			{
				JumpToLocation = JumpOverLocation;
			}
			else if(i != 0)
			{
				bJumpOverLocationSet = true;
				break;
			}
		}
	}
	if (!bJumpOverLocationSet)
	{
		return false;
	}

	const FVector TraceStart = JumpOverLocation + ForwardVector * VerticalTraceStep;

	if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, JumpOverLocation, HorizontalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true))
	{
		JumpOverLocation = TraceHit.ImpactPoint;
	}
	if (bShowTraversal)
	{
		DrawDebugSphere(GetWorld(), JumpToLocation, 15, 16, FColor::White, false, 7);
		DrawDebugSphere(GetWorld(), JumpOverLocation, 15, 16, FColor::White, false, 7);
	}
	return true;
}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		K2_EndAbility();
		return;
	}
	AGASystemCharacter* Character = GetActionGameCharacterFromActorInfo();

	UCharacterMovementComponent* CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr;

	if (CharacterMovement)
	{
		CharacterMovement->SetMovementMode(MOVE_Flying);
	}
	UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr;

	if (CapsuleComponent)
	{
		for (ECollisionChannel Channel : CollisionChannelsToIgnore)
		{
			CapsuleComponent->SetCollisionResponseToChannel(Channel, ECollisionResponse::ECR_Ignore);
		}
	}
	UGAS_MotionWarpingComponent* MotionWarpingComponent = Character ? Character->GetGASMotionWarpingComponent() : nullptr;

	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpToLocation"), JumpToLocation, Character->GetActorRotation());
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpOverLocation"), JumpOverLocation, Character->GetActorRotation());
	}
	
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, VaultMontage);

	MontageTask->OnBlendOut.AddDynamic(this, &UGA_Vault::K2_EndAbility);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Vault::K2_EndAbility);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Vault::K2_EndAbility);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Vault::K2_EndAbility);
	MontageTask->ReadyForActivation();
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(MontageTask))
	{
		MontageTask->EndTask();
	}

	AGASystemCharacter* Character = GetActionGameCharacterFromActorInfo();

	UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr;

	if (CapsuleComponent)
	{
		for (ECollisionChannel Channel : CollisionChannelsToIgnore)
		{
			CapsuleComponent->SetCollisionResponseToChannel(Channel, ECollisionResponse::ECR_Block);
		}
	}

	UCharacterMovementComponent* CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr;

	if (CharacterMovement && CharacterMovement->IsFlying())
	{
		CharacterMovement->SetMovementMode(MOVE_Falling);
	}

	UGAS_MotionWarpingComponent* MotionWarpingComponent = Character ? Character->GetGASMotionWarpingComponent() : nullptr;

	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpToLocation"));
		MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpOverLocation"));
	}

	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
