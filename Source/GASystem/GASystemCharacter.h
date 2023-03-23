// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GASGameTypes.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Abilities/GameplayAbility.h"
#include "GASystemCharacter.generated.h"

// Forward declarations
class UGAS_AbilitySystemComponent;
class UGAS_AttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;

UCLASS(config=Game)
class AGASystemCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchInputAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintInputAction;

public:
	AGASystemCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
	
	// Override for IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// Applies a gameplay effect to the character's ability system component
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void OnJumpActionStarted(const FInputActionValue& Value);

	void OnJumpActionEnded(const FInputActionValue& Value);
	
	void OnCrouchActionStarted(const FInputActionValue& Value);

	void OnCrouchActionEnded(const FInputActionValue& Value);

	void OnSprintActionStarted(const FInputActionValue& Value);

	void OnSprintActionEnded(const FInputActionValue& Value);
	// Initializes the character's attributes
	//void InitializeAttributes();

	
	// Gives the character its default abilities
	void GiveAbilities();

	// Applies default gameplay effects to the character's ability system component
	void ApplyStartupEffects();

	// Overrides for AActor
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	/*
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly ,Category = "GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributeSet;

	
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly ,Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly ,Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
	*/

	UPROPERTY(EditDefaultsOnly)
	UGAS_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	UGAS_AttributeSetBase* AttributeSetBase;

	UPROPERTY(EditDefaultsOnly)
	class UGASFootstepsComponent* FootstepsComponent;

protected:
	// Overrides for APawn
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Called when the character begins play
	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

	class UGASFootstepsComponent* GetFootstepsComponent() const;

	void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(Replicated = OnRep_CharacterData)
	FCharacterData CharacterData;

	UFUNCTION()
	void OnRep_CharacterData();
	
	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

	UPROPERTY(EditDefaultsOnly)
	class UCharacterDataAsset* CharacterDataAsset;

	//Gameplay Events

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	//GameplayTags
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

	//Gameplay Effects

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

	FDelegateHandle MaxMovementSpeedChangedDelegateHandle;
};
