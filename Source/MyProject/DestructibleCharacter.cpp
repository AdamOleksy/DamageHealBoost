// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleCharacter.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "MyProjectCharacter.h"

// Sets default values
ADestructibleCharacter::ADestructibleCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DamageCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	//DamageCapsuleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADestructibleCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(AttackTimeHandle, this, &ADestructibleCharacter::AttackForvard, 1.f, false);
	HP = BaseHP;
	HPProcentage = HP / BaseHP;
}

// Called every frame
void ADestructibleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation
	(
		FRotator(0.f, RotationSpeed, 0.f)
	);

}

// Called to bind functionality to input
void ADestructibleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADestructibleCharacter::OnDamageTaken(float Damage)
{
	HP -= Damage;
	HPProcentage = HP / BaseHP;
}

void ADestructibleCharacter::AttackForvard()
{
	TArray<FHitResult> OutHits;
	FVector InitLocation = GetActorLocation();
	FVector DirectionVector = GetActorForwardVector();
	FVector AttackEnd = ((DirectionVector * 1500.f) + InitLocation);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), InitLocation, AttackEnd, FColor::Red, false, 0.1f, 0, 5.f);

	bool isHitting = GetWorld()->LineTraceMultiByChannel(OutHits, InitLocation, AttackEnd, ECC_Visibility, CollisionParams);

	if (isHitting)
	{
		for (auto& Hit : OutHits)
		{
			if (Hit.Actor->GetName() != GetName())
			{
				ADestructibleCharacter* DC = Cast<ADestructibleCharacter>(Hit.GetActor());
				AMyProjectCharacter* PC = Cast<AMyProjectCharacter>(Hit.GetActor());
				if (DC)
				{
					DC->OnDamageTaken(10.f);
				}
				if (PC)
				{
					PC->OnDamageTaken(10.f);
				}
			}
			
		}
	}
	GetWorldTimerManager().SetTimer(AttackTimeHandle, this, &ADestructibleCharacter::AttackForvard, 1.f, false);
}