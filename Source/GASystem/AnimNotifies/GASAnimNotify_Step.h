// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASGameTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GASAnimNotify_Step.generated.h"

/**
 * 
 */
UCLASS()
class GASYSTEM_API UGASAnimNotify_Step : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;	

	UPROPERTY(EditAnywhere)
	EFoot Foot;

};
