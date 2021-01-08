// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "CollisionShape.h"
#include "../DestructibleCharacter.h"
#include "../MyProjectCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
AActorGrenade::AActorGrenade()
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = GrenadeMesh;

	GrenadeMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Grenade Movement"));
	GrenadeMovement->InitialSpeed = MovementSpeed;
	GrenadeMovement->MaxSpeed = MovementSpeed;
	InitialLifeSpan = 3.f;

	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
}

// Called when the game starts or when spawned
void AActorGrenade::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(ExplosionTimeHandle, this, &AActorGrenade::Explosion, ExplosionTime, false);
	
}

// Called every frame
void AActorGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorGrenade::Explosion_Implementation()
{

}

bool AActorGrenade::Explosion_Validate()
{
	UE_LOG(LogTemp, Warning, TEXT("In speed %f"), MovementSpeed);
	TArray<APawn> PawnsInExplosion;

	FVector InitLocation = GetActorLocation();
	FVector PlayerDirectionVector = GetActorForwardVector();
	FCollisionShape CollisionShere = FCollisionShape::MakeSphere(500.f);
	DrawDebugSphere(GetWorld(), InitLocation, CollisionShere.GetSphereRadius(), 10, FColor::Red, false, 1.f);
	TArray<FHitResult> MultipleHits;
	bool isHit = GetWorld()->SweepMultiByChannel(MultipleHits, InitLocation, InitLocation, FQuat::Identity, ECC_Pawn, CollisionShere);

	if (isHit)
	{
		uint8 i = 0;
		for (auto& Hit : MultipleHits)
		{
			ADestructibleCharacter* DC = Cast<ADestructibleCharacter>(Hit.GetActor());
			AMyProjectCharacter* PC = Cast<AMyProjectCharacter>(Hit.GetActor());
			if (DC)
			{
				DC->OnDamageTaken(10.f);
				UE_LOG(LogTemp, Warning, TEXT("DestructibleCharacter %s hit by explosion"), *DC->GetName());
			}
			if (PC)
			{
				i++;
				PC->OnDamageTaken(10.f);
				UE_LOG(LogTemp, Warning, TEXT("DestructibleCharacter %s hit by explosion"), *PC->GetName(), i);
			}
		}
	}
	Destroy();

	return true;
}