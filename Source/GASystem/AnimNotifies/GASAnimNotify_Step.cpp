// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/GASAnimNotify_Step.h"

#include "GASystemCharacter.h"
#include "ActorComponent/GASFootstepsComponent.h"

void UGASAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	check(MeshComp);

	AGASystemCharacter* Character = Cast<AGASystemCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		if(UGASFootstepsComponent* FootstepsComponent = Character->GetFootstepsComponent())
		{
			FootstepsComponent->HandleFootStep(Foot);
		}
		
	}
}
