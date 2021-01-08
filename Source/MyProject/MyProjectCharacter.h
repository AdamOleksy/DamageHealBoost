// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyProjectCharacter.generated.h"


UENUM(BlueprintType)
enum class EItemClass : uint8
{
	NONE,
	SPEED_BOOSTER
};

class AActorGrenade;
class AItemBase;

UCLASS(config=Game)
class AMyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* GrenadeSpawnPoint;
	
	/* Use to boost set in editor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Parameters", meta = (AllowPrivateAccess = "true"))
	float SpeedMultipier = 1.f;

	/* Use to boost set in editor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Parameters", meta = (AllowPrivateAccess = "true"))
	float PlayerBaseWalkSpeed = 600.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	float PlayerBaseHP = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	float PlayerCurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	float BoostDuration = 5.f;

	/* Boost time left */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	float BoostCountDown;

	float BoostCountDownInterval = 0.1f;

	float PlayerAttackTolerance = 45.f;

	/* Used to handle counting down boost */
	FTimerHandle BoostTimeHandle;

	/*Reset player speed */
	void ResetBoost();

	/*To check if boost time have end*/
	void CheckBoostTimeLeft();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	class UInventory* CharacterInventory;

	bool bIsInventoryActivated = false;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InventoryOnOff();

public:
	AMyProjectCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintCallable)
	void PickUp(EItemClass Item);

	UFUNCTION(BlueprintCallable)
	void OnDamageTaken(float Damage);

	UFUNCTION(BlueprintCallable)
	void OnHealed(float Heal);

	UFUNCTION(BlueprintNativeEvent)
	void KillPlayer();
	void KillPlayer_Implementation();

	UFUNCTION(BlueprintCallable)
	void PlayerAttack();

	UPROPERTY(Replicated, EditAnywhere, Category = "Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActorGrenade> GrenadeBP;

	UFUNCTION(BlueprintCallable)
	void PickUpItem(AItemBase* Item);

	UFUNCTION(BlueprintCallable)
	void ThrowOutItem(AItemBase* Item);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//UFUNCTION(NetMulticast, Reliable, WithValidation)
	UFUNCTION(unreliable, server, WithValidation)
	void Throw();
	void Throw_Implementation();
	bool Throw_Validate();

	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(class AItemBase* Item);

	UPROPERTY(ReplicatedUsing = PlayThrowAnimation)
	bool bIsTrowing = false;

	FTimerHandle ResetTrowingTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* ThrowAnimationRef;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, NetMulticast, Reliable, WithValidation)
	void SpawnGrenade();

	UFUNCTION(Client, Reliable, WithValidation)
	void PlayThrowAnimation();
	void PlayThrowAnimation_Implementation();
	bool PlayThrowAnimation_Validate();
	
	void ResetIsTrowing();

	UPROPERTY()
	bool bIsShoping;

	UPROPERTY()
	class AShop* ActiveShop;

	UFUNCTION()
	void SetActiveShop(AShop* NewShop);

	UFUNCTION()
	void LeaveShop();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Money")
	int32 Money = 1000;
};

