// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/GAS_AnimInstance.h"

#include "GASystemCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UGAS_AnimInstance::GetLocomotionBlendspace() const
{
	if(AGASystemCharacter* GASCharacter = Cast<AGASystemCharacter>(GetOwningActor()))
	{
		FCharacterData Data = GASCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("null blendspace"));
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace : nullptr;
}

UAnimSequenceBase* UGAS_AnimInstance::GetIdleAnimation() const
{
	if(AGASystemCharacter* GASCharacter = Cast<AGASystemCharacter>(GetOwningActor()))
	{
		FCharacterData Data = GASCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("null idle"));
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset-> CharacterAnimationData.IdleAnimationAsset : nullptr;
}
