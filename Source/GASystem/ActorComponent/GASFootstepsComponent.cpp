// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/GASFootstepsComponent.h"

#include "GASystemCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterial/GAS_PhysicalMaterial.h"

static TAutoConsoleVariable<int32> CVarShowfootsteps(
	TEXT("ShowDebugFootsteps"),
	0,
	TEXT("Draws debug info about footsteps")
	TEXT("  0: off/n")
	TEXT("  1: on/n"),
	ECVF_Cheat);

// Sets default values for this component's properties
UGASFootstepsComponent::UGASFootstepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UGASFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGASFootstepsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGASFootstepsComponent::HandleFootStep(EFoot Foot)
{
	if(AGASystemCharacter* Character = Cast<AGASystemCharacter>(GetOwner()))
	{
		const int32 DebugShowFootsteps =CVarShowfootsteps.GetValueOnAnyThread();
		
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -50.f, ECollisionChannel::ECC_WorldStatic, QueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					if (HitResult.PhysMaterial.Get())
					{
						UGAS_PhysicalMaterial* PhysicalMaterial = Cast<UGAS_PhysicalMaterial>(HitResult.PhysMaterial.Get());

						if (PhysicalMaterial)
						{
							UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootStepSound, Location, 1.f);
						}
						if (DebugShowFootsteps > 0)
						{
							DrawDebugString(GetWorld(), Location, GetNameSafe(PhysicalMaterial), nullptr, FColor::White,4.f);
						}
					}

					if (DebugShowFootsteps > 0)
					{
						DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
					}
				}
				else
				{
					if (DebugShowFootsteps > 0)
					{
						DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector -50.f, FColor::Red, false, 4.f, 0, 1);
					}
				}
			}
			else
			{
				if (DebugShowFootsteps > 0)
				{
					DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector -50.f, FColor::Red, false, 4.f, 0, 1);
					DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);

				}
			}
		}
	}
}

