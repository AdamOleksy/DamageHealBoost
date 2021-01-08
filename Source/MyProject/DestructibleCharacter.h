// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DestructibleCharacter.generated.h"

UCLASS()
class MYPROJECT_API ADestructibleCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float BaseHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 0.1f;

	float HP;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//UCapsuleComponent* DamageCapsuleComponent = nullptr;

	/* Used to handle counting down boost */
	FTimerHandle AttackTimeHandle;

public:
	// Sets default values for this character's properties
	ADestructibleCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnDamageTaken(float Damage);

	void AttackForvard();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP Parameters", meta = (AllowPrivateAccess = "true"))
	float HPProcentage;

};
